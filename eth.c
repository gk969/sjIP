#include "sjip.h"
#include "eth.h"
#include "arp.h"
#include "mem.h"
#include "ip.h"

// accept lower layer protocol frame
// submit to the upper layer protocol
void eth_accept(u8* dat, u32 plen)
{
	eth_frm* eth=(eth_frm*)dat;
	
	plen-=ETH_HDRLEN;
	switch(htons(eth->type))
	{
		case ETH_TYPE_ARP:
			arp_accept(eth->data, plen);
		break;
		
		case ETH_TYPE_IP:
			ip_accept(eth->data, plen);
		break;
	}
}
