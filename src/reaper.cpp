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

#include "reaper.hpp"
#include "err.hpp"
#include "session.hpp"
#include "msg_define.hpp"
#include "net_utils.h"
#include "options.hpp"
#include "tinyxml.h"
#include "ip.hpp"
#include "debug.h"
#include <algorithm>

host::reaper_t::reaper_t (class ctx_t *ctx_, uint32_t tid_) :
    object_t (ctx_, tid_),
    sockets (0),
    terminating (false)
{
    poller = new (std::nothrow) poller_t;
    alloc_assert (poller);

    mailbox_handle = poller->add_fd (mailbox.get_fd (), this);
    poller->set_pollin (mailbox_handle);

#ifdef HAVE_FORK
    pid = getpid();
#endif

    _recv_func = 0;
    _out_func = 0;
}

host::reaper_t::~reaper_t ()
{
    delete poller;
}

host::mailbox_t *host::reaper_t::get_mailbox ()
{
    return &mailbox;
}

void host::reaper_t::start ()
{
    //  Start the thread.
    poller->start ();
}

void host::reaper_t::stop ()
{
    send_stop ();
}


bool host::reaper_t::create_link(const char* xml_path)
{
    TiXmlDocument   doc;

    if(!doc.LoadFile(xml_path))
    {
        return  false;
    }
    TiXmlElement*   rootElement =   doc.RootElement();
    if (rootElement == 0)
    {
        return  false;
    }
    TiXmlElement*   pElement    =   rootElement->FirstChildElement();
    if (pElement == 0)
    {
        return  false;
    }


    for ( ;pElement; pElement =  pElement->NextSiblingElement())
    {
        const char* pLinkId     =   pElement->Attribute("m_node_id");
        if (pLinkId == NULL)
        {
            continue;
        }
        size_t      nLinkId     =   (size_t)atol(pLinkId);

        i_linkinfo_t  linkinfo;
        /**
        *   首先查看是否已经存在
        */
        linkinfo.m_node_id   = nLinkId;

        const char* strIP                  =   pElement->Attribute("m_node_ip");
        if (strIP != 0)
        {
            memcpy(linkinfo.m_node_ip, strIP, sizeof(char) * 120);
        }


        const char* strTime             =   pElement->Attribute("m_scan_time");
        if (strTime != 0)
        {
            linkinfo.m_scan_time        =   (size_t)atol(strTime);
        }
        const char* strTimeOut             =   pElement->Attribute("m_time_out");
        if (strTimeOut != 0)
        {
            linkinfo.m_time_out        =   (size_t)atol(strTimeOut);
        }
        _link_info_map[linkinfo.m_node_id ] = linkinfo;
    }
    return  true;
}

void host::reaper_t::open_all_link()
{
    send_plug(this);
}

void host::reaper_t::in_event ()
{
    while (true) {
#ifdef HAVE_FORK
        if (unlikely(pid != getpid()))
        {
            //printf("host::reaper_t::in_event return in child process %d\n", (int)getpid());
            return;
        }
#endif

        //  Get the next command. If there is none, exit.
        command_t cmd;
        int rc = mailbox.recv (&cmd, 0);
        if (rc != 0 && errno == EINTR)
            continue;
        if (rc != 0 && errno == EAGAIN)
            break;
        errno_assert (rc == 0);

        //  Process the command.
        cmd.destination->process_command (cmd);
    }
}

void host::reaper_t::out_event ()
{
    net_assert (false);
}

void host::reaper_t::timer_event (int)
{
    net_assert (false);
}

void host::reaper_t::add_queue_item(int identifier, void * const buf, int size)
{
    scoped_lock_t autolocker(sync_queue);
    workqueue.push_item(identifier, buf, size);
    send_fetch_buf();
}

void host::reaper_t::process_stop ()
{
    terminating = true;
    //  If there are no sockets being reaped finish immediately.
    poller->rm_fd (mailbox_handle);
    poller->stop ();
}

void host::reaper_t::process_plug()
{
    options_t options;
    std::map<int, i_linkinfo_t>::iterator i = _link_info_map.begin();
    for (; i!= _link_info_map.end(); ++i)
    {
        i_linkinfo_t& link_info = i->second;

        if(!link_info.m_scan_time)
        {
            continue;
        }

        io_thread_t* io_thread = choose_io_thread (options.affinity);
        if (!io_thread) {
            errno = EMTHREAD;
            return;
        }
        debug_str("start connect remoteLink Ip = %s *********\n", link_info.m_node_ip);
        session_node* connector = new session_node(io_thread, this );
        connector->set_link_info(link_info);
        _connecters[link_info.m_node_id] = connector;
        send_plug(connector);
    }
}


void host::reaper_t::process_fetch_buf()
{
    //获取数据
    tagDataHead data_head;
    bool ret = get_queue_item(data_head, workbuffer, sizeof(workbuffer));
    net_assert(ret);

    switch(data_head.wIdentifier)
    {
    case queue_msg_net_data:
    {
        process_net_message(workbuffer, data_head.wDataSize);
    }
        break;
    case queue_msg_send_data:
    {
        host::queue_msg_header* msg_header = (host::queue_msg_header*)workbuffer;
        int offset = sizeof(host::queue_msg_header);
        send_session_data(msg_header->msg_send.linkid,
                          msg_header->msg_send.fd,
                          workbuffer + offset,
                          msg_header->msg_send.buflen - offset);
    }
        break;
    default:
        break;
    }
}

void host::reaper_t::send_session_data(int link_id, int fd, void* buf, int buflen)
{
//    net_msg_header* pHeader = (net_msg_header*)buf;
//    SESSION_ITERATOR i = _sessions.find(link_id);
//    if (i == _sessions.end())
//    {
//        return;
//    }

//    i_linkinfo_t link_info = _link_info_map[link_id];

//    std::list<sessionbase_t*>& l = i->second;

//    net_assert(!l.empty());

//    switch(link_info.m_node_type)
//    {
//    case LT_TCP_CLIENT:
//    case LT_UDP:
//    {
//        sessionbase_t* s = l.front();
//        s->asyn_send_buf(buf, buflen);
//    }
//        break;
//    case LT_TCP_SERVER:
//    {
//        if(0 == fd)
//        {
//            std::list<sessionbase_t*>::iterator i_s = l.begin();
//            for(; i_s != l.end(); ++i_s)
//            {
//                sessionbase_t* s = *i_s;
//                s->asyn_send_buf(buf, buflen);
//            }
//        }
//        else
//        {
//            std::list<sessionbase_t*>::iterator i_s = l.begin();
//            for(; i_s != l.end(); ++i_s)
//            {
//                sessionbase_t* s = *i_s;
//                if(s->get_fd() == fd)
//                {
//                    s->asyn_send_buf(buf, buflen);
//                    break;
//                }
//            }
//        }
//    }
//        break;
//    default:
//        break;
//    }
}

void host::reaper_t::insert_session(int link_id, sessionbase_t* s)
{
//    SESSION_ITERATOR i = _sessions.find(link_id);
//    if (i != _sessions.end())
//    {
//        i->second.push_back(s);
//    }
//    else
//    {
//        std::list<sessionbase_t*> l;
//        l.push_back(s);
//        _sessions[link_id] = l;
//    }

}

void host::reaper_t::remove_session(sessionbase_t* s)
{
    i_linkinfo_t link_info;
    s->get_link_info(link_info);
    SESSION_ITERATOR i = _connecters.find(link_info.m_node_id);
    if (i != _connecters.end())
    {
            _connecters.erase(i);
    }
}

host::sessionbase_t* host::reaper_t::find_session(int link_id, int fd)
{
//    SESSION_ITERATOR i = _sessions.find(link_id);
//    if (i != _sessions.end())
//    {
//        std::list<sessionbase_t*>& l = i->second;
//        std::list<sessionbase_t*>::iterator i_s = l.begin();
//        for (; i_s != l.end(); ++i_s)
//        {
//            sessionbase_t* session = *i_s;
//            if (session->get_fd() == fd)
//            {
//                return session;
//            }
//        }
//    }
//    return 0;
}

void host::reaper_t::process_net_message(void* msg, int msglen)
{

}
//提取数据
bool host::reaper_t::get_queue_item(tagDataHead &data_head, void * buf, int size)
{
    scoped_lock_t autolocker(sync_queue);
    return workqueue.pop_item(data_head, buf, size);
}

void host::reaper_t::process_user_cmd(int cmd, long wparam, long lparam)
{
    switch (cmd)
    {
    case command_t::cmd_id_connect_time_out:
    {
        sessionbase_t* s = (sessionbase_t*)lparam;
        session_time_out(s);
    }
        break;
    case command_t::cmd_id_detach_session:
    {
        sessionbase_t* s = (sessionbase_t*)lparam;
        session_leave(s);
    }
        break;
    default:
        break;
    }
}

void host::reaper_t::session_leave  (sessionbase_t* s)
{
    i_linkinfo_t link_info;
    s->get_link_info(link_info);
    remove_session(s);
    send_term(s);
    if(_out_func)
    {
        //_out_func(sizeof(i_linkinfo_t),&link_info);
    }
}
void host::reaper_t::session_time_out(sessionbase_t* s)
{
    i_linkinfo_t link_info;
    s->get_link_info(link_info);
    if(_out_func)
    {
        _out_func(sizeof(i_linkinfo_t),&link_info);
    }
}

void host::reaper_t::add_timer (int timeout_, int id_)
{
    poller->add_timer (timeout_, this, id_);
}

void host::reaper_t::cancel_timer (int id_)
{
    poller->cancel_timer (this, id_);
}

void host::reaper_t::process_term_req(host::object_t* reqobj_)
{

}
