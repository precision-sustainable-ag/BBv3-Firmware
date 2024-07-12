#ifdef TESTING
#define BLOCKING true
//class ClearCore

#endif

//User set variables specific to system
#define AXES {X,Y,Z,NULL_CHAR, NULL_CHAR}
#define MAX_AXES 4

#ifndef MAX_AXES
#define MAX_AXES 4
#endif

#define MAX_CMD_LEN 128
#define MAX_FLOATS_LENGTH 16  

#define DIGITAL_CONNECTORS 9

#define MAX_BUFFER 1024
#define PORT 8049