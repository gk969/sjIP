#include "sjip.h"
#include "enc28j60.h"
#include "netif.h"
#include "eth.h"
#include "mem.h"

// Э�鷢����������
// �����²���ڴ����� ��ȡ�㹻���ɱ���Э��֡���²�Э��ͷ���ڴ�ռ�
// ��䱾��Э��֡ ���ݱ���Э����Ҫ����²�Э��ͷ
// �����²�Э����ջ���� ���ݵ��뷢�ͻ�������(ջ ����ȳ�)
// ��ѯ������ӿڷ��ͻ��� �������� ȫ������



netif local =
{
	{0x00, 0x02, 0x99, 0x79, 0x24, 0x58},	// MAC
	{192, 168, 0, 10},						// IP
	{255, 255, 255, 0},						// Mask
	{192, 168, 0, 1},						// Gateway

	LOST,                                   // ��·״̬

	NULL,                                   // ���ջ�����
	NULL,                                   // ���ͻ�����

	enc28j60PacketSend                      // ���ͺ���
};


// ��������ӿ�Ӳ���Ѵ��л��� �ʶ�������軺�� ����һ������һ������
void netif_interrupt(void)
{
	u16 plen;
	u8 eir;
	u16 phy;

	netbuf_tag *recv_buf;

	enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, EIE, EIE_INTIE);

	eir = enc28j60Read(EIR);

	if(eir & EIR_PKTIF)		// �������ݰ�������
	{

		plen = enc28j60PacketReceiveLen(NETIF_DAT_MAX_LEN);
		if((plen > 0)&&(plen<NETIF_DAT_MAX_LEN))
		{
			NETIF_DBP("\r\n�յ�������%d:\r\n", plen);
			
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
		NETIF_DBP("���մ���!!!��������!!!\r\n");
		enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, EIR, EIR_RXERIF);
	}
	else if(eir & EIR_LINKIF)	// PHY����״̬�ı�
	{
		phy = enc28j60PhyRead(PHIR);
		NETIF_DBP("PHIR=%04X\r\n", phy);

		if(phy & PHIR_PLINKIF)		// PHY����״̬�ı�
		{
			phy = enc28j60PhyRead(PHSTAT2);
			NETIF_DBP("PHSTAT2=%04X\r\n", phy);

			if(phy & PHSTAT2_LSTAT)
			{
				local.link = LINK;
				NETIF_DBP("��·����\r\n");
			}
			else
			{
				local.link = LOST;
				NETIF_DBP("��·ʧЧ\r\n");
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

// ���һ�����ݰ������ͻ��������ͷ
void netif_push(void *pbuf, u32 size, netif *net_if)
{
	netbuf_tag	*new_buf;
    
    if(size > NETIF_DAT_MAX_LEN)// ���ȴ���
    {
        netif_free(pbuf);
    }
    else 
    {
        // ȷ������ͷ
        new_buf = (netbuf_tag*)(((u32)pbuf) - sizeof(netbuf_hdr));
        new_buf->size = size;

        // ������ݰ�����������
        new_buf->next = net_if->tx_buf;
        net_if->tx_buf = new_buf;
    }
    
}

// ���������ϵ��������ݰ�
void netif_send(netif *cur_netif)
{
	netbuf_tag	*pbuf = cur_netif->tx_buf;
	netbuf_tag	*next_pbuf;

	while(pbuf != NULL)
	{
        NETIF_DBP("��������%d�ֽ�:\r\n", pbuf->size);
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


