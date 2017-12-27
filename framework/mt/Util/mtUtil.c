/*
 * mtSys.c
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

/*********************************************************************
 * INCLUDES
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mtUtil.h"
#include "mtParser.h"
#include "rpc.h"
#include "dbgPrint.h"

/*********************************************************************
 * LOCAL VARIABLE
 */
static mtUtilCb_t mtUtilCbs;
extern uint8_t srspRpcBuff[RPC_MAX_LEN];
extern uint8_t srspRpcLen;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void processSrsp(uint8_t *rpcBuff, uint8_t rpcLen);

/*********************************************************************
 * @fn      utilCallbackSubCmd
 *
 * @brief   This command requests to subscribe to specified lmayer callbacks
 *
 * @param   req - Pointer to command specific structure.
 *
 * @return   status, either Success (0) or Failure (1).
 */
uint8_t utilCallbackSubCmd(CallbackSubCmdFormat_t *req)
{
	uint8_t status;
	uint8_t cmInd = 0;
	uint32_t cmdLen = 3;
	uint8_t *cmd = malloc(cmdLen);

	if (cmd)
	{

		cmd[cmInd++] = (uint8_t)(req->SubsystemId & 0xFF);
		cmd[cmInd++] = (uint8_t)((req->SubsystemId >> 8) & 0xFF);
		cmd[cmInd++] = req->Action;

		status = rpcSendFrame((MT_RPC_CMD_SREQ | MT_RPC_SYS_UTIL),
		MT_UTIL_CALLBACK_SUB_CMD, cmd, cmdLen);
		free(cmd);
		return status;
	}
	else
	{
		LOG_ERR("Memory for cmd was not allocated");
		return 1;
	}
}

/*********************************************************************
 * @fn      processCallbackSubCmdSrsp
 *
 * @brief   This Function is trigered after a call to utilCallbackSubCmd.
 *           Parses the incoming buffer to a command specific structure
 *           and passes it to its respective callback function.
 *
 * @param   rpcBuff - Incoming buffer.
 * @param   rpcLen - Length of buffer.
 *
 */
static void processCallbackSubCmdSrsp(uint8_t *rpcBuff, uint8_t rpcLen)
{
	if (mtUtilCbs.pfnUtilCallbackSubCmdSrsp)
	{
		uint8_t msgIdx = 2;
		CallbackSubCmdSrspFormat_t rsp;
		if (rpcLen < 2)
		{
			LOG_WARN("MT_RPC_ERR_LENGTH");

		}

		rsp.Status = rpcBuff[msgIdx++];
		mtUtilCbs.pfnUtilCallbackSubCmdSrsp(&rsp);
	}
}

/*********************************************************************
 * @fn      utilRegisterCallbacks
 *
 * @brief
 *
 * @param
 *
 */
void utilRegisterCallbacks(mtUtilCb_t cbs)
{
	memcpy(&mtUtilCbs, &cbs, sizeof(mtUtilCb_t));
}

/*********************************************************************
 * @fn      processSrsp
 *
 * @brief  Generic function for processing the SRSP and copying it to
 *         local buffer for SREQ function to deal with
 *
 * @param
 *

 */
static void processSrsp(uint8_t *rpcBuff, uint8_t rpcLen)
{
	//copies sresp to local buffer
	memcpy(srspRpcBuff, rpcBuff, rpcLen);
	//srspRpcLen = rpcLen;
    switch (rpcBuff[1])
    {
        case MT_UTIL_CALLBACK_SUB_CMD:
            LOG_DBG("utilProcess: MT_UTIL_CALLBACK_SUB_CMD");
            processCallbackSubCmdSrsp(rpcBuff, rpcLen);
            break;
        default:
            LOG_WARN("processSrsp: unsupported UTIL message : %d (0x%02X)", rpcBuff[1], rpcBuff[1]);
            break;
    }

}

/*************************************************************************************************
 * @fn      utilProcess()
 *
 * @brief   read and process the RPC Util message from the ZB SoC
 *
 * @param   rpcLen has the size of the frame: cmd0 + cmd1 + payload + FCS
 *

 *************************************************************************************************/
void utilProcess(uint8_t *rpcBuff, uint8_t rpcLen)
{
	LOG_DBG("Processing CMD0:%x, CMD1:%x", rpcBuff[0], rpcBuff[1]);

	//process the synchronous SRSP from SREQ
    if ((rpcBuff[0] & MT_RPC_CMD_TYPE_MASK) == MT_RPC_CMD_SRSP)
    {
        processSrsp(rpcBuff, rpcLen);
    }
    else
    {
        //Read CMD1 and processes the specific SREQ
        switch (rpcBuff[1])
        {
            default:
                LOG_ERR("CMD0:%x, CMD1:%x, not handled", rpcBuff[0], rpcBuff[1]);
                break;
        }
    }
}

