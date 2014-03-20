#ifndef NETIF_H
#define NETIF_H

#define TRUE	1
#define FALSE	0

#define NETIF_DAT_MAX_LEN	1480//(6+6+2+1500)

#define MAC_ADDR_LEN			6


#define LINK		1
#define LOST		0

typedef struct net_buf
{
	u32				size;
	struct net_buf*	next;
}netbuf_hdr;


typedef struct netif_buf
{
	u32					size;
	struct netif_buf*	next;
	u8					data[NETIF_DAT_MAX_LEN];
}netbuf_tag;

typedef union
{
	u8 	inb[4];
	u32	inw;
}ip_addr;

typedef struct
{
	u8		mac[MAC_ADDR_LEN];	// MAC��ַ
	ip_addr	ip;					// IP��ַ
	ip_addr	mask;				// ��������
	ip_addr	gateway;			// ����
	
	u8		link;			// ��·״̬
	
	netbuf_tag*	rx_buf;		// ���ջ�����
	netbuf_tag*	tx_buf;		// ���ͻ�����
	
	void (*send)(u32 size, u8 *packet);
}netif;	



#define NETIF_INT()	(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2))

extern netif local;

void *netif_alloc(u32 size);
void netif_interrupt(void);
void netif_push(void *pbuf, u32 size, netif *net_if);
void netif_send(netif *cur_netif);
void netif_poll(void);


#endif
