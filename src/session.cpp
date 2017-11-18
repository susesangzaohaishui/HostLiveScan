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

#include "session.hpp"
#include "msg_define.hpp"
#include "i_queue_service.hpp"
#include "command.hpp"
#include "clock.hpp"
#include "ctx.hpp"
#include "reaper.hpp"
#include "random.hpp"

#if !defined NET_HAVE_WINDOWS
#include "sock_op.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#endif
#include "debug.h"


inline void PACK(void* val, char valLen)
{
#ifdef _BIG
    char temp[10];
    char* pVal = (char*)val;
    for(int i=0;i<valLen;i++)
    {
        temp[i]=pVal[valLen-i-1];
    }
    memcpy(val,temp,valLen);
#endif
}

host::sessionbase_t::sessionbase_t(io_thread_t *io_thread_, i_queue_service* sink_ ):
    object_t((object_t*)io_thread_),
    io_object_t(io_thread_),
    i_queue_sink(sink_),
    s(retired_fd),
    io_error(false),
    recv_size(0),
    had_report(false)
{
    session_id = 0;
    timer_started = false;
}

//  Create a session of the particular type.
host::sessionbase_t::~sessionbase_t()
{
    if (s != retired_fd) {
        int rc = close (s);
        errno_assert (rc == 0);
        s = retired_fd;
    }
}

host::sessionbase_t *host::sessionbase_t::create (io_thread_t *io_thread_,i_queue_service* sink_,
                                                  fd_t fd_, session_type type)
{
    switch(type)
    {
    case SESSION_NODE:
        return new session_node(io_thread_, sink_);
    default:
        return 0;
    }
}

void host::sessionbase_t::set_link_info(const i_linkinfo_t& link)
{
    memcpy(&_link_info, &link, sizeof(i_linkinfo_t));
}

void host::sessionbase_t::get_link_info(i_linkinfo_t& link)
{
    memcpy(&link, &_link_info, sizeof(i_linkinfo_t));
}

void host::sessionbase_t::in_event ()
{
    int const rc = read (NULL, 0);
    if (rc == 0) {
        error ();
        return;
    }
    if (rc == -1) {
        if (errno != EAGAIN)
            error ();
        return;
    }
    on_recv(rc);
}

void host::sessionbase_t::on_recv(int)
{
    _last_check_tick = clock_t::now_us();
}

void host::sessionbase_t::timer_event (int id_)
{
    if(id_ == net_check_timer_id)
    {
        timer_started = false;
        uint64_t now = clock_t::now_us();
        if( abs (now - _last_check_tick) / 1000000 >= (_link_info.m_time_out))
        {
            if(!had_report)
            {
                debug_str("%s is time out %d\n",_link_info.m_node_ip,abs( now -_last_check_tick ) / 1000000);
                error();
            }
        }
        set_pollout(handle);
        int timer_val = (_link_info.m_scan_time <= 0 ? 1: _link_info.m_scan_time) * 1000;
        add_timer(timer_val, net_check_timer_id);
        timer_started = true;
    }
}

void host::sessionbase_t::out_event ()
{
    write(NULL,0);
    reset_pollout(handle);
}

void host::sessionbase_t::process_plug ()
{
    handle = add_fd (s);
    io_error = false;
    set_pollin (handle);
    _last_check_tick = clock_t::now_us();
    if(_link_info.m_scan_time)
    {
        debug_str("start Scan Timer LinId = %d &&&&&&&&&&&&&&&&&&\n", _link_info.m_node_id);
        int timer_val = (_link_info.m_scan_time <= 0 ? 1: _link_info.m_scan_time) * 1000;
        add_timer(timer_val, net_check_timer_id);
        timer_started = true;
    }
}


void host::sessionbase_t::process_term()
{
    if (timer_started) {
        cancel_timer (net_check_timer_id);
        timer_started = false;
    }
    close(s);
    s = retired_fd;
    io_object_t::unplug ();
    delete this;
}


void host::sessionbase_t::error ()
{
    io_error = true;
    had_report = true;
    ctx_t* ctx = get_ctx();
    command_t cmd;
    cmd.type = command_t::user_cmd;
    cmd.destination = ctx->get_reaper();
    cmd.args.user_cmd.cmdId = command_t::cmd_id_connect_time_out;
    cmd.args.user_cmd.lparam = (long)this;
    send_command (cmd);
}

//????????
host::send_overlapped * host::sessionbase_t::get_send_overlapped()
{
    //?????н?
    send_overlapped * send_item = NULL;
    if(!_free_send_list.empty())
    {
        send_item = _free_send_list.front();
        _free_send_list.pop_front();
        _active_send_list.push_back(send_item);
        return send_item;
    }

    //????????
    try
    {
        send_item = new send_overlapped;
        _active_send_list.push_back(send_item);
        return send_item;
    }
    catch (...) { }
    return 0;
}

void host::sessionbase_t::asyn_send_buf(const void *data_, size_t size_)
{
    scoped_lock_t autolocker(syn_send);

    //???????
    send_overlapped * send_item = get_send_overlapped();
    net_assert(send_item != 0);
    if (send_item == 0) return;
    send_item->send_pos = 0;
    send_item->send_len = size_;
    memcpy(send_item->send_buf, data_, size_);
    if (_active_send_list.size() == 1)
    {
        command_t cmd;
        cmd.type = command_t::user_cmd;
        cmd.destination = this;
        cmd.args.user_cmd.cmdId = command_t::cmd_id_active_send;
        send_command (cmd);
    }
}

void host::sessionbase_t::process_user_cmd(int cmd, long wparam, long lparam)
{
    switch (cmd)
    {
    case command_t::cmd_id_active_send:
    {
        if(io_error) return;
        set_pollout(handle);
    }
        break;
    }
}

//--------------------------------------------------------------------------
host::session_node::session_node(io_thread_t *io_thread_, i_queue_service* sink_)
    :sessionbase_t(io_thread_, sink_)
{

}

//  Create a session of the particular type.
host::session_node::~session_node()
{

}

int host::session_node::write (const void *data_, size_t size_)
{
    int			len;
    struct icmp	*icmp;
    icmp = (struct icmp *) sendbuf;
    icmp->icmp_type = ICMP_ECHO;
    icmp->icmp_code = 0;
    icmp->icmp_id = ic_id; //需要用进程ID
    icmp->icmp_seq = nsent++;
    memset(icmp->icmp_data, 0xa5, datalen);	/* fill with pattern */
    gettimeofday((struct timeval *) icmp->icmp_data, NULL);
    len = 8 + datalen;		/* checksum ICMP header and data */
    icmp->icmp_cksum = 0;
    icmp->icmp_cksum = in_cksum((u_short *) icmp, len);
    ssize_t nbytes = Sendto(s, sendbuf, len, 0, sock_send, sock_len);

    //  Several errors are OK. When speculative write is being done we may not
    //  be able to write a single byte from the socket. Also, SIGSTOP issued
    //  by a debugging tool can result in EINTR error.
    if (nbytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK ||
                         errno == EINTR))
        return 0;

    //  Signalise peer failure.
    if (nbytes == -1) {
        errno_assert (errno != EACCES
                && errno != EBADF
                && errno != EDESTADDRREQ
                && errno != EFAULT
                && errno != EINVAL
                && errno != EISCONN
                && errno != EMSGSIZE
                && errno != ENOMEM
                && errno != ENOTSOCK
                && errno != EOPNOTSUPP);
        return -1;
    }

    return static_cast <int> (nbytes);
}

int host::session_node::read (void *data_/*not used*/, size_t size_)
{
    (void)data_;
    msg.msg_namelen = sock_len;
    msg.msg_controllen = sizeof(controlbuf);
    const ssize_t rc  = recvmsg(s, &msg, 0);
    if (rc < 0) {
        errno_assert (errno != EBADF
                && errno != EFAULT
                && errno != EINVAL
                && errno != ENOMEM
                && errno != ENOTSOCK);
        if (errno == EINTR)
            errno = EAGAIN;
        else
            debug_err("recvmsg error");
    }
    return static_cast <int> (rc);
}

void host::session_node::on_recv(int n)
{
    try
    {
        int				hlen1, icmplen;
        double			rtt;
        struct ip		*ip;
        struct icmp		*icmp;
        struct timeval	*tvsend;
        struct timeval	tval;
        gettimeofday(&tval, NULL);

        ip = (struct ip *) recvbuf;		/* ip报文头*/
        hlen1 = ip->ip_hl << 2;		/* ip报文头长度*/
        if (ip->ip_p != IPPROTO_ICMP)
            return;				/*ip报文头如果不是ICMP协议 */

        icmp = (struct icmp *) (recvbuf + hlen1);	/*ICMP报文头*/
        if ( (icmplen = n - hlen1) < 8)
            return;				/* ICMP至少八个字节 */

        char *str_remote = sock_ntop_host(sock_recv, sock_len);
        if(!str_remote)
        {
            return;
        }
        if (icmp->icmp_type == ICMP_ECHOREPLY && !strcmp(str_remote,_link_info.m_node_ip)) {
            if (icmp->icmp_id != ic_id)
                return;			/* 通过ICMP ID来确定是否是我们发的数据报 */
            if (icmplen < 16)
                return;			/*数据长度判断*/
            had_report = false;
            _last_check_tick = clock_t::now_us();
            tvsend = (struct timeval *) icmp->icmp_data;
            tv_sub(&tval, tvsend);
            rtt = tval.tv_sec * 1000.0 + tval.tv_usec / 1000.0;

            reaper_t* r = static_cast<reaper_t*>(get_ctx()->get_reaper());
            PFN_RECV_REPALY func = r->get_replay_fun();
            if (0 != func)
            {
                i_replay_info replay_info;
                replay_info.m_icd_seq = icmp->icmp_seq;
                replay_info.m_recv_len = icmplen;
                replay_info.m_rtt = rtt;
                replay_info.m_ttl =  ip->ip_ttl;
                replay_info.m_node_id = _link_info.m_node_id;
                strcpy(replay_info.m_remote_ip, str_remote);
                func(sizeof(i_replay_info), &replay_info);
            }

        } /*else if ( !strcmp(str_remote,_link_info.m_node_ip)) {
            printf("  %d bytes from %s: type = %d, code = %d\n",
                   icmplen, sock_ntop_host(sock_recv, sock_len),
                   icmp->icmp_type, icmp->icmp_code);
        }*/
    }

    catch (...)
    {
        error();
        return;
    }
}

void host::session_node::exit()
{
    ctx_t* ctx = get_ctx();
    command_t cmd;
    cmd.type = command_t::user_cmd;
    cmd.destination = ctx->get_reaper();
    cmd.args.user_cmd.cmdId = command_t::cmd_id_detach_session;
    cmd.args.user_cmd.lparam = (long)this;
    send_command (cmd);
}

void host::session_node::process_plug ()
{
    struct addrinfo	*ai;
    char *h;
    char* host;//host name
    host = _link_info.m_node_ip;
    int				size;
    ai = host_serv(host, NULL, 0, 0);

    if(NULL == ai)
    {
        exit();
    }
    datalen  = sizeof(struct rec);	/* defaults */
    h = sock_ntop_host(ai->ai_addr, ai->ai_addrlen);
    debug_str("PING %s (%s): %d data bytes\n",
           ai->ai_canonname ? ai->ai_canonname : h,
           h, datalen);

    if(NULL == h)
    {
        exit();
    }
    sock_send = ai->ai_addr;
    sock_recv = (sockaddr*)calloc(1, ai->ai_addrlen);
    sock_len = ai->ai_addrlen;
    ic_id =  getpid();
    nsent = 0;

    icmpproto = IPPROTO_ICMP;

    s = socket(sock_send->sa_family, SOCK_RAW, icmpproto);

    if (s == -1)
    {
        io_error = true;
        exit();
        return;
    }
    size = 60 * 1024;		/* OK if setsockopt fails */
    setsockopt(s, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));

    iov.iov_base = recvbuf;
    iov.iov_len = sizeof(recvbuf);
    msg.msg_name = sock_recv;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = controlbuf;

    handle = add_fd (s);
    io_error = false;
    set_pollin (handle);

    _last_check_tick = clock_t::now_us();

    debug_str("start Scan Host, LinId = %d &&&&&&&&&&&&&&&&&&\n",
              _link_info.m_node_id);
    int timer_val = (_link_info.m_scan_time <= 0 ? 1: _link_info.m_scan_time) * 1000;
    add_timer(timer_val, net_check_timer_id);
    timer_started = true;

}
