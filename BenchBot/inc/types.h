#ifndef TYPES_H_
#define TYPES_H_
#include "chars.h"

#include <cstdint>

typedef struct motor {
    bool configured = false;
    void * axis = nullptr;
    uint32_t step_per_rev;
    int limit_switch = -1;
    bool reverse = false;
} motor;

typedef struct axis {
    char designator = NULL_CHAR;
    double home = 0.0;
    double limits[2] = {0.0,0.0};
    void * motors[4] = {nullptr, nullptr, nullptr, nullptr};
} axis;

typedef struct config {
    int e_stop_io_port = -1;
    axis * axes[4];
    motor * motors[4];
} config_profile;

#endif
