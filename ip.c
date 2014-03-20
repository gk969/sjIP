#include "sjip.h"
#include "netif.h"
#include "eth.h"
#include "ip.h"
#include "icmp.h"
#include "arp.h"
#include "udp.h"

// IP��ʶ ʵ��Ϊһ��16λ������ 
u16 ip_id;

// ����У���
u16 chksum_short(const u16 *data, u16 len)
{
	u32 sum;
	
	len/=2;
	for(sum = 0; len > 0; len--)
	{
		sum += htons(*data);	// �ۼ�
		data++;
	}

	sum = (sum >> 16) + (sum & 0xFFFF); // �������
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
		sum += htons(*data);	// �ۼ�
		data++;
	}
    
    data=(u16*)ip->data;
	i=data_size/2;
	for(; i > 0; i--)
	{
		sum += htons(*data);	// �ۼ�
		data++;
	}
    if((data_size&0x01)!=0)
    {
        sum+=htons(*(u8*)data);
    }

	sum = (sum >> 16) + (sum & 0xFFFF); // �������
	sum += (sum >> 16);

	return ~sum;
}

void ip_accept(const u8 *data, u16 plen)
{
	ip_frm *ip_recv = (ip_frm *)data;
	u16 frm_len;
	
	frm_len = htons(ip_recv->frmlen);
	// ���ȼ��
	if((plen >= frm_len) && (frm_len > IP_HDRLEN))
	{
		IP_DBP("IP Package received %d\r\n", frm_len);

		// IP�汾�� �ײ����� IPv4 �ײ�����20�ֽ�
		if(ip_recv->vsn==IPV4_HDR20B)
		{
			IP_DBP("IP chksum ");
			if(chksum_short((u16 *)ip_recv, IP_HDRLEN)==0)
			{
				IP_DBP("Right\r\n");
				
				//IP_DBP("ip_recv->ofs %x\r\n", ip_recv->ofs);
				if((ip_recv->ofs & IP_MF) == 0)	// �ֶν���
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
				else	 // ���зֶ� 
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

// ip���ڴ���� size����ip��ͷ
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
    


// �����ϲ�Э���·�������IP���ݰ������MAC���ݱ�ͷ��
// Ȼ��ѹ������ӿڷ��ͻ���
void ip_push(ip_frm *ip)
{
    eth_frm *eth;
    u8 *dstmac;
    u32 dstip;
    
    if(((ip->dst.inb[0]&0xF0)==0xE0)    // D��IP �ಥ��ַ
     ||((ip->dst.inb[0]&0xF8)==0xF0)    // E��IP δʹ��
     ||(ip->dst.inw==0))                // Ŀ���ַΪ0 
    {
        IP_DBP("IP Addr Error!\r\n");
        ip_free(ip);
        return;
    }
    
    eth=(eth_frm*)((u32)ip-ETH_HDRLEN);
    if(ip->dst.inw==0xFFFFFFFF)         // �㲥��ַ
    {
        mem_setbytes(eth->dst, 0xFF, MAC_ADDR_LEN);
    }
    else
    {
        if((ip->dst.inw&local.mask.inw)!=(local.ip.inw&local.mask.inw)) // ����ͬһ���� ����·���� ��֮ת��
        {
            dstip=local.gateway.inw;
        }
        else    // ͬһ���������跢��·����ת��
        {
            dstip=ip->dst.inw;
        }
        
        dstmac=arp_iptomac(dstip);
        if(dstmac!=NULL)
        {
            // ���Eth֡ͷ
            mem_copy(eth->dst, dstmac, MAC_ADDR_LEN);
        }
        else// ARP������û�д�IP ���ٵ�ǰ���ݰ� ������һ����IP��ARP���� 
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

