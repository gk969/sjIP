#include "sjip.h"
#include "enc28j60.h"
#include "netif.h"
#include "eth.h"
#include "mem.h"

// 协议发送数据流程
// 调用下层的内存分配宏 获取足够容纳本层协议帧和下层协议头的内存空间
// 填充本层协议帧 根据本层协议需要填充下层协议头
// 调用下层协议入栈函数 数据调入发送缓存链表(栈 后进先出)
// 轮询任务检查接口发送缓存 如有数据 全部发送



netif local =
{
	{0x00, 0x02, 0x99, 0x79, 0x24, 0x58},	// MAC
	{192, 168, 0, 10},						// IP
	{255, 255, 255, 0},						// Mask
	{192, 168, 0, 1},						// Gateway

	LOST,                                   // 链路状态

	NULL,                                   // 接收缓存区
	NULL,                                   // 发送缓存区

	enc28j60PacketSend                      // 发送函数
};


// 由于网络接口硬件已带有缓存 故而软件无需缓存 接受一个处理一个即可
void netif_interrupt(void)
{
	u16 plen;
	u8 eir;
	u16 phy;

	netbuf_tag *recv_buf;

	enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, EIE, EIE_INTIE);

	eir = enc28j60Read(EIR);

	if(eir & EIR_PKTIF)		// 接收数据包待处理
	{

		plen = enc28j60PacketReceiveLen(NETIF_DAT_MAX_LEN);
		if((plen > 0)&&(plen<NETIF_DAT_MAX_LEN))
		{
			NETIF_DBP("\r\n收到包长度%d:\r\n", plen);
			
			recv_buf = mem_alloc(plen + sizeof(netbuf_hdr));
            if(recv_buf!=NULL)
            {
                enc28j60ReadBuffer(plen, recv_buf->data);

                //USART1_SendArray(recv_buf->data, plen);
                
                recv_buf->next = NULL;
                recv_buf->size = plen;
                local.rx_buf = recv_buf;
                
                eth_accept(local.rx_buf->data, plen);

                mem_free(recv_buf);
            }
		}
	}
	else if(eir & EIR_RXERIF)
	{
		NETIF_DBP("接收错误!!!缓存已满!!!\r\n");
		enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, EIR, EIR_RXERIF);
	}
	else if(eir & EIR_LINKIF)	// PHY连接状态改变
	{
		phy = enc28j60PhyRead(PHIR);
		NETIF_DBP("PHIR=%04X\r\n", phy);

		if(phy & PHIR_PLINKIF)		// PHY连接状态改变
		{
			phy = enc28j60PhyRead(PHSTAT2);
			NETIF_DBP("PHSTAT2=%04X\r\n", phy);

			if(phy & PHSTAT2_LSTAT)
			{
				local.link = LINK;
				NETIF_DBP("链路激活\r\n");
			}
			else
			{
				local.link = LOST;
				NETIF_DBP("链路失效\r\n");
			}
		}
	}

	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE);
}

void *netif_alloc(u32 size)
{
	void *pbuf;

	pbuf = mem_alloc(size + sizeof(netbuf_hdr));

	if(pbuf == NULL)
	{
		return NULL;
	}
	
	return (void *)((u32)pbuf + sizeof(netbuf_hdr));

}

#define netif_free(pbuf) mem_free((void*)(((u32)pbuf)-sizeof(netbuf_hdr)))

// 添加一个数据包到发送缓存链表表头
void netif_push(void *pbuf, u32 size, netif *net_if)
{
	netbuf_tag	*new_buf;
    
    if(size > NETIF_DAT_MAX_LEN)// 长度错误
    {
        netif_free(pbuf);
    }
    else 
    {
        // 确定数据头
        new_buf = (netbuf_tag*)(((u32)pbuf) - sizeof(netbuf_hdr));
        new_buf->size = size;

        // 添加数据包到缓存链表
        new_buf->next = net_if->tx_buf;
        net_if->tx_buf = new_buf;
    }
    
}

// 发送链表上的所有数据包
void netif_send(netif *cur_netif)
{
	netbuf_tag	*pbuf = cur_netif->tx_buf;
	netbuf_tag	*next_pbuf;

	while(pbuf != NULL)
	{
        NETIF_DBP("发送数据%d字节:\r\n", pbuf->size);
        //USART1_SendArray(pbuf->data, pbuf->size);
		cur_netif->send(pbuf->size, pbuf->data);
		next_pbuf = pbuf->next;
		mem_free(pbuf);
		pbuf = next_pbuf;
	}

	cur_netif->tx_buf = NULL;
}

void netif_poll()
{


}


