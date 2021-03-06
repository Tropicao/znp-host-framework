/*
 * rpc.c
 *
 * This module contains the RPC (Remote Procedure Call) API for the
 * ZigBee Network Processor (ZNP) Host Interface.
 *
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>      // For O_* constants
#include <errno.h>
#include <signal.h>
#include <semaphore.h>
#include <time.h>
#include "queue.h"
#include <sys/time.h>

#include "rpc.h"
#include "rpcTransport.h"
#include "mtParser.h"
#include "dbgPrint.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

#define SB_FORCE_BOOT              (0xF8)
#define SB_FORCE_RUN               (SB_FORCE_BOOT ^ 0xFF)

#define SRSP_TIMEOUT_MS            (2000) // 2000ms timeout
/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

// expected SRSP command ID
static uint8_t expectedSrspCmdId;

// RPC message queue for passing RPC frame from RPC process to APP process
static llq_t rpcLlq;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTIONS DECLARATION
 */

// function for calculating FCS in RPC UART frame
static uint8_t calcFcs(uint8_t *msg, uint8_t len);

// function for printing out RPC frames
static void printRpcMsg(char* preMsg, uint8_t sof, uint8_t len, uint8_t *msg);

/*********************************************************************
 * API FUNCTIONS
 */

/*********************************************************************
 * @fn      rpcOpen
 *
 * @brief   opens the serial port to the CC253x.
 *
 * @param   devicePath - path to the UART device
 *
 * @return  status
 */
int32_t rpcOpen(char *_devicePath)
{
	int fd;

	// open RPC transport
	fd = rpcTransportOpen(_devicePath);
	if (fd < 0)
	{
		perror(_devicePath);
		LOG_CRI("%s device open failed", _devicePath);
		return (-1);
	}

	return fd;
}

/*********************************************************************
 * @fn      rpcOpen
 *
 * @brief   Close serial device file descriptor
 *
 * @return  status
 */
void rpcClose(void)
{
    rpcTransportClose();
}


/*********************************************************************
 * @fn      rpcInitMq
 *
 * @brief   init message queue
 *
 * @param   -
 *
 * @return  status
 */
int32_t rpcInitMq(void)
{

	llq_open(&rpcLlq);
	return 0;
}

/*********************************************************************
 * @fn      rpcGetMqClientMsg
 *
 * @brief   wait (blocking function) for incoming message and process
 *          it
 *
 * @param   -
 *
 * @return  status
 */
int32_t rpcGetMqClientMsg(void)
{
	uint8_t rpcFrame[RPC_MAX_LEN + 1];
	int32_t rpcLen;

	LOG_DBG("Retrieving new message from queue");

	// wait for incoming message queue
	rpcLen = llq_receive(&rpcLlq, (char *) rpcFrame, RPC_MAX_LEN + 1);

	if (rpcLen != -1)
	{
		LOG_DBG("processing MT[%d]", rpcLen);

		// process incoming message
		mtProcess(rpcFrame, rpcLen);
	}
	else
	{
		LOG_ERR("No message in queue");
		return -1;
	}

	return 0;
}

/*********************************************************************
 * @fn      rpcForceRun
 *
 * @brief   send force run bootloader command
 *
 * @param   -
 *
 * @return  -
 */
void rpcForceRun(void)
{
	uint8_t forceBoot = SB_FORCE_RUN;

	// send the bootloader force boot incase we have a bootloader that waits
	rpcTransportWrite(&forceBoot, 1);
}

/*************************************************************************************************
 * @fn      rpcProcess()
 *
 * @brief   Read bytes from transport layer and form an RPC frame
 *
 * @param   none
 *
 * @return  length of current Rx Buffer
 *************************************************************************************************/
int32_t rpcProcess(void)
{
	uint8_t rpcLen, rpcTempLen, bytesRead, sofByte, rpcBuffIdx;
	uint8_t retryAttempts = 0, len, rpcBuff[RPC_MAX_LEN];
	uint8_t fcs;

#ifndef HAL_UART_IP //No SOF for IP	//read first byte and check it is a SOF
	bytesRead = rpcTransportRead(&sofByte, 1);

	if ((sofByte == MT_RPC_SOF) && (bytesRead == 1))
#endif
	{
		// clear retry counter
		retryAttempts = 0;

		// read length byte
		bytesRead = rpcTransportRead(&rpcLen, 1);

		if (bytesRead == 1)
		{
			len = rpcLen;
			rpcBuff[0] = rpcLen;

#ifdef HAL_UART_IP //No FCS for IP			//allocating RPC payload (+ cmd0, cmd1)
			rpcLen += RPC_CMD0_FIELD_LEN + RPC_CMD1_FIELD_LEN;
#else
			//allocating RPC payload (+ cmd0, cmd1 and fcs)
			rpcLen +=
			RPC_CMD0_FIELD_LEN + RPC_CMD1_FIELD_LEN + RPC_UART_FCS_LEN;
#endif

			//non blocking read, so we need to wait for the rpc to be read
			rpcBuffIdx = 1;
			rpcTempLen = rpcLen;
			while (rpcTempLen > 0)
			{
				// read RPC frame
				bytesRead = rpcTransportRead(&(rpcBuff[rpcBuffIdx]),
				        rpcTempLen);

				// check for error
				if (bytesRead > rpcTempLen)
				{
					//there was an error
					LOG_ERR("read of %d bytes failed - %s", rpcTempLen, strerror(errno));

					// check whether retry limits has been reached
					if (retryAttempts++ < 5)
					{
						// sleep for 10ms
						usleep(10000);

						// try again
						bytesRead = 0;
					}
					else
					{
						// something went wrong, abort
						LOG_CRI("transport read failed too many times");

						return -1;
					}
				}

				// update counters
				if (rpcTempLen > bytesRead)
				{
					rpcTempLen -= bytesRead;
				}
				else
				{
					rpcTempLen = 0;
				}
				rpcBuffIdx += bytesRead;
			}

			// print out incoming RPC frame
			printRpcMsg("SOC IN  <--", MT_RPC_SOF, len, &rpcBuff[1]);

			//Verify FCS of incoming MT frames
			fcs = calcFcs(&rpcBuff[0], (len + 3));
			if (rpcBuff[len + 3] != fcs)
			{
				LOG_ERR("fcs error %x:%x", rpcBuff[len + 3], fcs);
				return -1;
			}

			if ((rpcBuff[1] & MT_RPC_CMD_TYPE_MASK) == MT_RPC_CMD_SRSP)
			{
				// SRSP command ID deteced
				if (expectedSrspCmdId == (rpcBuff[1] & MT_RPC_SUBSYSTEM_MASK))
				{
					LOG_DBG( "Processing expected srsp [%02X]", rpcBuff[1] & MT_RPC_SUBSYSTEM_MASK);
					LOG_DBG( "Writing %d bytes SRSP to head of the queue", rpcLen);

					// send message to queue
					llq_add(&rpcLlq, (char*) &rpcBuff[1], rpcLen, 1);
                    expectedSrspCmdId = 0xFF;
				}
				else
				{
					// unexpected SRSP discard
					LOG_ERR( "UNEXPECTED SREQ!: %02X:%02X", expectedSrspCmdId, (rpcBuff[1] & MT_RPC_SUBSYSTEM_MASK));
                    expectedSrspCmdId = 0xFF;
					return 0;
				}
			}
			else
			{
				// should be AREQ frame
				LOG_DBG("writing %d bytes AREQ to tail of the queue", rpcLen);

				// send message to queue
				llq_add(&rpcLlq, (char*) &rpcBuff[1], rpcLen, 0);
			}

			return 0;
		}
		else
		{
			LOG_ERR("Len Not read [%x]", bytesRead);
		}
	}
	else
	{
		LOG_ERR("No valid Start Of Frame found [%x:%x]", sofByte, bytesRead);
	}

	return -1;
}

/*************************************************************************************************
 * @fn      sendRpcFrame()
 *
 * @brief   builds the Frame and sends it to the transport layer - usually called by the
 *          application thread(s)
 *
 * @param   cmd0 System, cmd1 subsystem, ptr to payload, lenght of payload
 *
 * @return  length of current Rx Buffer
 *************************************************************************************************/
uint8_t rpcSendFrame(uint8_t cmd0, uint8_t cmd1, uint8_t *payload,
        uint8_t payload_len)
{
	uint8_t buf[RPC_MAX_LEN];
	int32_t status = MT_RPC_SUCCESS;

	LOG_DBG("Sending RPC");

	// fill in header bytes
	buf[0] = MT_RPC_SOF;
	buf[1] = payload_len;
	buf[2] = cmd0;
	buf[3] = cmd1;

	if ((cmd0 & MT_RPC_CMD_TYPE_MASK) == MT_RPC_CMD_SREQ)
	{
		// calculate expected SRSP
		expectedSrspCmdId = (cmd0 & MT_RPC_SUBSYSTEM_MASK);
        LOG_DBG("Expecting SRSP %02X", expectedSrspCmdId);
	}

	if (payload_len > 0)
	{
		// copy payload to buffer
		memcpy(buf + RPC_UART_HDR_LEN, payload, payload_len);
	}

	// calculate FCS field
	buf[payload_len + RPC_UART_HDR_LEN] = calcFcs(
	        &buf[RPC_UART_FRAME_START_IDX], payload_len + RPC_HDR_LEN);

#ifdef HAL_UART_IP
	// No SOF or FCS
	rpcTransportWrite(buf+1, payload_len + RPC_HDR_LEN + RPC_UART_FCS_LEN);
#else
	// send out RPC  message
	rpcTransportWrite(buf, payload_len + RPC_UART_HDR_LEN + RPC_UART_FCS_LEN);
#endif

	// print out message to be sent
	printRpcMsg("SOC OUT -->", buf[0], payload_len, &buf[2]);
    status = MT_RPC_SUCCESS;

	return status;
}

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      calcFcs
 *
 * @brief   calculate the FCS (Frame Check Sequence) of the RPC payload.
 *
 * @param   msg  - pointer to the RPC general format frame message
 * @param   size - RPC general format frame size
 *
 * @return  FCS value
 */
static uint8_t calcFcs(uint8_t *msg, uint8_t size)
{
	uint8_t result = 0;

	// calculate FCS by XORing all bytes
	while (size--)
	{
		result ^= *msg++;
	}

	return result;
}

/*********************************************************************
 * @fn      printRpcMsg
 *
 * @brief   print out RPC message
 *
 * @param   preMsg - initial string
 * @param   sof - SOF (Start of Frame) bytes
 * @param   len - length byte
 * @param   msg  - pointer to the RPC message starting from Cmd0 byte
 *
 * @return  FCS value
 */
static void printRpcMsg(char* preMsg, uint8_t sof, uint8_t len, uint8_t *msg)
{
	uint8_t i;

	// print headers
	LOG_DBG("%s %d Bytes: SOF:%02X, Len:%02X, CMD0:%02X, CMD1:%02X",
	        preMsg, len + 5, sof, len, msg[0], msg[1]);

	// print frame payload
    LOG_DBG_NLR("Payload:");
	for (i = 2; i < len + 2; i++)
		LOG_DBG_NLR("%02X%s", msg[i], i < (len + 2 - 1) ? ":" : ",");

	// print FCS
	LOG_DBG("FCS:%02X", msg[i]);

}
