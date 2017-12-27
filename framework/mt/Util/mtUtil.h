/*
 * mtsys.h
 *
 * This module contains the API for the MT Util Interface.
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef ZBMTUTIL_H
#define ZBMTUTIL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/***************************************************************************************************
 * UTIL COMMANDS
 ***************************************************************************************************/

/* SREQ/SRSP */
#define MT_UTIL_GET_DEVICE_INFO             0x00
#define MT_UTIL_GET_NV_INFO                 0x01
#define MT_UTIL_GET_SET_PAN_ID              0x02
#define MT_UTIL_SET_CHANNELS                0x03
#define MT_UTIL_SET_SEC_LEVEL               0x04
#define MT_UTIL_SET_PRECFGKEY               0x05
#define MT_UTIL_CALLBACK_SUB_CMD            0x06
#define MT_UTIL_KEY_EVENT                   0x07
#define MT_UTIL_TIME_ALIVE                  0x09
#define MT_UTIL_LED_CONTROL                 0x0A
#define MT_UTIL_LOOPBACK                    0x10
#define MT_UTIL_DATA_REQUEST                0x11
#define MT_UTIL_ADDRMGR_EXT_ADDR_LOOKUP     0x40
#define MT_UTIL_ADDRMGR_NWK_ADDR_LOOKUP     0x41
#define MT_UTIL_APSME_LINK_KEY_DATA_GET     0x44
#define MT_UTIL_ASSOC_COUNT                 0x48
#define MT_UTIL_ASSOC_FIND_DEVICE           0x49
#define MT_UTIL_ASSOC_GET_WITH_ADDRESS      0x4A
#define MT_UTIL_ZCL_KEY_EST_INIT_EST        0x80
#define MT_UTIL_ZCL_KEY_EST_SIGN            0x81

/* AREQ to host */
#define MT_UTIL_SYNC_REQ                    0xE0
#define MT_UTIL_ZCL_KEY_ESTABLISHED_IND     0xE1

/* Callbacks masks */
#define MT_SYS_CB                           0x0100
#define MT_MAC_CB                           0x0200
#define MT_NWK_CB                           0x0300
#define MT_AF_CB                            0x0400
#define MT_ZDO_CB                           0x0500
#define MT_SAPI_CB                          0x0600
#define MT_UTIL_CB                          0x0700
#define MT_DEBUG_CB                         0x0800
#define MT_APP_CB                           0x0900
#define MT_ALL_CB                           0xFFFF

#define MT_CB_ACTION_ENABLE                 0x1
#define MT_CB_ACTION_DISABLE                0x0

typedef struct
{
	uint16_t SubsystemId;
	uint8_t Action;
} CallbackSubCmdFormat_t;

typedef struct
{
	uint8_t Status;
} CallbackSubCmdSrspFormat_t;

typedef uint8_t (*mtUtilCallbackSubCmdSrspCb_t)(CallbackSubCmdSrspFormat_t *msg);

typedef uint8_t (*mtUtilStub_t)(void);

typedef struct
{
	mtUtilCallbackSubCmdSrspCb_t pfnUtilCallbackSubCmdSrsp;
} mtUtilCb_t;

/*MACROS*/
#define SUCCESS 0x00
#define FAILURE 0x01
#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)
#define BREAK_UINT32(var, ByteNum) \
                (uint8_t)((uint32_t)(((var)>>((ByteNum) * 8)) & 0x00FF))

void utilRegisterCallbacks(mtUtilCb_t cbs);
void utilProcess(uint8_t *rpcBuff, uint8_t rpcLen);
uint8_t utilCallbackSubCmd(CallbackSubCmdFormat_t *req);

#ifdef __cplusplus
}
#endif

#endif /* ZBMTUTIL_H */
