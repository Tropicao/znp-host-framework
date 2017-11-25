/*
 * dbgPrint.c
 *
 * This module contains the API for debug print.
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
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "dbgPrint.h"

/*********************************************************************
 * MACROS
 */

#define PRINT_LEVEL_ENV "LOG_LEVEL"

/*********************************************************************
 * LOCAL VARIABLE
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/**
 * @brief Function used to check if an environment variable has been set for
 * loglevel
 *
 * @return The set log level if a correct value has been set, otherwise the
 * default log level value
 */
static int _get_log_level()
{
    int result = -1;
    char *env = getenv(PRINT_LEVEL_ENV);
    if(env)
    {
        if(strcmp(env, "LOG_ERROR") == 0)
           result = PRINT_LEVEL_ERROR;
        else if(strcmp(env, "LOG_WARNING") == 0)
           result = PRINT_LEVEL_WARNING;
        else if(strcmp(env, "LOG_INFO") == 0)
           result = PRINT_LEVEL_INFO;
        else if(strcmp(env, "LOG_INFO_LOWLEVEL") == 0)
           result = PRINT_LEVEL_INFO_LOWLEVEL;
        else if(strcmp(env, "LOG_VERBOSE") == 0)
           result = PRINT_LEVEL_VERBOSE;
    }

    if(result == -1)
        result = PRINT_LEVEL;
    return result;
}



/*********************************************************************
 * API FUNCTIONS
 */

/**************************************************************************************************
 * @fn          dbgPrint
 *
 * @brief       This function checks the print level and prints if required.
 *
 * input parameters
 *
 * @param       simpleDesc - A pointer to the simpleDesc descriptor to register.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
void dbg_print(int print_level, const char *fmt, ...)
{
	if (print_level > _get_log_level())
	{
		return;
	}
	else
	{
		va_list argp;
		va_start(argp, fmt);
		vprintf(fmt, argp);
		va_end(argp);
	}
}
