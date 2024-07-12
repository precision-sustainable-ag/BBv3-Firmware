#include <cstdint>

typedef struct motor {
    char axis = NULL_CHAR;
    double cm_step;
    bool reverse = false;
    bool configured = false;
} motor;

typedef struct axis {
    double cm_rev;
} axis;

typedef struct config {
    int e_stop_io_port = -1;
#ifdef AXES
    char valid_axes[5] = AXES;
#else
    char valid_axes[5];
#endif
    motor motors[4];
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