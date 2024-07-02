#define TESTING

#include <stdio.h>
#include <iostream>
#include <config.hpp>
#include <commands.hpp>
#include <types.hpp>
#include <errors.hpp>
#include "stdarg.h"
//#include <ClearCore.h>

using namespace std;

int nprintf(const char * format, ...) {
    va_list arg_list;
    va_start(arg_list, format);
    bool net = false;
    ClearCore::EthernetTCPServer * server;
    if (format[0] == AT) {
        net = true;
        server = va_arg(arg_list, ClearCore::EthernetTCPServer *);
        format ++;
    }
    char output_buffer[MAX_BUFFER];
    int chars = vsnprintf(output_buffer, MAX_BUFFER, format, arg_list);
    if (net) server->Send(&output_buffer[0]);
    cout << output_buffer;
    return chars;
}

class Parser {
public:
    Parser(ClearCore::EthernetTCPServer* server_ref){
        Parser::available = true;
        Parser::config = config;
        server = server_ref;
    }
    ~Parser(){
        return;
    }

    int32_t parse_command(char * input_command){
        uint32_t result;
        if (config_mode){
            if ((input_command[0] == C) && (input_command[1] == S)){
                config_mode = false;
                nprintf("Configuration Done\n");
            }
            else{
                result = Parser::parse_config_command(input_command);
                if (result <0) return result;
            }
        }
        else{
        switch (*(input_command ++)){
            case M:{
                switch (*(input_command++)){
                    case A:{
                        nprintf("Move Absolute Command\n");
                        Parser::absolute_motion = true;
                        result = Parser::parse_movement_command(input_command);
                        if (result < 0) return result;
                    } break;
                    case R:{
                        nprintf("Move Relative Command\n");
                        Parser::absolute_motion = false;
                        result = Parser::parse_movement_command(input_command);
                        if (result < 0) return result;
                    } break;
                    default:{
                        return -1;
                    }
                }
            } break;
            case C:{
                if (*(input_command++) == S){
                    Parser::config_mode = true;
                    Parser::last_valid_axis_idx;
                    nprintf("@Ready for Configuration\n",server);
                }
            } break;
            case K:{
                nprintf("MOTORS STOPPED\n");
                return 0;
            } break;
            case QUESTION:{
                switch (*(input_command ++)){
                    case P:{
                        nprintf("Query Position\n");
                    } break;
                    case C:{
                        nprintf("CURRENT CONFIG\n");
                        nprintf("E-Stop I/O Port: %d\n", config.e_stop_io_port);
                        nprintf("Valid Axes: %s\n",config.valid_axes);
                        for (int i =0; i<MAX_AXES; i++){
                            nprintf("- Motor: %d\n", i);
                            nprintf(" - Axis: %c\n", config.motors[i].axis);
                            nprintf(" - CM/Step: %f\n", config.motors[i].cm_step);
                            nprintf(" - Reverse: %s\n",(config.motors[i].reverse) ? "True":"False");
                            nprintf(" - Configured: %s\n",(config.motors[i].configured) ? "True":"False");
                        }
                    } break;
                    case A:{
                        cout << "Query Axes\n";
                        nprintf("Last Command: %c:%.2fcm %c:%.2fcm %c:%.2fcm\n",Parser::config.valid_axes[0],axis_floats[0],Parser::config.valid_axes[1],axis_floats[1],Parser::config.valid_axes[2],axis_floats[2]);
                    } break;
                    case S:{
                        cout << "Query Status\n";
                    } break;
                }
            } break;
            default:{
                return -1;
            }
        }
        }
        return 0;
    }

    bool is_axis(char value){
        return ((value >= A) && (value <= Z));
    }

    bool is_numeric_in_range(char value, int max = 10){
        return ((value >= ZERO) && (value < (max + ZERO)));
    }

    bool is_axis_end(char value){
        return ((value == LF) || (value == CR) || (value == SPACE) || (value == NULL_CHAR));
    }

    bool is_end(char value){
        return ((value == LF) || (value == CR) || (value == NULL_CHAR));
    }

    void adjust_axes(){
        char found_axes[MAX_AXES];
        for (int i; i < MAX_AXES; i++){
            found_axes[i] = Parser::config.motors[i].axis;
        }
        char approved_axes[MAX_AXES];
        int index;
        char * current_axes = &Parser::config.valid_axes[0];
        while(*current_axes){
            for (int i; i< MAX_AXES; i++){
                if (*current_axes == found_axes[i]){
                    approved_axes[index] = *current_axes;
                    index++;
                }
            }
            current_axes++;
        }
        for (int i; i<MAX_AXES; i++){
            Parser::config.valid_axes[i] = Parser::is_axis(approved_axes[i]) ? approved_axes[i] : NULL_CHAR;
        }
    }

    void reset_float_axes(void){
        for (int i = 0; i < MAX_AXES; i++){
            Parser::axis_floats[i] = 0.0;
        }
    }

    int32_t get_next_valid_axis(char axis_designator){
        while(Parser::config.valid_axes[last_valid_axis_idx]){
            if (Parser::config.valid_axes[last_valid_axis_idx] == axis_designator) return 0;
            last_valid_axis_idx++;
        }
        return AXIS_ERROR;
    }

    int32_t extract_floats(char* char_float, double * motor_ptr = 0x00){
        double float_holder = 0.0;
        int idx_counter = MAX_FLOATS_LENGTH;
        while (Parser::is_numeric_in_range(*char_float)){
            float_holder = (*char_float - ZERO) + (float_holder * 10);
            char_float++;
            if (!idx_counter--) return EXCEEDS_BUFFER;
        }
        if ((*char_float++) != DECIMAL_POINT) return FLOAT_INVALID;
        if (!idx_counter--) return EXCEEDS_BUFFER;
        float counter = 10.0;
        while (Parser::is_numeric_in_range(*char_float)){
            float_holder = ((*char_float - ZERO) / counter) + (float_holder);
            counter *= 10.0;
            char_float++;
            if (!idx_counter--) return EXCEEDS_BUFFER;
        }
        if (counter == 10.0) return FLOAT_INVALID;
        if (motor_ptr == 0x00) Parser::axis_floats[Parser::last_valid_axis_idx] = float_holder;
        else *motor_ptr = float_holder;
        return 0;
    }

    int32_t parse_movement_command(char * command){
        last_valid_axis_idx = 0;
        char * current_char = command;
        int32_t holder;
        char float_values[MAX_AXES][MAX_FLOATS_LENGTH + 1];
        while (!Parser::is_end(*current_char)){
            if ((*(current_char++)) != SPACE) return CMD_FORMAT_ERROR;
            if (!Parser::is_axis(*(current_char))) return CMD_FORMAT_ERROR;
            holder = Parser::get_next_valid_axis(*(current_char++));
            if (holder) return holder;
            char* char_float_ptr = &float_values[Parser::last_valid_axis_idx][0];
            while (!Parser::is_axis_end(*current_char)){
                *char_float_ptr = *current_char;
                current_char++;
                char_float_ptr++;
            }
            holder = Parser::extract_floats(&float_values[Parser::last_valid_axis_idx][0]);
            if (holder) return holder;
        }
        return 0;
    }

    void load_motor_into_config(int motor_index, motor * data){
        Parser::config.motors[motor_index].axis = data->axis;
        Parser::config.motors[motor_index].cm_step = data->cm_step;
        Parser::config.motors[motor_index].reverse = data->reverse;
        Parser::config.motors[motor_index].configured = true;
    }

    int32_t parse_config_command(char * command){
        int32_t result;
        switch (*(command++)){
            case M:{
                motor motor_temp;
                if (!Parser::is_numeric_in_range(*command,MAX_AXES)) return MOTOR_CONFIG_IDX_ERR;
                int motor_index = *command++ - ZERO;
                if (Parser::is_end(*command)){
                    Parser::config.motors[motor_index].axis = NULL_CHAR;
                    Parser::config.motors[motor_index].cm_step = 0.0;
                    Parser::config.motors[motor_index].reverse = false;
                    Parser::config.motors[motor_index].configured = false;
                    break;
                }
                if (!(*(command++) == BAR)) return MOTOR_CONFIG_FORMAT_ERR;
                if (!Parser::is_axis(*command)) return AXIS_ERROR;
                motor_temp.axis = *command;
                command++;
                if (!(*command++ == BAR)) return MOTOR_CONFIG_FORMAT_ERR;
                result = Parser::extract_floats(command, &motor_temp.cm_step);
                if (result < 0) return result;
                while (!(*command == BAR) && !Parser::is_end(*command)) command++;
                command++;
                if (!Parser::is_numeric_in_range(*command,2)) return PARSE_BOOL_ERR;
                motor_temp.reverse = (bool) (*command - ZERO);
                Parser::load_motor_into_config(motor_index, &motor_temp);
            } break;
            case E:{
                if (!Parser::is_numeric_in_range(*command,DIGITAL_CONNECTORS)) return PARSE_BOOL_ERR;
                config.e_stop_io_port = *command - ZERO;
            } break;
        }
        return 0;
    }

    double axis_floats[MAX_AXES];
    bool available = false;

private:
    bool absolute_motion = true;
    int last_valid_axis_idx = 0;
    bool config_mode = false;
    config_profile config;
    char command[MAX_CMD_LEN + 1];
    ClearCore::EthernetTCPServer * server;
};

int main(){
    Parser parser((ClearCore::EthernetTCPServer *) NULL);
    char command_stream[128];
    while (1){
        fgets(&command_stream[0],128, stdin);
        parser.parse_command(&command_stream[0]);
    }
}

/* int main(){
    cout << "Line 0 ";
    ClearCore::EthernetTCPServer TCP_Server;
    cout << "Line 1 ";
    Parser temp_parser(&TCP_Server);
    cout << "Line 2 ";
    Parser* parser = &temp_parser;
    cout << "Line 3 ";
    char command_stream[128];
    while(1){
        fgets(&command_stream[0],128,stdin);
        printf("CMD: %d",command_stream[0]);
        if (command_stream[0] == AT){//"@"
            int port;
            sscanf(&command_stream[1],"%d", &port);
            printf("Port: %d\n",port);
            port = (port >= 1024 && port <= 65535) ? port : PORT;
            TCP_Server.~EthernetTCPServer();
            parser->~Parser();
            ClearCore::EthernetTCPServer TCP_Server;
            Parser temp_parser(&TCP_Server);
            parser = &temp_parser;
        }
        switch (parser->parse_command(&command_stream[0])){
            case CMD_FORMAT_ERROR:
                nprintf("CMD FORMAT ERROR\n");
                break;
            case FLOAT_INVALID:
                nprintf("FLOAT INVALID\n");
                break;
            case EXCEEDS_BUFFER:
                nprintf("EXCEEDS BUFFER\n");
                break;
            case AXIS_ERROR:
                nprintf("AXIS ERROR\n");
                break;
            case MOTOR_BUSY:
                nprintf("MOTOR BUSY\n");
                break;
            case MOTOR_CONFIG_FORMAT_ERR:
                nprintf("MOTOR CONFIG FORMAT ERROR\n");
                break;
            case MOTOR_CONFIG_IDX_ERR:
                nprintf("MOTOR CONFIG INDEX ERROR\n");
                break;
            case PARSE_BOOL_ERR:
                nprintf("MOTOR CONFIG BOOLEAN ERROR\n");
                break;
            default:
                break;
        }
        nprintf("%c",4);
    }
} */