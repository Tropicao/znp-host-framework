/*
 * mqueue.c
 *
 * This module contains the POSIX wrapper for Semaphore.
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include "queue.h"

static void addToHead(llq_t *hndl, char *data, int length)
{
	if (hndl->head == NULL)
	{
		hndl->head = (node_t *) malloc(1 * sizeof(node_t));
		hndl->head->ptr = NULL;
		hndl->head->data = (char *) malloc(length * (sizeof(char)));
		hndl->head->length = length;
		memcpy(hndl->head->data, data, length);
		hndl->tail = hndl->head;
	}
	else
	{
		hndl->temp = (node_t *) malloc(1 * sizeof(node_t));
		hndl->temp->ptr = hndl->head;
		hndl->temp->data = (char *) malloc(length * (sizeof(char)));
		hndl->temp->length = length;
		memcpy(hndl->temp->data, data, length);

		hndl->head = hndl->temp;
	}
}

static void addToTail(llq_t *hndl, char *data, int length)
{

	if (hndl->tail == NULL)
	{
		hndl->tail = (node_t *) malloc(1 * sizeof(node_t));
		hndl->tail->ptr = NULL;
		hndl->tail->data = (char *) malloc(length * (sizeof(char)));
		hndl->tail->length = length;
		memcpy(hndl->tail->data, data, length);
		hndl->head = hndl->tail;
	}
	else
	{
		hndl->temp = (node_t *) malloc(1 * sizeof(node_t));
		hndl->tail->ptr = hndl->temp;
		hndl->temp->data = (char *) malloc(length * (sizeof(char)));
		hndl->temp->length = length;
		memcpy(hndl->temp->data, data, length);
		hndl->temp->ptr = NULL;

		hndl->tail = hndl->temp;
	}
}

/*********************************************************************
 * @fn      llq_open
 *
 * @brief   Create a queue handle
 *
 * @param    llq_t *hndl - handle to queue to be created
 *
 * @return   none
 */
void llq_open(llq_t *hndl)
{
	hndl->head = hndl->tail = NULL;
}

void llq_close(llq_t *hndl __attribute__((unused)))
{

}

/*********************************************************************
 * @fn      llq_receive
 *
 * @brief   Get next message in queue
 *
 * @param   llq_t *hndl - handle to queue to read the message from
 * @Param	char *buffer - Pointer to buffer to read the message in to
 * @Param	int maxLength - Max length of message to read
 *
 * @return   length of message read from queue
 */
int llq_receive(llq_t *hndl, char *buffer, int maxLength __attribute__((unused)))
{
	int rLength = -1;

    if (hndl->head != NULL)
    {
        hndl->temp = hndl->head->ptr;
        memcpy(buffer, hndl->head->data, hndl->head->length);
        rLength = (int) hndl->head->length;

        //did head point to another element
        if (hndl->temp != NULL)
        {
            //free current head element and point head to next
            free(hndl->head->data);
            free(hndl->head);
            hndl->head = hndl->temp;
        }
        else
        {
            //no elements left in queue
            free(hndl->head->data);
            free(hndl->head);
            hndl->head = NULL;
            hndl->tail = NULL;
        }
    }
	return rLength;
}

/*********************************************************************
 * @fn      llq_add
 *
 * @brief   write message to queue
 *
 * @param   llq_t *hndl - handle to queue to read the message from
 * @Param	char *buffer - Pointer to buffer containing the message
 * @Param	int len - Length of message
 * @Param	int prio - 1 message has priority and should be added to
 * 			head of queue, 0 message assed to tail of queue
 *
 * @return   length of message read from queue
 */
int llq_add(llq_t *hndl, char *buffer, int len, int prio)
{
	int ret = 0;

	if (prio == 1)
	{
		addToHead(hndl, buffer, len);
	}
	else
	{
		addToTail(hndl, buffer, len);
	}

	return ret;
}

