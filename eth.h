#ifndef __ETH_H
#define	__ETH_H

#include "netif.h"
#include "mem.h"


#define ETH_TYPE_ARP	0x0806
#define ETH_TYPE_IP		0x0800

#define ETH_HDRLEN      14
#define	ETH_DAT_MAX_LEN	(NETIF_DAT_MAX_LEN-ETH_HDRLEN)

#pragma pack(1)


typedef struct
{
	u8	dst[MAC_ADDR_LEN];		// 目的地址
	u8	src[MAC_ADDR_LEN];		// 源地址
	u16	type;					// 数据类型
	u8	data[ETH_DAT_MAX_LEN];	// 数据
}eth_frm;



#pragma pack()


//void *eth_malloc(u32 size);
void eth_accept(u8* dat, u32 plen);

#endif
