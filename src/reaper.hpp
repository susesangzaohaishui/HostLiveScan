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

#ifndef __NET_REAPER_HPP_INCLUDED__
#define __NET_REAPER_HPP_INCLUDED__

#include "object.hpp"
#include "mailbox.hpp"
#include "poller.hpp"
#include "data_queue.hpp"
#include "i_poll_events.hpp"
#include "i_queue_service.hpp"
#include "i_linkinfo_t.h"
#include "host_scan.h"
#include <map>
#include <list>

namespace host
{
    class ctx_t;
 //   class tcp_connecter_t;
    class sessionbase_t;
//	class tcp_listener_t;
    class session_node;
    class reaper_t : public object_t,
                     public i_poll_events,
                     public i_queue_service
    {
    public:
        reaper_t (host::ctx_t *ctx_, uint32_t tid_);
        ~reaper_t ();

        mailbox_t *get_mailbox ();

        void start ();
        void stop ();

        //  i_poll_events implementation.
        void in_event ();
        void out_event ();
        void timer_event (int id_);

        void add_queue_item(int identifier, void * const buf, int size);

         bool create_link(const char* xml_path);

         void open_all_link();

         void set_logout_fun(PFN_NET_OUT_TIME func){_out_func = func;}

         void set_replay_fun(PFN_RECV_REPALY func){_recv_func = func;}

         PFN_RECV_REPALY get_replay_fun(){ return _recv_func;}
    private:

        //  Command handlers.
        void process_stop ();

        void process_plug();

        void process_fetch_buf();

        void process_term_req(host::object_t* reqobj_);

        void process_user_cmd(int cmd, long wparam, long lparam);

        bool get_queue_item(tagDataHead &data_head, void * buf, int size);

        void add_timer (int timeout_, int id_);

        void cancel_timer (int id_);



    protected:

        void process_net_message(void* msg, int msglen);

        void send_session_data(int link_id, int fd, void* buf, int buflen);


        void insert_session(int link_id, sessionbase_t* s);

        void remove_session(sessionbase_t* s);

        sessionbase_t* find_session(int link_id, int fd = 0);

        void session_leave(sessionbase_t* s);

        void session_time_out(sessionbase_t *s);
    private:

        //  Reaper thread accesses incoming commands via this mailbox.
        mailbox_t mailbox;

        //  Handle associated with mailbox' file descriptor.
        poller_t::handle_t mailbox_handle;

        //  I/O multiplexing is performed using a poller object.
        poller_t *poller;

        //  Number of sockets being reaped at the moment.
        int sockets;

        //  If true, we were already asked to terminate.
        bool terminating;

        std::map<int, session_node*>  _connecters;
        //std::map<int, tcp_listener_t*>  _listeners;

        std::map<int, std::list<sessionbase_t*> >    _sessions;

        std::map<int, i_linkinfo_t>      _link_info_map;

        mutex_t      sync_queue;
        data_queue_t workqueue;
        char		  workbuffer[NET_MAX_ASYNCHRONISM_DATA];


        PFN_RECV_REPALY   _recv_func;
        PFN_NET_OUT_TIME  _out_func;

        typedef std::map<int, session_node*>::iterator SESSION_ITERATOR;
        //typedef std::map<int, tcp_connecter_t*>::iterator CONN_ITERATOR;
    private:

     private:
        reaper_t (const reaper_t&);
        const reaper_t &operator = (const reaper_t&);

#ifdef HAVE_FORK
        // the process that created this context. Used to detect forking.
        pid_t pid;
#endif
    };

}

#endif
