#ifndef AMQ_NET_H
#define AMQ_NET_H

#if defined _WIN32
#   if defined NET_STATIC
#       define NET_EXPORT
#   elif defined DLL_EXPORT
#       define NET_EXPORT __declspec(dllexport)
#   else
#       define NET_EXPORT __declspec(dllimport)
#   endif
#else
#   if defined __SUNPRO_C  || defined __SUNPRO_CC
#       define NET_EXPORT __global
#   elif (defined __GNUC__ && __GNUC__ >= 4) || defined __INTEL_COMPILER
#       define NET_EXPORT __attribute__ ((visibility("default")))
#   else
#       define NET_EXPORT
#   endif
#endif

#include <string.h>
namespace host
{
    struct i_replay_info
    {
        char      m_remote_ip[120];
        int         m_ttl;
        double   m_rtt;
        int         m_icd_seq;
        int         m_recv_len;
        int         m_node_id;
        i_replay_info()
        {
            memset(this, 0, sizeof(i_replay_info));
        }
    };
    struct i_linkinfo_t
    {
        char              m_node_ip[120];
        int              m_node_id;
        int              m_scan_time;
        int              m_time_out;
        int              m_node_type;
        i_linkinfo_t()
        {
            memset(this, 0, sizeof(i_linkinfo_t));
        }
    };
}

//接收到回射
typedef void (*PFN_RECV_REPALY)(int  dwBufSize, void* pBuffer );

//超时
typedef void (*PFN_NET_OUT_TIME)(int  dwBufSize,
                                 void*      pBuffer
                                 );

extern "C"
{
     bool  NET_EXPORT host_init(
         const   char* xml_path,
             PFN_RECV_REPALY recvFun,
             PFN_NET_OUT_TIME outTimeFun
             );

     void  NET_EXPORT host_asyn_send( int nLinkId,
         int    sockfd,
         int    nLength,
         char*  pBuffer);

     void  NET_EXPORT host_sutdown();
}

#endif // AMQ_NET_H
