/*
 * dbgPrint.h
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
#ifndef DBGPRINT_H
#define DBGPRINT_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define LOG_CRI(x, ...)        log_cri(__FILE__, __func__, x,  ##__VA_ARGS__)
#define LOG_ERR(x, ...)        log_err(__FILE__, __func__, x, ##__VA_ARGS__)
#define LOG_WARN(x, ...)       log_warn(__FILE__, __func__, x, ##__VA_ARGS__)
#define LOG_INF(x, ...)        log_inf(__FILE__, __func__, x, ##__VA_ARGS__)
#define LOG_DBG(x, ...)        log_dbg(__FILE__, __func__, x, ##__VA_ARGS__)
#define LOG_DBG_NLR(x, ...)    log_dbg_no_line_return(__FILE__, __func__, x, ##__VA_ARGS__)

enum
{
	PRINT_LEVEL_CRI,
	PRINT_LEVEL_ERR,
	PRINT_LEVEL_WARN,
	PRINT_LEVEL_INF,
	PRINT_LEVEL_DBG
};

#define PRINT_LEVEL PRINT_LEVEL_WARN
//#define PRINT_LEVEL PRINT_LEVEL_VERBOSE

void log_cri(const char *file, const char *func, const char *fmt, ...);
void log_err(const char *file, const char *func, const char *fmt, ...);
void log_warn(const char *file, const char *func, const char *fmt, ...);
void log_inf(const char *file, const char *func, const char *fmt, ...);
void log_dbg(const char *file, const char *func, const char *fmt, ...);
void log_dbg_no_line_return(const char *file, const char *func, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* DBGPRINT_H */
