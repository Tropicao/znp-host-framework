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
#include <time.h>

#include "dbgPrint.h"

/*********************************************************************
 * MACROS
 */

#define PRINT_LEVEL_ENV     "LOG_LEVEL"
#define ANSI_COLOR_RED      "\x1b[31m"
#define ANSI_COLOR_GREEN    "\x1b[32m"
#define ANSI_COLOR_YELLOW   "\x1b[33m"
#define ANSI_COLOR_BLUE     "\x1b[34m"
#define ANSI_COLOR_MAGENTA  "\x1b[35m"
#define ANSI_COLOR_CYAN     "\x1b[36m"
#define ANSI_COLOR_RESET    "\x1b[0m"
#define MAX_LOG_LINE_SIZE   512
#define PREFIX_CRI          "CRI"
#define PREFIX_ERR          "ERR"
#define PREFIX_WARN         "WARN"
#define PREFIX_INF          "INF"
#define PREFIX_DBG          "DBG"
#define PREFIX_NONE         "..."

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
        if(strcmp(env, "LOG_CRI") == 0)
           result = PRINT_LEVEL_CRI;
        else if(strcmp(env, "LOG_ERR") == 0)
           result = PRINT_LEVEL_ERR;
        else if(strcmp(env, "LOG_WARN") == 0)
           result = PRINT_LEVEL_WARN;
        else if(strcmp(env, "LOG_INF") == 0)
           result = PRINT_LEVEL_INF;
        else if(strcmp(env, "LOG_DBG") == 0)
           result = PRINT_LEVEL_DBG;
    }

    if(result == -1)
        result = PRINT_LEVEL;
    return result;
}


/**
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
static void dbg_print(int print_level, const char *fmt, va_list argp)
{
    char buffer[MAX_LOG_LINE_SIZE] = {0};
    time_t rawtime;
    struct tm *tm_cur;
    char *color, *prefix;
    int index = -1;

	if (print_level > _get_log_level())
	{
		return;
	}

    switch(print_level)
    {
        case PRINT_LEVEL_CRI:
            color = ANSI_COLOR_RED;
            prefix = PREFIX_CRI;
            break;
        case PRINT_LEVEL_ERR:
            color = ANSI_COLOR_YELLOW;
            prefix = PREFIX_ERR;
            break;
        case PRINT_LEVEL_WARN:
            color = ANSI_COLOR_MAGENTA;
            prefix = PREFIX_WARN;
            break;
        case PRINT_LEVEL_INF:
            color = ANSI_COLOR_GREEN;
            prefix = PREFIX_INF;
            break;
        case PRINT_LEVEL_DBG:
            color = ANSI_COLOR_BLUE;
            prefix = PREFIX_DBG;
            break;
        default:
            color = ANSI_COLOR_RESET;
            prefix = PREFIX_NONE;
            break;
    }


    time(&rawtime);
    tm_cur = localtime (&rawtime);
    index = snprintf(buffer, MAX_LOG_LINE_SIZE, "%02d/%02d/%04d %02d:%02d:%02d %s%5s%s : ",
            tm_cur->tm_mday, tm_cur->tm_mon+1, tm_cur->tm_year + 1900, tm_cur->tm_hour, tm_cur->tm_min, tm_cur->tm_sec,
            color, prefix, ANSI_COLOR_RESET);
    if(index < 0)
        return;

    vsnprintf(buffer + index, MAX_LOG_LINE_SIZE - index, fmt, argp);
    fprintf(stdout, "%s\n", buffer);
}


/*********************************************************************
 * API FUNCTIONS
*********************************************************************/

void log_cri(const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    dbg_print(PRINT_LEVEL_CRI, fmt, argp);
    va_end(argp);
}

void log_err(const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    dbg_print(PRINT_LEVEL_ERR, fmt, argp);
    va_end(argp);
}

void log_warn(const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    dbg_print(PRINT_LEVEL_WARN, fmt, argp);
    va_end(argp);
}

void log_inf(const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    dbg_print(PRINT_LEVEL_INF, fmt, argp);
    va_end(argp);
}

void log_dbg(const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    dbg_print(PRINT_LEVEL_DBG, fmt, argp);
    va_end(argp);
}
