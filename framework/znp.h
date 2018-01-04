#ifndef ZNP_H
#define ZNP_H

#include "mtAf.h"
#include "mtSys.h"
#include "mtZdo.h"
#include "mtUtil.h"
#include "mtSapi.h"
#include "dbgPrint.h"

typedef void (*ZnpCallback_t)(void);


int znp_init();
void znp_shutdown();
int znp_socket_get();
void znp_loop_read();
int znp_message_cb_set(ZnpCallback_t cb);

#endif

