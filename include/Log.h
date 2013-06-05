/*
 * Log.h
 *
 * Copyright (C) 2013  Emil Penchev, Bulgaria
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 *  Created on: Feb 11, 2013
 *      Author: emo
 */

#ifndef LOG_H_
#define LOG_H_

#include <cstdarg>
#include <cstdio>

#define BLITZ_TRACE_ERROR
#define BLITZ_TRACE_WARN
#define BLITZ_TRACE_INFO

#if defined(BLITZ_TRACE_ERROR)
#define BLITZ_LOG_ERROR(fmt, args...) printf("[ ERROR ] [%s %s():%d] " fmt "\n", __FILE__, __FUNCTION__, __LINE__, ##args); fflush(NULL)
#else
#define BLITZ_LOG_ERROR(fmt, args...)
#endif

#if defined(BLITZ_TRACE_INFO)
#define BLITZ_LOG_INFO(fmt, args...) printf("[ INFO  ] [%s %s():%d] " fmt "\n", __FILE__, __FUNCTION__, __LINE__, ##args); fflush(NULL)
#else
#define BLITZ_LOG_INFO(fmt, args...)
#endif

#if defined(BLITZ_TRACE_WARN)
#define BLITZ_LOG_WARNING(fmt, args...) printf("[ WARN  ] [%s %s():%d] " fmt "\n", __FILE__, __FUNCTION__, __LINE__, ##args); fflush(NULL)
#else
#define BLITZ_LOG_WARNING(fmt, args...)
#endif




// Templates here
/**
 @file  :  Log.h
 Summary:  Debug header

 FULL_DUMP (global dump) or DUMP (for file-level dump) must be defined
 before calling this header file if debug output required

 To be able to use the DBG macro do the following:
 1. Somewhere in your Makefile hierarchy look for the FULL_DUMP env-var:
      ifneq "$(FULL_DUMP)" ""
      ifneq "$(FULL_DUMP)" "0"
        DCEAPP_CFLAGS += -DFULL_DUMP
      endif
      endif

 2. Then in a target compile command use the DCEAPP_CFLAGS variable:
     $(CC) $(DCEAPP_CFLAGS) -Werror ...

 3. Then use the macro in the code:
      errcode = db_init();
      DBG("errcode = 0x%x\n");
      return errcode;

 4. Then when building, call make with the FULL_DUMP env. var set to 1:
      FULL_DUMP=1 make all
 */

#ifndef __DCE_DEBUG_H
#define __DCE_DEBUG_H

#include <stdio.h>

// #define DUMP

// Define several variants of the DBG macro...
#if defined( DUMP )
extern  int g_trace_lvl;

#undef PRINT_THREAD_IN_DBG
#undef PRINT_TIME_IN_DBG

// Variant 1. DBG macros print time stamp + location in code + user message
#if defined(PRINT_TIME_IN_DBG)
#include <sys/timeb.h>
#include <time.h>

#define DBG(fmt, args...)    do{                    \
            struct tm * p_tm;                       \
            struct timeb var_timeb;                 \
            ftime( &var_timeb );                    \
            p_tm = localtime( &var_timeb.time );    \
            printf("[%02d:%02d:%03d] %s %s:%d : " fmt, p_tm->tm_min, p_tm->tm_sec, var_timeb.millitm, __FUNCTION__,__FILE__, __LINE__, ##args);fflush(stdout); }while(0)


// Variant 2. DBG macros print caller thread + location in code + user message
#elif defined(PRINT_THREAD_IN_DBG)
#include <pthread.h>
#define DBG(fmt, args...)   printf("0x%lx [%s %s:%d]: " fmt, pthread_self(), __FUNCTION__, __FILE__, __LINE__, ##args);fflush(stdout)


// Variant 3. DBG macros print location in code + user message
#else
#define DBG(fmt, args...)   printf("[%s %s:%d]: " fmt, __FUNCTION__, __FILE__, __LINE__, ##args);fflush(NULL)
#endif  // PRINT_TIME_IN_DBG


// Variant 4. DBG macros disabled
#else

#define DBG(fmt, args...)

#endif // ... end of DBG macro variants


// These macros are based on DBG
#define RETURN_ERR(e)   do{ DBG("ERROR 0x%x\n", (e)); return (e);}while(0)

#define EXIT_ERR(e)     do{ DBG("ERROR 0x%x\n", (e)); return;}while(0)

#define BREAK_ERR(e)    DBG("ERROR 0x%x\n", (e)); break;

// DBG_V macro (based on DBG) is for "spammy" messages
#if defined( DUMP ) && defined( DUMP_VERBOSE )
#define DBG_V DBG
#else
#define DBG_V(fmt, args...)
#endif // ... end of DBG_V macro variants


/* #define TRACE */

// Define several variants of the TRACE_... macros

#if defined(FULL_TRACE) || defined(TRACE)
extern  int g_trace_lvl;

#undef PRINT_THREAD_IN_TRACE
#undef PRINT_TIME_IN_TRACE

// Variant 1. TRACE macros print time stamp + location in code + user message
#if defined(PRINT_TIME_IN_TRACE)
#include <sys/timeb.h>
#include <time.h>

#define TRACE_IN()      { do{                        \
            struct tm * p_tm;                       \
            struct timeb var_timeb;                 \
            ftime( &var_timeb );                    \
            p_tm = localtime( &var_timeb.time );    \
            printf("TRACE IN  %*s> %s  [%02d:%02d:%03d] %s:%d\n", (++g_trace_lvl)*2, "", __FUNCTION__, p_tm->tm_min, p_tm->tm_sec, var_timeb.millitm, __FILE__, __LINE__);fflush(stdout); }while(0)

#define TRACE_OUT()      do{                        \
            struct tm * p_tm;                       \
            struct timeb var_timeb;                 \
            ftime( &var_timeb );                    \
            p_tm = localtime( &var_timeb.time );    \
            printf("TRACE OUT %*s< %s  [%02d:%02d:%03d] %s:%d\n", (--g_trace_lvl+1)*2, "", __FUNCTION__, p_tm->tm_min, p_tm->tm_sec, var_timeb.millitm, __FILE__, __LINE__);fflush(stdout); }while(0); }

#define TRACE_RET()      do{                        \
            struct tm * p_tm;                       \
            struct timeb var_timeb;                 \
            ftime( &var_timeb );                    \
            p_tm = localtime( &var_timeb.time );    \
            printf("TRACE OUT %*s< %s  [%02d:%02d:%03d] %s:%d\n", (--g_trace_lvl+1)*2, "", __FUNCTION__, p_tm->tm_min, p_tm->tm_sec, var_timeb.millitm, __FILE__, __LINE__);fflush(stdout); }while(0)

#define TRACE_MSG(fmt, args...)    do{              \
            struct tm * p_tm;                       \
            struct timeb var_timeb;                 \
            ftime( &var_timeb );                    \
            p_tm = localtime( &var_timeb.time );    \
            printf("TRACE msg %*s  %s  [%02d:%02d:%03d] %s:%d : " fmt, (g_trace_lvl)*2, "", __FUNCTION__, p_tm->tm_min, p_tm->tm_sec, var_timeb.millitm, __FILE__, __LINE__, ##args);fflush(stdout); }while(0)


// Variant 2. TRACE macros print caller thread + location in code + user message
#elif defined(PRINT_THREAD_IN_TRACE)
#include <pthread.h>
#define TRACE_IN()          {   printf("TRACE IN  %*s> %s  [0x%lx] %s:%d\n", (++g_trace_lvl)*2, "", __FUNCTION__, pthread_self(),__FILE__, __LINE__);fflush(stdout)
#define TRACE_OUT()             printf("TRACE OUT %*s< %s  [0x%lx] %s:%d\n", (--g_trace_lvl+1)*2, "", __FUNCTION__, pthread_self(),__FILE__, __LINE__);fflush(stdout); }
#define TRACE_RET()             printf("TRACE OUT %*s< %s  [0x%lx] %s:%d\n", (--g_trace_lvl+1)*2, "", __FUNCTION__, pthread_self(),__FILE__, __LINE__);fflush(stdout)
#define TRACE_MSG(fmt, args...) printf("TRACE msg %*s  %s  [0x%lx] %s:%d : " fmt, (g_trace_lvl)*2, "", __FUNCTION__, pthread_self(), __FILE__, __LINE__, ##args);fflush(stdout)


// Variant 3. TRACE macros print location in code + user message
#else
#define TRACE_IN()          {   printf("TRACE IN  %*s> %s  %s:%d\n", (++g_trace_lvl)*2, "", __FUNCTION__, __FILE__, __LINE__);fflush(stdout)
#define TRACE_OUT()             printf("TRACE OUT %*s< %s  %s:%d\n", (--g_trace_lvl+1)*2, "", __FUNCTION__, __FILE__, __LINE__);fflush(stdout); }
#define TRACE_RET()             printf("TRACE OUT %*s< %s  %s:%d\n", (--g_trace_lvl+1)*2, "", __FUNCTION__, __FILE__, __LINE__);fflush(stdout)
#define TRACE_MSG(fmt, args...) printf("TRACE msg %*s  %s  %s:%d : " fmt, (g_trace_lvl)*2, "", __FUNCTION__, __FILE__, __LINE__, ##args);fflush(stdout)
#endif  // PRINT_TIME_IN_TRACE


// Variant 4. TRACE macros disabled
#else

#define TRACE_IN()  {
#define TRACE_OUT() }
#define TRACE_RET()
#define TRACE_MSG(fmt, args...)

#endif // ... end of TRACE macro variants


#endif /* __DEBUG_H */




#endif /* LOG_H_ */
