#include <cstdint>

typedef struct motor {
    bool configured = false;
    void * axis = nullptr;
    uint step_per_rev;
    int limit_switch = -1;
    bool reverse = false;
} motor;

typedef struct axis {
    char designator = NULL_CHAR;
    double home = 0.0;
    double limits[2] = {0.0,0.0};
    void * motors[4] = {nullptr, nullptr, nullptr, nullptr};
} axis;

typedef struct axis {
    double cm_rev;
} axis;

typedef struct config {
    int e_stop_io_port = -1;
    axis * axes[4];
    motor * motors[4];
} config_profile;

namespace ClearCore {
class EthernetTCPServer {
public:
    explicit EthernetTCPServer(uint16_t port = PORT);
    ~EthernetTCPServer();
    uint32_t Send(char * buffer);
    uint16_t Read(char * buffer, uint32_t length);
private:
    //std::thread * TCP_thread;
    int serverSd, bindStatus, connfd;
};
}