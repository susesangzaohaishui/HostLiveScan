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

#include "platform.hpp"

#include "clock.hpp"
#include "err.hpp"
#include "thread.hpp"
#include "net_utils.h"
#include <assert.h>
#if !defined NET_HAVE_WINDOWS
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include "windows.hpp"
#endif
#ifdef HAVE_LIBSODIUM
#   include <sodium.h>
#endif



void net_sleep (int seconds_)
{
#if defined NET_HAVE_WINDOWS
    Sleep (seconds_ * 1000);
#else
    sleep (seconds_);
#endif
}

void *net_stopwatch_start ()
{
    uint64_t *watch = (uint64_t*) malloc (sizeof (uint64_t));
    alloc_assert (watch);
    *watch = host::clock_t::now_us ();
    return (void*) watch;
}

unsigned long net_stopwatch_stop (void *watch_)
{
    uint64_t end = host::clock_t::now_us ();
    uint64_t start = *(uint64_t*) watch_;
    free (watch_);
    return (unsigned long) (end - start);
}

void *net_threadstart(net_thread_fn* func, void* arg)
{
    host::thread_t* thread = new host::thread_t;
    thread->start(func, arg);
    return thread;
}

void net_threadclose(void* thread)
{
    host::thread_t* pThread = static_cast<host::thread_t*>(thread);
    pThread->stop();
    delete pThread;
}


unsigned long get_local_addr()
{
    char  hostname[255] = {0};
    hostent*  hostinfo;
    if(  gethostname  (  hostname,  sizeof(hostname))  ==  0)
    {
        if((hostinfo  =  gethostbyname(hostname))  !=  NULL)
        {
            in_addr ia = *(struct  in_addr  *)*hostinfo->h_addr_list;
            return ia.s_addr;
        }
    }

    return -1;
}

char*    get_local_ip ()
{
    char  hostname[255];
    hostent*  hostinfo = NULL;
    char* ip = NULL;
    if(  gethostname  (  hostname,  sizeof(hostname))  ==  0)
    {
        if((hostinfo  =  gethostbyname(hostname))  !=  NULL)
        {
            ip  =  inet_ntoa  (*(struct  in_addr  *)hostinfo->h_addr_list[0]);
        }
    }

    return ip;
}
