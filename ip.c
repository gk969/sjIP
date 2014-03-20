#include "sjip.h"
#include "netif.h"
#include "eth.h"
#include "ip.h"
#include "icmp.h"
#include "arp.h"
#include "udp.h"

// IP标识 实质为一个16位计数器 
u16 ip_id;

// 计算校验和
u16 chksum_short(const u16 *data, u16 len)
{
	u32 sum;
	
	len/=2;
	for(sum = 0; len > 0; len--)
	{
		sum += htons(*data);	// 累加
		data++;
	}

	sum = (sum >> 16) + (sum & 0xFFFF); // 反码相加
	sum += (sum >> 16);

	return ~sum;
}


u16 ChksumWithFackHdr(const ip_frm * ip, u16 type)
{
    fake_ipHdr      fack_hdr;
    u16             data_size;
    u32             sum;
    u16             *data;
    u16             i;
	
    data_size=htons(ip->frmlen)-IP_HDRLEN;
    fack_hdr.src_ip.inw=ip->src.inw;
    fack_hdr.dst_ip.inw=ip->dst.inw;
    fack_hdr.pad=htons(type);
    fack_hdr.frmlen=htons(data_size);
    
    
    data=(u16*)(&fack_hdr);
	i=sizeof(fack_hdr)/2;
	for(sum = 0; i > 0; i--)
	{
		sum += htons(*data);	// 累加
		data++;
	}
    
    data=(u16*)ip->data;
	i=data_size/2;
	for(; i > 0; i--)
	{
		sum += htons(*data);	// 累加
		data++;
	}
    if((data_size&0x01)!=0)
    {
        sum+=htons(*(u8*)data);
    }

	sum = (sum >> 16) + (sum & 0xFFFF); // 反码相加
	sum += (sum >> 16);

	return ~sum;
}

void ip_accept(const u8 *data, u16 plen)
{
	ip_frm *ip_recv = (ip_frm *)data;
	u16 frm_len;
	
	frm_len = htons(ip_recv->frmlen);
	// 长度检查
	if((plen >= frm_len) && (frm_len > IP_HDRLEN))
	{
		IP_DBP("IP Package received %d\r\n", frm_len);

		// IP版本号 首部长度 IPv4 首部长度20字节
		if(ip_recv->vsn==IPV4_HDR20B)
		{
			IP_DBP("IP chksum ");
			if(chksum_short((u16 *)ip_recv, IP_HDRLEN)==0)
			{
				IP_DBP("Right\r\n");
				
				//IP_DBP("ip_recv->ofs %x\r\n", ip_recv->ofs);
				if((ip_recv->ofs & IP_MF) == 0)	// 分段结束
				{
					//IP_DBP("local IP %x\r\nrecv IP %x\r\n", local.ip.inw, ip_recv->src.inw);
					if(ip_recv->dst.inw == local.ip.inw)
					{
						IP_DBP("local recv IP\r\n");
						
						switch(ip_recv->pt)
						{
							case IP_ICMP:
                                IP_DBP("ICMP\r\n");
								icmp_accept(ip_recv);
							break;
							
							case IP_IGMP:
								
							break;
							
							case IP_UDP:
                                IP_DBP("UDP\r\n");
                                udp_accept(ip_recv);
							break;
							
							case IP_TCP:
								IP_DBP("TCP\r\n");
							break;
						}
					}
                    
                    
                    LED_NOT(LED_PORT, LED_IP);
                    
				}
				else	 // 还有分段 
				{
					
				}
				
			}
			else
			{
				IP_DBP("Error\r\n");
			}
			
		}
	}
}

// ip层内存分配 size包含ip报头
void *ip_malloc(u16 size)
{
    void* ip_buf;
    u16 ethfrm_len;
    
    ethfrm_len=size+ETH_HDRLEN;
    if(ethfrm_len<=NETIF_DAT_MAX_LEN)
	{
        ip_buf=netif_alloc(ethfrm_len);
        
        if(ip_buf==NULL)
        {
            return NULL;
        }
        
        return (void *)((u32)ip_buf+ETH_HDRLEN);
    }
    
    return NULL;
}



#define ip_free(ip) mem_free((void*)(((u32)ip)-ETH_HDRLEN-sizeof(netbuf_hdr)))
    


// 根据上层协议下发的完整IP数据包，填充MAC数据报头，
// 然后压入网络接口发送缓存
void ip_push(ip_frm *ip)
{
    eth_frm *eth;
    u8 *dstmac;
    u32 dstip;
    
    if(((ip->dst.inb[0]&0xF0)==0xE0)    // D类IP 多播地址
     ||((ip->dst.inb[0]&0xF8)==0xF0)    // E类IP 未使用
     ||(ip->dst.inw==0))                // 目标地址为0 
    {
        IP_DBP("IP Addr Error!\r\n");
        ip_free(ip);
        return;
    }
    
    eth=(eth_frm*)((u32)ip-ETH_HDRLEN);
    if(ip->dst.inw==0xFFFFFFFF)         // 广播地址
    {
        mem_setbytes(eth->dst, 0xFF, MAC_ADDR_LEN);
    }
    else
    {
        if((ip->dst.inw&local.mask.inw)!=(local.ip.inw&local.mask.inw)) // 不在同一子网 发往路由器 由之转发
        {
            dstip=local.gateway.inw;
        }
        else    // 同一子网，无需发往路由器转发
        {
            dstip=ip->dst.inw;
        }
        
        dstmac=arp_iptomac(dstip);
        if(dstmac!=NULL)
        {
            // 填充Eth帧头
            mem_copy(eth->dst, dstmac, MAC_ADDR_LEN);
        }
        else// ARP缓存中没有此IP 销毁当前数据包 并发出一个此IP的ARP请求 
        {
            ip_free(ip);
            arp_request(dstip);
            return;
        }
    }
    mem_copy(eth->src, local.mac, MAC_ADDR_LEN);
    eth->type=htons(ETH_TYPE_IP);
    netif_push(eth, ETH_HDRLEN + htons(ip->frmlen), &local);
}

