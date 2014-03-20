#ifndef __ARP_H
#define __ARP_H

#include "netif.h"

// ARP缓存表长度
#define ARP_LIST_LEN    8
// ARP缓存项 生存时间 最长10分钟
#define ARP_TTL         10

#define ARP_IDLE    0
#define ARP_ACT    1

typedef struct
{
    u8      type;   // busy or idle
    u8      ttl;    // time to live (minute)
    ip_addr ip;
    u8      mac[6];
} arp_tag;

#pragma pack(1)

typedef struct
{
    u16     hardware_type;
    u16     protocol_type;
    u8      hw_addr_len;
    u8      pt_addr_len;
    u16     operation;
    u8      src_mac[6];
    ip_addr src_ip;
    u8      dst_mac[6];
    ip_addr dst_ip;
} arp_frm;

#pragma pack()

#define ARP_FRMLEN      sizeof(arp_frm)

#define ARP_TYPE_REQ    0x0001
#define ARP_TYPE_REP    0x0002

#define ARP_HWTYPE_ETH  0x0001
#define ARP_PTTYPE_IP   0x0800

void arp_timer(void);
u8 *arp_iptomac(u32 ip_addr);
void arp_accept(u8 *data, u32 plen);
void arp_reply(arp_frm *recv_arp);
void arp_addlist(u32 ip_addr, u8 *mac_addr);
void arp_request(u32 req_ip);

#endif
