#include "sjip.h"
#include "ip.h"
#include "udp.h"
#include "mem.h"
#include "arp.h"

udp_socket udp_scklist[UDP_SCKNUM];

void udp_accept(const ip_frm *ip_recv)
{
    udp_frm     *udp_recv = (udp_frm *)(ip_recv->data);
    udp_socket  *udp_echo;
    
    u16 i;
    u16 port;
    u16 data_len;

    UDP_DBP("src_ip:%x src_port:%d\r\n", ip_recv->src.inw, htons(udp_recv->src_port));

    UDP_DBP("dst_ip:%x dst_port:%d\r\n", ip_recv->dst.inw, htons(udp_recv->dst_port));
    
    UDP_DBP("ChksumWithFackHdr %d\r\n", ChksumWithFackHdr(ip_recv, IP_UDP));

    UDP_DBP("len:%d\r\n", htons(udp_recv->frmlen));

    USART1_SendArray(udp_recv->data, htons(udp_recv->frmlen)-UDP_HDRLEN);
    UDP_DBP("\r\n");

    LED_NOT(LED_PORT, LED_UDP);
    
    
    if((htons(udp_recv->frmlen)!=(htons(ip_recv->frmlen)-IP_HDRLEN))||(htons(udp_recv->frmlen)<=UDP_HDRLEN))
    {
        return;
    }
    if(ChksumWithFackHdr(ip_recv, IP_UDP)!=0)
    {
        return;
    }
    
    udp_echo=new_udpsck();
    
    if(udp_echo!=NULL)
    {
        UDP_DBP("udp_echo\r\n");
        udp_echo->local_port=12345;
        udp_echo->remote_ip.inw=ip_recv->src.inw;
        udp_echo->remote_port=htons(udp_recv->dst_port);
        
        udp_send(udp_echo, udp_recv->data, htons(udp_recv->frmlen)-UDP_HDRLEN);
        
        udp_echo->remote_ip.inw=0xFFFFFFFF;
        udp_echo->remote_port=htons(udp_recv->dst_port);
        udp_send(udp_echo, udp_recv->data, htons(udp_recv->frmlen)-UDP_HDRLEN);
        
        arp_request(0x6400a8c0);
        arp_request(0x100a8c0);
        
        close_udpsck(udp_echo);
    }
    
    data_len=htons(udp_recv->frmlen)-UDP_HDRLEN;
    port=htons(udp_recv->dst_port);
    for(i=0;i<UDP_SCKNUM;i++)
    {
        if(udp_scklist[i].local_port==port)
        {
            udp_scklist[i].recv(udp_recv->data, data_len, &udp_scklist[i]);
            return;
        }
    }
    
}

void udp_send(udp_socket *sck, void *data, u16 len)
{
    ip_frm *ip;
    udp_frm *udp;

    if(len > UDP_DAT_MAX_LEN)
    {
        return;
    }

    ip = ip_malloc(len + UDP_HDRLEN + IP_HDRLEN);

    if(ip == NULL)
    {
        return;
    }
    
    UDP_DBP("udp_send\r\n");

    // 填充IP首部
    ip->vsn = IPV4_HDR20B;
    ip->tos = 0x00;
    ip->frmlen = htons(len + UDP_HDRLEN + IP_HDRLEN);
    ip->id = htons(get_ipid());
    ip->ofs = 0;
    ip->ttl = UDP_TTL;
    ip->pt = IP_UDP;
    ip->chksum = 0;
    ip->src.inw = local.ip.inw;
    ip->dst.inw = sck->remote_ip.inw;
    ip->chksum = htons(chksum_short((u16 *)ip, IP_HDRLEN));

    udp = (udp_frm *)(ip->data);
    // 填充UDP首部
    udp->src_port = htons(sck->local_port);
    udp->dst_port = htons(sck->remote_port);
    udp->frmlen = htons(len + UDP_HDRLEN);
    udp->chksum = 0;

    // 填充UDP数据
    mem_copy(udp->data, data, len);
    
    udp->chksum = htons(ChksumWithFackHdr(ip, IP_UDP));
    ip_push(ip);
}

udp_socket *new_udpsck(void)
{
    u16 i;
    
    for(i=0; i<UDP_SCKNUM; i++)
    {
        if(udp_scklist[i].remote_port==0)
        {
            UDP_DBP("udp_scklist[%d]\r\n", i);
            return &udp_scklist[i];
        }
    }
    
    return NULL;
}

void close_udpsck(udp_socket *sck)
{
    sck->remote_port=0;
    sck->local_port=0;
}
