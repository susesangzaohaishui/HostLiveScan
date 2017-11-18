#ifndef __NET_MSG_HPP_INCLUDED__
#define __NET_MSG_HPP_INCLUDED__
namespace host
{
    //������Ϣ
    enum
    {
        queue_msg_net_data,     //������Ϣ
         queue_msg_send_data      //������������
    };

    struct queue_msg_header
    {
        union
        {
            struct {
                int    linkid;
                int    fd;
                int    buflen;
            }msg_send;
        };
    };

    struct net_msg_header
    {
        unsigned short  start_code   ;   //  ��ʼ����
        unsigned short  msg_len  ;      //  ���ݳ���
        unsigned short  src_addr  ;     //  Դ��ַ
        unsigned short  dst_addr  ;     //  Ŀ�ĵ�ַ
        unsigned short  cmd_order ;     //  �����
        unsigned short  cmd_code   ;    //  ������
        unsigned short  version     ;   //  �汾��
        unsigned char   attribute   ;   //  ����
        unsigned char   msg_seq ;       //  �������
        unsigned short  frame_number  ;  //  �ܰ���
        unsigned short  frame_seq;      //  ��ǰ����
        net_msg_header()
        {
            start_code = 0x7e7e;
        }
    };

    struct net_msg_end
    {
        net_msg_end()
        {
            crc = 0;
            end = 0x0A0D;
        }
        unsigned short crc;
        unsigned short end;
    };
}
#endif
