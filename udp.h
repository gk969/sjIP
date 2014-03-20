#ifndef _UDP_H
#define _UDP_H

#include "ip.h"


#define UDP_SCKNUM  32
#define UDP_HDRLEN  8
#define UDP_DAT_MAX_LEN (IP_DAT_MAX_LEN-UDP_HDRLEN)

#pragma pack(1)

typedef struct
{
    u16 src_port;   // 源端口                         
    u16 dst_port;   // 目的端口
    u16 frmlen;     // 长度 包括UDP首部
    u16 chksum;     // 校验和 包括数据、UDP首部、伪IP首部
    u8  data[UDP_DAT_MAX_LEN];
}udp_frm;

#pragma pack()



#define UDP_TTL 128

typedef struct sck_udp
{
    u16     local_port;
    ip_addr remote_ip;
    u16     remote_port;
    
    void    (*recv)(u8* data, u16 len, struct sck_udp *sck);
}udp_socket;


void udp_accept(const ip_frm *ip_recv);

void udp_send(udp_socket *sck, void *data, u16 len);
udp_socket *new_udpsck(void);
void close_udpsck(udp_socket *sck);

#endif
