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

#ifndef __NET_COMMAND_HPP_INCLUDED__
#define __NET_COMMAND_HPP_INCLUDED__

#include "stdint.hpp"
#include <time.h>

#ifdef	__USE_BSD
# ifndef __u_char_defined
typedef __u_char u_char;
typedef __u_short u_short;
typedef __u_int u_int;
typedef __u_long u_long;
typedef __quad_t quad_t;
typedef __u_quad_t u_quad_t;
typedef __fsid_t fsid_t;
#  define __u_char_defined
# endif
#endif

namespace host
{

    class object_t;
    //  This structure defines the commands that can be sent between threads.
    struct command_t
    {
        //  Object to process the command.
        host::object_t *destination;

        enum
        {
            cmd_id_attach_session = 1000,
            cmd_id_detach_session,
            cmd_id_active_send,
            cmd_id_connect_ok,
            cmd_id_connect_time_out
        };

        enum type_t
        {
            stop,
            plug,
            term_req,
            term,
            fetch_buf,
            done,
            user_cmd
        } type;

        union {

            //  Sent to I/O thread to let it know that it should
            //  terminate itself.
            struct {
            } stop;

            //  Sent to I/O object to make it register with its I/O thread.
            struct {
            } plug;

            //  Sent by I/O object ot the socket to request the shutdown of
            //  the I/O object.
            struct {
                host::object_t *object;
            } term_req;

            //  Sent by socket to I/O object to start its shutdown.
            struct {
            } term;

            struct {
            }fetch_buf;

            //  Sent by reaper thread to the term thread when all the sockets
            //  are successfully deallocated.
            struct {
            } done;

            struct {
                long  cmdId;
                long  wparam;
                long  lparam;
            }user_cmd;
        } args;
    };
    struct rec {					/* format of outgoing UDP data */
      u_short	rec_seq;			/* sequence number */
      u_short	rec_ttl;			/* TTL packet left with */
      struct timeval	rec_tv;		/* time packet left */
    };
}

#endif
