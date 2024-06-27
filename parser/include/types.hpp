typedef struct motor {
    char axis;
    double cm_step;
    bool reverse = false;
    bool configured = false;
} motor;

typedef struct config {
    int e_stop_io_port;
#ifdef AXES
    char valid_axes[5] = AXES;
#else
    char valid_axes[5];
#endif
    motor motors[4];
} config_profile;