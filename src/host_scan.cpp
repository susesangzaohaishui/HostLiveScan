#include "host_scan.h"
#include "ctx.hpp"
#include "reaper.hpp"
#include "msg_define.hpp"
#define MAX_NET_PACK_LEN (8192 + 128)
static host::ctx_t*  g_net_contex;

static bool start()
{
    return 0 != g_net_contex;
}

static host::reaper_t* get_reaper()
{
    return static_cast<host::reaper_t*>(g_net_contex->get_reaper());
}

extern "C"
{

bool   host_init( const   char* xml_path , PFN_RECV_REPALY recvFun,
                  PFN_NET_OUT_TIME outTimeFun)
{
#ifdef NET_HAVE_WINDOWS
    //  Intialise Windows sockets. Note that WSAStartup can be called multiple
    //  times given that WSACleanup will be called for each WSAStartup.
    //  We do this before the ctx constructor since its embedded mailbox_t
    //  object needs Winsock to be up and running.
    WORD version_requested = MAKEWORD (2, 2);
    WSADATA wsa_data;
    int rc = WSAStartup (version_requested, &wsa_data);
    net_assert (rc == 0);
    net_assert (LOBYTE (wsa_data.wVersion) == 2 &&
        HIBYTE (wsa_data.wVersion) == 2);
#endif
    g_net_contex = new host::ctx_t;
    g_net_contex->start();
    if(!get_reaper()->create_link(xml_path))
    {
        return false;
    }
    get_reaper()->set_logout_fun(outTimeFun);
    get_reaper()->set_replay_fun(recvFun);
    get_reaper()->open_all_link();
    return true;
}

void   host_asyn_send( int nLinkId,
    int    sockfd,
    int    nLength,
    char*  pBuffer)
{
    if(!start())
    {
        return;
    }
    if (nLength <= 0 || nLength > MAX_NET_PACK_LEN)
    {
        return;
    }

        char queue_buf[MAX_NET_PACK_LEN + sizeof(host::queue_msg_header)];
    host::queue_msg_header msg_header;
    msg_header.msg_send.linkid = nLinkId;
    msg_header.msg_send.fd    = sockfd;
    msg_header.msg_send.buflen = nLength + sizeof(host::queue_msg_header);
    memcpy(queue_buf, &msg_header, sizeof(msg_header));
    memcpy(queue_buf + sizeof(msg_header), pBuffer, nLength);

    int queue_msg_len = nLength + sizeof(host::queue_msg_header);

    get_reaper()->add_queue_item(host::queue_msg_send_data,
        queue_buf,
        queue_msg_len);
}

void   host_sutdown()
{

}

}
