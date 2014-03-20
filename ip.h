#ifndef __IP_H
#define __IP_H

#include	"netif.h"
#include    "eth.h"

#define IP_ADDR_LEN		4

// IP首部协议类型字段值
#define IP_ICMP		1
#define IP_IGMP		2
#define IP_TCP		6
#define IP_EGP		8
#define IP_IGP		9
#define IP_UDP		17

#define	IP_DF		0x4000
#define IP_MF		0x2000
#define IP_OFS		0x1FFF

// IP版本信息 
// IPv4 首部长度20字节
#define IPV4_HDR20B	0x45

extern u16 ip_id;
#define get_ipid() (ip_id++)

#define IP_HDRLEN   20
#define IP_DAT_MAX_LEN		(ETH_DAT_MAX_LEN-IP_HDRLEN)


#pragma pack(1)
typedef struct 
{
	u8		vsn;			// 版本信息高4位 首部长度低4位
	u8		tos;			// 服务类型 type of service
	u16		frmlen;			// 总长度 首部加数据
	u16 	id;				// 标识 用于长报文分片标识
	u16		ofs;			// 分片标志高3位 片偏移低13位 偏移量8字节/单位
	u8		ttl;			// 生存时间	time to live
	u8		pt;				// 报文协议类型
	u16		chksum;			// 首部校验和
	ip_addr	src;			// 源地址
	ip_addr	dst;			// 目标地址
    u8      data[IP_DAT_MAX_LEN];
}ip_frm;					// IP数据包首部



typedef struct
{
    ip_addr src_ip; // 源IP
    ip_addr dst_ip; // 目的IP
    u16     pad;    // 填充 协议类型
    u16     frmlen; // 帧长度
}fake_ipHdr;


#pragma pack()


u16 chksum_short(const u16 *data, u16 len);
u16 ChksumWithFackHdr(const ip_frm * ip, u16 type);
void ip_accept(const u8 *data, u16 plen);
void *ip_malloc(u16 size);
void ip_push(ip_frm *ipfrm);

#endif
