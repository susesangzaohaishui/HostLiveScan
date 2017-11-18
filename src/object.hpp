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

#ifndef __NET_OBJECT_HPP_INCLUDED__
#define __NET_OBJECT_HPP_INCLUDED__

#include "stdint.hpp"

namespace host
{
    struct command_t;
    class ctx_t;
    class io_thread_t;

    //  Base class for all objects that participate in inter-thread
    //  communication.

    class object_t
    {
    public:

        object_t (host::ctx_t *ctx_, uint32_t tid_);
        object_t (object_t *parent_);
        virtual ~object_t ();

        uint32_t get_tid ();
        void set_tid(uint32_t id);
        ctx_t *get_ctx ();
        void process_command (host::command_t &cmd_);

	protected:
		void send_stop ();
		void send_term (host::object_t *destination_);
		void send_term_req(host::object_t *destination_, host::object_t* reqobj_);
		void send_plug (host::object_t *destination_);
		void send_fetch_buf ();
		void send_command (command_t &cmd_);
    protected:

        //  Logs an message.
        void log (const char *format_, ...);

		void log_msg(const char* msg);

        //  Chooses least loaded I/O thread.
        host::io_thread_t *choose_io_thread (uint64_t affinity_);
        //  These handlers can be overloaded by the derived objects. They are
        //  called when command arrives from another thread.
        virtual void process_stop ();
        virtual void process_plug ();
		virtual void process_term_req(host::object_t* reqobj_);
		virtual void process_term();
		virtual void process_fetch_buf();
		virtual void process_destroy ();
		virtual void process_user_cmd(int cmd, long wparam, long lparam);
    private:

        //  Context provides access to the global state.
        host::ctx_t *ctx;

        //  Thread ID of the thread the object belongs to.
        uint32_t tid;
        object_t (const object_t&);
        const object_t &operator = (const object_t&);
    };

}

#endif
