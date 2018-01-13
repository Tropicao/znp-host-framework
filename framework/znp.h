#ifndef ZNP_H
#define ZNP_H

#include "mtAf.h"
#include "mtSys.h"
#include "mtZdo.h"
#include "mtUtil.h"
#include "mtSapi.h"
#include "dbgPrint.h"

typedef void (*ZnpCallback_t)(void);

typedef enum
{
    ZSuccess =                  0x00,
    Zfailure =                  0x01,
    ZinvalidParameter =         0x02,
    NV_ITEM_UNINIT =            0x09,
    NV_OPER_FAILED =            0x0a,
    NV_BAD_ITEM_LEN =           0x0c,
    ZmemError =                 0x10,
    ZbufferFull =               0x11,
    ZunsupportedMode =          0x12,
    ZmacMemError =              0x13,
    zdoInvalidRequestType =     0x80,
    zdoInvalidEndpoint =        0x82,
    zdoUnsupported =            0x84,
    zdoTimeout =                0x85,
    zdoNoMatch =                0x86,
    zdoTableFull =              0x87,
    zdoNoBindentry =            0x88,
    ZsecNoKey =                 0xa1,
    ZsecMaxFrmCount =           0xa3,
    ZapsFail =                  0xb1,
    ZapsTableFull =             0xb2,
    ZapsIllegalRequest =        0xb3,
    ZapsInvalidBinding =        0xb4,
    ZapsUnsupportedAttrib =     0xb5,
    ZapsNotSupported =          0xb6,
    ZapsNoAck =                 0xb7,
    zApsDuplicateEntry =        0xb8,
    ZapsNoBoundDevice =         0xb9,
    ZnwkInvalidParam =          0xc1,
    ZnwkInvalidRequest =        0xc2,
    ZnwkNotPermitted =          0xc3,
    ZnwkStartupFailure =        0xc4,
    ZnwkTableFull =             0xc7,
    ZnwkUnknownDevice =         0xc8,
    ZnwkUnsupportedAttribute =  0xc9,
    ZnwkNoNetworks =            0xca,
    ZnwkLeaveUnconfirmed =      0xcb,
    ZnwkNoAck =                 0xcc,
    ZnwkNoRoute =               0xcd,
    ZMacNoAck =                 0xe9,
} ZNPStatus;


int znp_init();
void znp_shutdown();
int znp_socket_get();
void znp_loop_read();
int znp_message_cb_set(ZnpCallback_t cb);
const char *znp_strerror(ZNPStatus status);

#endif

