#ifndef __IP_H
#define __IP_H

#include	"netif.h"
#include    "eth.h"

#define IP_ADDR_LEN		4

// IP�ײ�Э�������ֶ�ֵ
#define IP_ICMP		1
#define IP_IGMP		2
#define IP_TCP		6
#define IP_EGP		8
#define IP_IGP		9
#define IP_UDP		17

#define	IP_DF		0x4000
#define IP_MF		0x2000
#define IP_OFS		0x1FFF

// IP�汾��Ϣ 
// IPv4 �ײ�����20�ֽ�
#define IPV4_HDR20B	0x45

extern u16 ip_id;
#define get_ipid() (ip_id++)

#define IP_HDRLEN   20
#define IP_DAT_MAX_LEN		(ETH_DAT_MAX_LEN-IP_HDRLEN)


#pragma pack(1)
typedef struct 
{
	u8		vsn;			// �汾��Ϣ��4λ �ײ����ȵ�4λ
	u8		tos;			// �������� type of service
	u16		frmlen;			// �ܳ��� �ײ�������
	u16 	id;				// ��ʶ ���ڳ����ķ�Ƭ��ʶ
	u16		ofs;			// ��Ƭ��־��3λ Ƭƫ�Ƶ�13λ ƫ����8�ֽ�/��λ
	u8		ttl;			// ����ʱ��	time to live
	u8		pt;				// ����Э������
	u16		chksum;			// �ײ�У���
	ip_addr	src;			// Դ��ַ
	ip_addr	dst;			// Ŀ���ַ
    u8      data[IP_DAT_MAX_LEN];
}ip_frm;					// IP���ݰ��ײ�



typedef struct
{
    ip_addr src_ip; // ԴIP
    ip_addr dst_ip; // Ŀ��IP
    u16     pad;    // ��� Э������
    u16     frmlen; // ֡����
}fake_ipHdr;


#pragma pack()


u16 chksum_short(const u16 *data, u16 len);
u16 ChksumWithFackHdr(const ip_frm * ip, u16 type);
void ip_accept(const u8 *data, u16 plen);
void *ip_malloc(u16 size);
void ip_push(ip_frm *ipfrm);

#endif
