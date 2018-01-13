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
    ZNPStatus status;
    const char *string;
}ZNPStatusString;

static ZNPStatusString _status_list[] =
{
    {ZSuccess , "ZNP command success"},
    {Zfailure , "ZNP failure"},
    {ZinvalidParameter , "ZNP invalid parameter"},
    {NV_ITEM_UNINIT, "ZNP NV item is not intialized"},
    {NV_OPER_FAILED , "ZNP NV operation is not valid"},
    {NV_BAD_ITEM_LEN , "ZNP NV item length is invalid"},
    {ZmemError , "ZNP memory error"},
    {ZbufferFull , "ZNP buffer is full"},
    {ZunsupportedMode , "ZNP unsupported mode"},
    {ZmacMemError , "ZNP MAC memory error"},
    {zdoInvalidRequestType , "ZNP ZDO request is invalid"},
    {zdoInvalidEndpoint , "ZNP ZDO request on invalid endpoint"},
    {zdoUnsupported , "ZNP ZDO command is not supported"},
    {zdoTimeout , "ZNP ZDO timeout"},
    {zdoNoMatch , "ZNP ZDO found no match"},
    {zdoTableFull , "ZNP ZDO table is full"},
    {zdoNoBindentry , "ZNP ZDO found no bind entry"},
    {ZsecNoKey , "ZNP found no security key"},
    {ZsecMaxFrmCount , "ZNP security max frame count reached"},
    {ZapsFail , "ZNP APS failed"},
    {ZapsTableFull , "ZNP APS table is full"},
    {ZapsIllegalRequest , "ZNP APS illegal request"},
    {ZapsInvalidBinding , "ZNP APS binding is invalid"},
    {ZapsUnsupportedAttrib , "ZNP APS unsupported attribute"},
    {ZapsNotSupported , "ZNP APS operation not supported"},
    {ZapsNoAck , "ZNP APS no acknowledgement received"},
    {zApsDuplicateEntry , "ZNP APS duplicate entry"},
    {ZapsNoBoundDevice , "ZNP APS no bound device found"},
    {ZnwkInvalidParam , "ZNP APS invalid parameter"},
    {ZnwkInvalidRequest , "ZNP APS invalid request"},
    {ZnwkNotPermitted , "ZNP APS operation not permitted"},
    {ZnwkStartupFailure , "ZNP NWK startup failure"},
    {ZnwkTableFull , "ZNP NWK Table is full"},
    {ZnwkUnknownDevice , "ZNP NWK unkown device"},
    {ZnwkUnsupportedAttribute , "ZNP NWM unsupported attribute"},
    {ZnwkNoNetworks , "ZNP NWK no network available"},
    {ZnwkLeaveUnconfirmed , "ZNP NWK Leave command unconfirmed"},
    {ZnwkNoAck , "ZNP NWK No Acknowledgement received"},
    {ZnwkNoRoute , "ZNP NWK no route found"},
    {ZMacNoAck , "ZNP MAC no acknowledgemeent received"}
};

static uint8_t _status_list_len = sizeof(_status_list)/sizeof(ZNPStatusString);

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
        return 1;
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
    if(rpcProcess() < 0)
    {
        LOG_WARN("Failed to retrieve message from serial");
        return;
    }
    rpcGetMqClientMsg();
}

int znp_message_cb_set(ZnpCallback_t cb __attribute__((unused)))
{
    // TODO : add first param as message type, second message as callback
    return 0;
}

const char *znp_strerror(ZNPStatus status)
{
    int index = 0;
    for(index = 0; index < _status_list_len; index++)
    {
        if(_status_list[index].status == status)
            return _status_list[index].string;
    }
    return NULL;
}



