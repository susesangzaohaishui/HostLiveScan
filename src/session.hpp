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

#ifndef __NET_SESSION_BASE_HPP_INCLUDED__
#define __NET_SESSION_BASE_HPP_INCLUDED__

#include <string>
#include <stdarg.h>
#include <strings.h>
#include <stdio.h>
#include <list>
#include "object.hpp"
#include "mutex.hpp"
#include "io_object.hpp"
#include "host_scan.h"


namespace host
{
class  io_thread_t;
struct  i_queue_service;
struct address_t;

//发送结构
class send_overlapped
{
    //数据变量
public:
    int         send_pos;
    int         send_len;
    char		send_buf[NET_SOCKET_BUFFER];			//数据缓冲
    //函数定义
public:
    send_overlapped()
    {
        send_pos = 0;
        send_len = 0;
    }
    ~send_overlapped(){}
};

enum session_type
{
    SESSION_NODE
};
class sessionbase_t :
        public object_t,
        public io_object_t
{
protected:
    sessionbase_t(host::io_thread_t *io_thread_, i_queue_service* sink_);
    //  Create a session of the particular type.
    ~sessionbase_t();

    enum {net_check_timer_id = 100,
         net_timeout_timer_id};

public:
    static host::sessionbase_t *create (host::io_thread_t *io_thread_,
                                        i_queue_service* sink_,
                                        fd_t fd_, session_type type);
public:
    void set_session_id(int id_){_sessionid = id_;}
    int  get_session_id(){return _sessionid;}
    void set_link_info(const i_linkinfo_t&);
    void get_link_info(i_linkinfo_t&);
    int  get_fd(){return s;}
    void asyn_send_buf(const void *data_, size_t size_);

public:
    void in_event ();
    void out_event ();
    void timer_event (int id_);
protected:
    virtual void process_plug ();

    virtual void process_term();

    virtual void process_user_cmd(int cmd, long wparam, long lparam);

protected:

    virtual int write (const void *data_, size_t size_){return 0;}

    virtual int read (void *data_, size_t size_){return 0;}

    virtual void on_recv(int);

    void error ();

    host::send_overlapped * get_send_overlapped();
protected:
    i_queue_service*  i_queue_sink;
    fd_t     s;

    //XXXXXXXXXXXX
    struct sockaddr* sock_send;
    struct sockaddr* sock_recv;
    int      sock_len;
    int      icmpproto;
    int		 datalen ;
    int      ic_id;
    long long int nsent;
    char	 sendbuf[BUFSIZE];
    char	 recvbuf[BUFSIZE];
    char	 controlbuf[BUFSIZE];
    struct iovec	iov;
    struct msghdr	msg;
    bool   had_report;
    //XXXXXXXXXXXX

    handle_t handle;
    int      session_id;
    bool     io_error;
    unsigned int  	 recv_size;						//接收长度
    mutex_t  syn_send;
    std::list<send_overlapped*>  _free_send_list;   //空闲链表
    std::list<send_overlapped*>  _active_send_list; //发送链表
    int     _sessionid;
    i_linkinfo_t   _link_info;
    uint64_t _last_check_tick;
    bool timer_started;
private:
    sessionbase_t (const sessionbase_t&);
    const sessionbase_t &operator = (const sessionbase_t&);
};

//test host is live
class session_node : public sessionbase_t
{
public:
    session_node(host::io_thread_t *io_thread_, i_queue_service* sink_);
    //  Create a session of the particular type.
    ~session_node();

protected:
    virtual int write (const void *data_, size_t size_);

    virtual int read (void *data_, size_t size_);

    virtual void on_recv(int n);

    void exit();
private:
    virtual void process_plug ();
private:
    session_node (const session_node&);
    const session_node &operator = (const session_node&);
};

}

#endif
