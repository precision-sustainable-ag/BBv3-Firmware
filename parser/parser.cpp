#define TESTING

#include <stdio.h>
#include <iostream>
#include <config.hpp>
#include <commands.hpp>
#include <types.hpp>
#include <errors.hpp>
//#include <ClearCore.h>

using namespace std;

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
                parse_config_command(input_command);
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
        return ((value >= ZERO) && (value < max));
    }

    bool is_axis_end(char value){
        return ((value == LF) || (value == CR) || (value == SPACE) || (value == NULL_CHAR));
    }

    bool is_end(char value){
        return ((value == LF) || (value == CR) || (value == NULL_CHAR));
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

    int32_t extract_floats(char* char_float, int motor_idx = -1){
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
        if (motor_idx >= 0) Parser::axis_floats[Parser::last_valid_axis_idx] = float_holder;
        else Parser::config.motors[motor_idx].cm_step = float_holder;
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

    int32_t parse_config_command(char * command){
        int32_t result;
        switch (*(command++)){
            case M:{
                if (!Parser::is_numeric_in_range(*command,MAX_AXES)) return MOTOR_CONFIG_IDX_ERR;
                int motor_index = *command - ZERO;
                if (!(*command++ == BAR)) return MOTOR_CONFIG_FORMAT_ERR;
                if (!Parser::is_axis(*command)) return AXIS_ERROR;
                config.motors[motor_index].axis = *command;
                command++;
                if (!(*command++ == BAR)) return MOTOR_CONFIG_FORMAT_ERR;
                result = Parser::extract_floats(command, motor_index);
                while (!(*command == BAR) && !Parser::is_end(*command)) command++;
                command++;
                if (!Parser::is_numeric_in_range(*command,2)) return MOTOR_CONFIG_BOOL_ERR;
                config.motors[motor_index].reverse = (bool) (*command - ZERO);
            }
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
            default:
                break;
        }
    }
}