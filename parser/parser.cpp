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

int nprintf(ClearCore::EthernetTCPServer * server, const char * format, ...) {
    va_list arg_list;
    va_start(arg_list, format);
    char output_buffer[MAX_BUFFER];
    int chars = vsnprintf(output_buffer, MAX_BUFFER, format, arg_list);
    server->write(output_buffer);
    cout << output_buffer;
    return chars;
}

class Parser {
public:
    Parser(){
        Parser::available = true;
        Parser::config = config;
    }

    int32_t parse_command(char * input_command){
        uint32_t result;
        if (config_mode){
            if ((input_command[0] == C) && (input_command[1] == S)){
                config_mode = false;
                cout << "Configuration Done\n";
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
                        cout << "Move Absolute Command\n";
                        Parser::absolute_motion = true;
                        result = Parser::parse_movement_command(input_command);
                        if (result < 0) return result;
                    } break;
                    case R:{
                        cout << "Move Relative Command\n";
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
                    cout << "Ready for Configuration\n";
                }
            } break;
            case K:{
                cout << "MOTORS STOPPED\n";
                return 0;
            } break;
            case QUESTION:{
                switch (*(input_command ++)){
                    case P:{
                        cout << "Query Position\n";
                    } break;
                    case C:{
                        cout << "CURRENT CONFIG\n";
                        printf("E-Stop I/O Port: %d\n", config.e_stop_io_port);
                        printf("Valid Axes: %s\n",config.valid_axes);
                        for (int i =0; i<MAX_AXES; i++){
                            printf("- Motor: %d\n", i);
                            printf(" - Axis: %c\n", config.motors[i].axis);
                            printf(" - CM/Step: %f\n", config.motors[i].cm_step);
                            printf(" - Reverse: %s\n",(config.motors[i].reverse) ? "True":"False");
                            printf(" - Configured: %s\n",(config.motors[i].configured) ? "True":"False");
                        }
                    } break;
                    case A:{
                        cout << "Query Axes\n";
                        printf("Last Command: %c:%.2fcm %c:%.2fcm %c:%.2fcm\n",Parser::config.valid_axes[0],axis_floats[0],Parser::config.valid_axes[1],axis_floats[1],Parser::config.valid_axes[2],axis_floats[2]);
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
    ClearCore::EthernetTCPServer server;
};

int main(){
    Parser parser;
    char command_stream[128];
    while(1){
        fgets(&command_stream[0],128,stdin);
        switch (parser.parse_command(&command_stream[0])){
            case CMD_FORMAT_ERROR:
                printf("CMD FORMAT ERROR\n");
                break;
            case FLOAT_INVALID:
                printf("FLOAT INVALID\n");
                break;
            case EXCEEDS_BUFFER:
                printf("EXCEEDS BUFFER\n");
                break;
            case AXIS_ERROR:
                printf("AXIS ERROR\n");
                break;
            case MOTOR_BUSY:
                printf("MOTOR BUSY\n");
                break;
            case MOTOR_CONFIG_FORMAT_ERR:
                printf("MOTOR CONFIG FORMAT ERROR\n");
                break;
            case MOTOR_CONFIG_IDX_ERR:
                printf("MOTOR CONFIG INDEX ERROR\n");
                break;
            case PARSE_BOOL_ERR:
                printf("MOTOR CONFIG BOOLEAN ERROR\n");
                break;
            default:
                break;
        }
        printf("%c",4);
    }
}