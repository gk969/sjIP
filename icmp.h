#ifndef __ICMP_H
#define __ICMP_H

#include "ip.h"

// ICMP 差错报告
#define	ICMP_DST_UNREACH	3	// destination unreachable 终点不可达
#define ICMP_SRC_QUENCH		4	// source quench 源站抑制
#define ICMP_TIME_UP		11	// 超时
#define ICMP_PARAM_ERROR	12	// 参数错误
#define ICMP_REDIRECT		5	// 路由重定向

// ICMP 查询
#define ICMP_ECHO_REQ		8	// 回送请求
#define ICMP_ECHO_REP		0	// 回送答复
#define ICMP_TIME_REQ		13	// 时间戳请求
#define ICMP_TIME_REP		14	// 时间戳回复
#define ICMP_MASK_REQ		17	// 地址掩码请求
#define ICMP_MASK_REP		18	// 地址掩码回复
#define ICMP_ROUT_REQ		10	// 路由请求
#define ICMP_ROUT_REP		9	// 路由回复


// ICMP回复报文TTL
#define ICMP_REP_TTL        128

#define ICMP_HDRLEN         8

#pragma pack(1)
typedef struct
{
	u8	type;
	u8	code;
	u16	chksum;
	u8	opt[4];
    u8  data[IP_DAT_MAX_LEN-ICMP_HDRLEN];
}icmp_frm;
#pragma pack()

void icmp_accept(const ip_frm *ip_recv);

#endif

