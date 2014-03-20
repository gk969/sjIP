#ifndef __ICMP_H
#define __ICMP_H

#include "ip.h"

// ICMP �����
#define	ICMP_DST_UNREACH	3	// destination unreachable �յ㲻�ɴ�
#define ICMP_SRC_QUENCH		4	// source quench Դվ����
#define ICMP_TIME_UP		11	// ��ʱ
#define ICMP_PARAM_ERROR	12	// ��������
#define ICMP_REDIRECT		5	// ·���ض���

// ICMP ��ѯ
#define ICMP_ECHO_REQ		8	// ��������
#define ICMP_ECHO_REP		0	// ���ʹ�
#define ICMP_TIME_REQ		13	// ʱ�������
#define ICMP_TIME_REP		14	// ʱ����ظ�
#define ICMP_MASK_REQ		17	// ��ַ��������
#define ICMP_MASK_REP		18	// ��ַ����ظ�
#define ICMP_ROUT_REQ		10	// ·������
#define ICMP_ROUT_REP		9	// ·�ɻظ�


// ICMP�ظ�����TTL
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

