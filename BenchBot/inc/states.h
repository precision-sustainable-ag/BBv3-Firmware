#ifndef STATES_H_
#define STATES_H_

#define S_NORMAL 0x0100
#define S_BUSY 0x0200
#define S_CONFIG 0x0800
#define S_MOT_CON 0x0900
#define S_AX_CON 0x0a00

#define S_DES_MASK 0x00ff
#define S_STATE_MASK 0xff00
#define S_CONF_STATE_MASK 0x0700

#endif
