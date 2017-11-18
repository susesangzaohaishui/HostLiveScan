#include <QCoreApplication>
#include "amq_net.h"
#include "debug.h"
#include <stdio.h>

using namespace host;

void  recv_fun(int size, void* buf)
{
    if(size == sizeof(i_replay_info))
    {
        i_replay_info* ptr =  (i_replay_info*) buf;
        debug_str("call BACK %d bytes from %s: seq=%u, ttl=%d, rtt=%.3f ms id=%d\n",
                  ptr->m_recv_len,
                  ptr->m_remote_ip ,
                  ptr->m_icd_seq,
                  ptr->m_ttl,
                  ptr->m_rtt,
                  ptr->m_node_id);
    }
}
void  out_time_fun(int size, void* buf)
{

    if(size == sizeof(i_linkinfo_t))
    {
        i_linkinfo_t* ptr =  (i_linkinfo_t*) buf;
        debug_str("call BACK session logout LinkId = %d ,ip %s!!!!!!!!!!\n",
                  ptr->m_node_id,
                  ptr->m_node_ip);
    }
}
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    host_init("host_list.xml",recv_fun,out_time_fun);
    debug_fun();
    return a.exec();
}
