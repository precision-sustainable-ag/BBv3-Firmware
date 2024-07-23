/*
 * parser.cpp
 *
 * Created: 7/23/2024 1:50:03 PM
 *  Author: daniel
 */ 

#define TESTING

#include "config.h"
#include "commands.h"
#include "chars.h"
#include "states.h"
#include "types.h"
#include "errors.h"
#include "ClearCore.h"
#include "parser.h"

namespace BenchBot{

int printf(const char * format, ...){
    va_list arg_list;
    char holder[MAX_BUFFER];
    int count = vsnprintf(&holder[0], MAX_BUFFER, format, arg_list);
    ConnectorUsb.Send(&holder[0]);
    return count;
}

/*
class Parser {
public:
    Parser(* ClearCore::EthernetTCPServer * server *){
        Parser::available = true;
        Parser::config = config;
    }
    ~Parser(){
        return;
    }

    int32_t key_gen(unsigned char * input_command){
        uint32_t result = 0x0;
        for (int i = 0; i <= 3; i++){
            if (input_command[i] == SPACE_CHAR || input_command[i] == LF_CHAR || input_command[i] == CR_CHAR) break;
            else result = (result << 8) + input_command[i];
        }
        printf("%X", result);
        return result;
    }

    int32_t parse_command(char * input_command){
        uint32_t result = 0;
        uint32_t next_state;
        uint32_t digested_command = key_gen((unsigned char *)input_command);
        switch (state){
            case S_NORMAL:{
                switch(digested_command){
                    case MOVEABS_KEY:{
                        printf("Movement Absolute Command\n");
                        next_state = S_BUSY;
                    } break;
                    case MOVEREL_KEY:{
                        printf("Movement Relative Command\n");
                        next_state = S_BUSY;
                    } break;
                    default: {
                        result = CMD_UNKNOWN_ERROR;
                    } break;
                }
            } break;
            case S_BUSY:{
                switch (digested_command){
                    case ESTOP_KEY:{
                        printf("Stopping Motors\n");
                        next_state = S_NORMAL;
                    } break;
                    default:{
                        result = MOTOR_BUSY;
                    } break;
                }
            } break;
        }
        state = next_state;
        return result;
    }

    bool is_axis(char value){
        return ((value >= A_CHAR) && (value <= Z_CHAR));
    }

    bool is_numeric_in_range(char value, int max = 10){
        return ((value >= ZERO_CHAR) && (value < (max + ZERO_CHAR)));
    }

    bool is_axis_end(char value){
        return ((value == LF_CHAR) || (value == CR_CHAR) || (value == SPACE_CHAR) || (value == NULL_CHAR));
    }

    bool is_end(char value){
        return ((value == LF_CHAR) || (value == CR_CHAR) || (value == NULL_CHAR));
    }

    void reset_float_axes(void){
        for (int i = 0; i < MAX_AXES; i++){
            Parser::axis_floats[i] = 0.0;
        }
    }

    int32_t get_next_valid_axis(char axis_designator){
        for (int i = last_valid_axis_idx; i < MAX_AXES; i++){
            if (Parser::config.axes[last_valid_axis_idx]->designator == axis_designator) return 0;
            last_valid_axis_idx++;
        }
        return AXIS_ERROR;
    }

    int32_t extract_floats(char* char_float, double * motor_ptr = 0x00){
        double float_holder = 0.0;
        int idx_counter = MAX_FLOATS_LENGTH;
        while (Parser::is_numeric_in_range(*char_float)){
            float_holder = (*char_float - ZERO_CHAR) + (float_holder * 10);
            char_float++;
            if (!idx_counter--) return EXCEEDS_BUFFER;
        }
        if ((*char_float++) != DECIMAL_POINT) return FLOAT_INVALID;
        if (!idx_counter--) return EXCEEDS_BUFFER;
        float counter = 10.0;
        while (Parser::is_numeric_in_range(*char_float)){
            float_holder = ((*char_float - ZERO_CHAR) / counter) + (float_holder);
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
            if ((*(current_char++)) != SPACE_CHAR) return CMD_FORMAT_ERROR;
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
        
    }

    int32_t parse_config_command(char * command){
        int32_t result;
        
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
    uint32_t state = S_NORMAL;
};
*/
}
