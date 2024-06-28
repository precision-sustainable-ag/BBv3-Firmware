#define CMD_FORMAT_ERROR -1 // Error parsing command. 
#define FLOAT_INVALID -2 // An incomplete float was recieved, such as 1.
#define EXCEEDS_BUFFER -3 // More characters recieved than expected.
#define AXIS_ERROR -4 // Invalid axis designation
#define MOTOR_BUSY -10 // Motors are not available for new command.
#define MOTOR_CONFIG_FORMAT_ERR -20
#define MOTOR_CONFIG_IDX_ERR -21 //Index of Motor invalid
#define PARSE_BOOL_ERR -22