#include <stdlib.h>
#include "znp.h"
#include "dbgPrint.h"
#include "rpc.h"

#define DEFAULT_DEVICE      "/dev/ttyACM0"

//          TODO :
//          * add typedef on cb
//          * standard message structure to send

typedef struct
{
    int socket_fd;
    ZnpCallback_t message_cb;
} Znp_Private_Data;

static Znp_Private_Data *priv = NULL;

int znp_init()
{
    if(priv)
    {
        LOG_WARN("ZNP library already initialized");
        return 0;
    }
    priv = calloc(1, sizeof(Znp_Private_Data));
    if(!priv)
    {
        LOG_CRI("Cannot allocate memory for ZNP private data");
        exit(1);
    }
    priv->socket_fd = rpcOpen(DEFAULT_DEVICE);
    if(priv->socket_fd < 0)
    {
        LOG_ERR("Cannot open connection to Zigbee end dongle");
        free(priv);
        return 1;
    }
    return 0;
}

void znp_shutdown()
{
    if(!priv)
    {
        LOG_WARN("ZNP library already deinitialized");
        return;
    }
    rpcClose();
    free(priv);
    priv = NULL;
}

int znp_socket_get()
{
    if(priv)
        return priv->socket_fd;
    return -1;
}

void znp_loop_read()
{
    if(rpcGetMqClientMsg() < 0)
        LOG_WARN("Failed to retrieve any message from log queue");
}

int znp_message_cb_set(ZnpCallback_t cb __attribute__((unused)))
{
    // TODO : add first param as message type, second message as callback
    return 0;
}


