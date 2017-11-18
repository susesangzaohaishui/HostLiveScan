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

#include <string.h>
#include <stdarg.h>

#include "object.hpp"
#include "ctx.hpp"
#include "debug.h"
#include "err.hpp"
#include "io_thread.hpp"

host::object_t::object_t (ctx_t *ctx_, uint32_t tid_) :
    ctx (ctx_),
    tid (tid_)
{
}

host::object_t::object_t (object_t *parent_) :
    ctx (parent_->ctx),
    tid (parent_->tid)
{
}

host::object_t::~object_t ()
{
}

uint32_t host::object_t::get_tid ()
{
    return tid;
}

void host::object_t::set_tid(uint32_t id)
{
    tid = id;
}

host::ctx_t *host::object_t::get_ctx ()
{
    return ctx;
}

host::io_thread_t *host::object_t::choose_io_thread (uint64_t affinity_)
{
	return ctx->choose_io_thread (affinity_);
}

void host::object_t::log (const char *format_, ...)
{
	return;
}

void host::object_t::log_msg(const char* msg)
{
   // debug_str(msg);
}

void host::object_t::send_plug (host::object_t *destination_)
{
	command_t cmd;
	cmd.destination = destination_;
	cmd.type = command_t::plug;
	send_command (cmd);
}

void host::object_t::send_stop ()
{
	//  'stop' command goes always from administrative thread to
	//  the current object. 
	command_t cmd;
	cmd.destination = this;
	cmd.type = command_t::stop;
	ctx->send_command (tid, cmd);
}

void host::object_t::send_term (host::object_t *destination_)
{
	command_t cmd;
	cmd.destination = destination_;
	cmd.type = command_t::term;
	send_command (cmd);
}

void host::object_t::send_term_req(host::object_t *destination_, host::object_t* reqobj_)
{
	command_t cmd;
	cmd.destination = destination_;
	cmd.type = command_t::term_req;
	cmd.args.term_req.object = reqobj_;
	send_command (cmd);
}

void host::object_t::send_fetch_buf ()
{
	command_t cmd;
	cmd.destination = this;
	cmd.type = command_t::fetch_buf;
	ctx->send_command (tid, cmd);
}

void host::object_t::process_command (command_t &cmd_)
{
    switch (cmd_.type) {

    case command_t::stop:
        process_stop ();
        break;

    case command_t::plug:
        process_plug ();
        break;

    case command_t::fetch_buf:
         process_fetch_buf();
         break;

    case command_t::term:
         process_term();
         break;

    case command_t::term_req:
         process_term_req(cmd_.args.term_req.object);
         break;
    case command_t::user_cmd:
        process_user_cmd(cmd_.args.user_cmd.cmdId,
              cmd_.args.user_cmd.wparam,
              cmd_.args.user_cmd.lparam);
         break;
    default:
        net_assert (false);
    }
}

void host::object_t::process_stop ()
{
    net_assert (false);
}

void host::object_t::process_plug ()
{
    net_assert (false);
}

void host::object_t::process_fetch_buf()
{
	net_assert (false);
}

void host::object_t::process_term_req(host::object_t* reqobj_)
{
	net_assert (false);
}

void host::object_t::process_term()
{
	net_assert (false);
}

void host::object_t::process_user_cmd(int cmd, long wparam, long lparam)
{
	net_assert (false);
}

void host::object_t::process_destroy ()
{
	delete this;
}

void host::object_t::send_command (command_t &cmd_)
{
    ctx->send_command (cmd_.destination->get_tid (), cmd_);
}



