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

#ifndef __NET_TCP_LISTENER_HPP_INCLUDED__
#define __NET_TCP_LISTENER_HPP_INCLUDED__

#include "fd.hpp"
#include "object.hpp"
#include "stdint.hpp"
#include "io_object.hpp"
#include "tcp_address.hpp"
#include "options.hpp"
#include "net_common.h"
#include "i_linkinfo_t.h"

namespace net
{

    class io_thread_t;
    class tcp_listener_t : public object_t, public io_object_t
    {
	public:
        //  Set address to listen on.
        int set_address (const char *addr_);

        // Get the bound address for use with wildcard
        int get_address (std::string &addr_);

		 void set_link_info(const i_linkinfo_t&);
		 void get_link_info(i_linkinfo_t&);

	protected:
		tcp_listener_t (net::io_thread_t *io_thread_,const options_t &options_);
		~tcp_listener_t ();
	public:
		static tcp_listener_t* create_listener(net::io_thread_t *io_thread_,const options_t &options_);
    private:

        //  Handlers for incoming commands.
        void process_plug ();
        void process_term ();

        //  Handlers for I/O events.
        void in_event ();

        //  Close the listening socket.
        void close ();

        //  Accept the new connection. Returns the file descriptor of the
        //  newly created connection. The function may return retired_fd
        //  if the connection was dropped while waiting in the listen backlog
        //  or was denied because of accept filters.
        fd_t accept ();
    private:
        options_t options;

        //  Underlying socket.
        fd_t s;

        //  Address to listen on.
        tcp_address_t address;



        //  Handle corresponding to the listening socket.
        handle_t handle;

       // String representation of endpoint to bind to
        std::string endpoint;

		i_linkinfo_t _link_info;

        tcp_listener_t (const tcp_listener_t&);
        const tcp_listener_t &operator = (const tcp_listener_t&);
    };

}

#endif
