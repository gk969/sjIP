#include "sjip.h"
#include "mem.h"
#include "netif.h"
#include "eth.h"
#include "arp.h"
#include "ip.h"

// 收到ARP请求帧或ARP回复帧时获取ARP缓存项
// ARP缓存项超过其生存时间后 置为无效

arp_tag arp_list[ARP_LIST_LEN];


// ARP计时 每分钟调用一次
// 用于ARP缓存表生存时间 如时间结束 ARP项置为空闲
void arp_timer(void)
{
    u8 i;

    for(i = 0; i < ARP_LIST_LEN; i++)
    {
        if(arp_list[i].type == ARP_ACT)
        {
            if(arp_list[i].ttl != 0)
            {
                arp_list[i].ttl--;
            }
            else
            {
                arp_list[i].type = ARP_IDLE;
            }

            ARP_DBP("ARP %d %d.%d.%d.%d:", arp_list[i].ttl, arp_list[i].ip.inb[0], arp_list[i].ip.inb[1], arp_list[i].ip.inb[2], arp_list[i].ip.inb[3]);
            ARP_DBP("%02X:%02X:%02X:%02X:%02X:%02X\r\n", arp_list[i].mac[0], arp_list[i].mac[1], arp_list[i].mac[2], arp_list[i].mac[3], arp_list[i].mac[4], arp_list[i].mac[5]);
        }
    }
}


u8 *arp_iptomac(u32 ip_addr)
{
    u8 i;

    for(i = 0; i < ARP_LIST_LEN; i++)
    {
        if((arp_list[i].ip.inw == ip_addr) && (arp_list[i].type == ARP_ACT))
        {
            return arp_list[i].mac;
        }
    }

    return NULL;
}

// 接收处理ARP帧
void arp_accept(u8 *data, u32 plen)
{
    arp_frm *arp_recv = (arp_frm *)data;

    if(plen >= ARP_FRMLEN)
    {

        ARP_DBP("Recv %d.%d.%d.%d ARP ", arp_recv->src_ip.inb[0], arp_recv->src_ip.inb[1], arp_recv->src_ip.inb[2], arp_recv->src_ip.inb[3]);

        // 添加源IP、MAC至ARP缓存表
        arp_addlist(arp_recv->src_ip.inw, arp_recv->src_mac);

        if(htons(arp_recv->operation) == ARP_TYPE_REQ)
        {
            ARP_DBP("Request %d.%d.%d.%d\r\n", arp_recv->dst_ip.inb[0], arp_recv->dst_ip.inb[1], arp_recv->dst_ip.inb[2], arp_recv->dst_ip.inb[3]);


            if(arp_recv->dst_ip.inw == local.ip.inw)
            {
                ARP_DBP("Req Local IP. ARP Reply\r\n");
                arp_reply(arp_recv);
            }
        }
        else if(htons(arp_recv->operation) == ARP_TYPE_REP)
        {
            ARP_DBP("Reply\r\n");
        }
    }
}

// 添加ARP项
void arp_addlist(u32 ip_addr, u8 *mac_addr)
{
    u8 i;
    u8 minttl = ARP_TTL;
    u8 minttl_seq = 0;

    if((ip_addr == 0) || (mac_addr[0] & 0x01))
    {
        return;
    }

    // 检查是否已存在相同的ARP项
    // 如存在则生存时间初始化为最大值
    for(i = 0; i < ARP_LIST_LEN; i++)
    {
        if(arp_list[i].type == ARP_ACT)
        {
            // 只检测IP 检测到相同IP即更新其MAC
            if(arp_list[i].ip.inw == ip_addr)
            {
                mem_copy(arp_list[i].mac, mac_addr, MAC_ADDR_LEN);
                arp_list[i].ttl = ARP_TTL;
                return;
            }
        }
    }

    // 寻找空闲项 同时找出生存时间最小项
    for(i = 0; i < ARP_LIST_LEN; i++)
    {
        if(arp_list[i].type == ARP_IDLE)
        {
            break;
        }

        if(minttl > arp_list[i].ttl)
        {
            minttl = arp_list[i].ttl;
            minttl_seq = i;
        }
    }

    // 未找到空闲项 用生存时间最小项代之
    if(i == ARP_LIST_LEN)
    {
        i = minttl_seq;
    }

    arp_list[i].type = ARP_ACT;     // 声明该项已使用
    arp_list[i].ttl = ARP_TTL;          // 初始化该项生存时间
    arp_list[i].ip.inw = ip_addr;       // IP地址
    mem_copy(arp_list[i].mac, mac_addr, MAC_ADDR_LEN);  // MAC地址
}

// ARP回复
void arp_reply(arp_frm *arp_recv)
{
    arp_frm *arp_rep;
    eth_frm *eth_arp;

    eth_arp = netif_alloc(ETH_HDRLEN + ARP_FRMLEN);

    if(eth_arp != NULL)
    {
        arp_rep = (arp_frm *)(eth_arp->data);

        arp_rep->hardware_type = htons(ARP_HWTYPE_ETH);
        arp_rep->protocol_type = htons(ARP_PTTYPE_IP);
        arp_rep->hw_addr_len = MAC_ADDR_LEN;
        arp_rep->pt_addr_len = IP_ADDR_LEN;
        arp_rep->operation = htons(ARP_TYPE_REP);
        mem_copy(arp_rep->src_mac, local.mac, MAC_ADDR_LEN);
        arp_rep->src_ip.inw = local.ip.inw;
        mem_copy(arp_rep->dst_mac, arp_recv->src_mac, MAC_ADDR_LEN);
        arp_rep->dst_ip.inw = arp_recv->src_ip.inw;

        mem_copy(eth_arp->dst, arp_recv->src_mac, MAC_ADDR_LEN);
        mem_copy(eth_arp->src, local.mac, MAC_ADDR_LEN);
        eth_arp->type = htons(ETH_TYPE_ARP);

        netif_push(eth_arp, ETH_HDRLEN + ARP_FRMLEN, &local);
    }
}

// ARP请求
void arp_request(u32 req_ip)
{
    arp_frm *arp_rep;
    eth_frm *eth_arp;

    eth_arp = netif_alloc(ETH_HDRLEN + ARP_FRMLEN);
    

    if(eth_arp != NULL)
    {
        arp_rep = (arp_frm *)(eth_arp->data);

        arp_rep->hardware_type = htons(ARP_HWTYPE_ETH);
        arp_rep->protocol_type = htons(ARP_PTTYPE_IP);
        arp_rep->hw_addr_len = MAC_ADDR_LEN;
        arp_rep->pt_addr_len = IP_ADDR_LEN;
        arp_rep->operation = htons(ARP_TYPE_REQ);
        mem_copy(arp_rep->src_mac, local.mac, MAC_ADDR_LEN);
        arp_rep->src_ip.inw = local.ip.inw;
        mem_setbytes(arp_rep->dst_mac, 0x00, MAC_ADDR_LEN);
        arp_rep->dst_ip.inw = req_ip;

        mem_setbytes(eth_arp->dst, 0xFF, MAC_ADDR_LEN);
        mem_copy(eth_arp->src, local.mac, MAC_ADDR_LEN);
        eth_arp->type = htons(ETH_TYPE_ARP);

        netif_push(eth_arp, ETH_HDRLEN + ARP_FRMLEN, &local);
    }
}

