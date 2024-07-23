/*
 * parser.h
 *
 * Created: 7/23/2024 1:49:46 PM
 *  Author: daniel
 */ 


#ifndef PARSER_H_
#define PARSER_H_
#include "types.h"
#include "config.h"
#include "types.h"
#include "commands.h"
#include "chars.h"
#include "states.h"
#include <stdarg.h>
#include <stdio.h>

//#include <ClearCore.h>

namespace BenchBot{

int printf(const char * format, ...);

class Parser {
public:
    Parser(/* ClearCore::EthernetTCPServer * server */);
    ~Parser();
    int32_t key_gen(unsigned char * input_command);
    int32_t parse_command(char * input_command);
    bool is_axis(char value);
    bool is_numeric_in_range(char value, int max = 10);
    bool is_axis_end(char value);
    bool is_end(char value);
    void reset_float_axes(void);
    int32_t get_next_valid_axis(char axis_designator);
    int32_t extract_floats(char* char_float, double * motor_ptr = 0x00);
    int32_t parse_movement_command(char * command);
    void load_motor_into_config(int motor_index, motor * data);
    int32_t parse_config_command(char * command);
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
}
#endif
