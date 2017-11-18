/*
    Copyright (c) 2007-2013 Contributors as noted in the AUTHORS file

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __NET_UTILS_H_INCLUDED__
#define __NET_UTILS_H_INCLUDED__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*  Define integer types needed for event interface                          */
#if defined NET_HAVE_SOLARIS || defined NET_HAVE_OPENVMS
#   include <inttypes.h>
#elif defined _MSC_VER && _MSC_VER < 1600
#   ifndef int32_t
typedef __int32 int32_t;
#   endif
#   ifndef uint16_t
typedef unsigned __int16 uint16_t;
#   endif
#else
#   include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void (net_thread_fn) (void*);

/*  These functions are not documented by man pages                           */

/*  Helper functions are used by perf tests so that they don't have to care   */
/*  about minutiae of time-related functions on different OS platforms.       */

/*  Starts the stopwatch. Returns the handle to the watch.                    */
void *net_stopwatch_start (void);

/*  Stops the stopwatch. Returns the number of microseconds elapsed since     */
/*  the stopwatch was started.                                                */
unsigned long net_stopwatch_stop (void *watch_);

/*  Sleeps for specified number of seconds.                                   */
void net_sleep (int seconds_);

/* Start a thread. Returns a handle to the thread.                            */
void *net_threadstart (net_thread_fn* func, void* arg);

/* Wait for thread to complete then free up resources.                        */
void net_threadclose (void* thread);


unsigned long get_local_addr();

char*    get_local_ip ();

#ifdef __cplusplus
}
#endif

#endif
