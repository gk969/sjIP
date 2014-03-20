#ifndef __SIP_H
#define __SIP_H

// LED
#include "stm32f10x.h"
#include "stm32f10x_conf.h"


#define LED_PORT    GPIOF

#define LED_IP      GPIO_Pin_6
#define LED_UDP     GPIO_Pin_7
#define LED_ICMP    GPIO_Pin_8

#define LED_NOT(port, pin) GPIO_WriteBit(port, pin, (GPIO_ReadOutputDataBit(port, pin)?Bit_RESET:Bit_SET));





// 调试信息总开关
#define _DEBUG

// 调试网络接口
#define	DB_NETIF

// 调试内存分配
//#define DB_MEM

// 调试以太网协议
#define DB_ETH

// 调试ARP协议
#define	DB_ARP

// 调试IP协议
#define DB_IP

// 调试ICMP协议
#define DB_ICMP

// 调试UDP协议
#define DB_UDP



#ifdef	_DEBUG
#include "stm32f10x.h"
#include "USART.H"
#define DBP(fmt,arg...) 		USART1_printf(fmt,##arg)

#else
#define	DBP(fmt,arg...)
#endif

#ifdef	DB_NETIF
#define	NETIF_DBP(fmt,arg...)	DBP(fmt,##arg)
#else
#define	NETIF_DBP(fmt,arg...)
#endif

#ifdef	DB_MEM
#define	MEM_DBP(fmt,arg...)	DBP(fmt,##arg)
#else
#define	MEM_DBP(fmt,arg...)
#endif

#ifdef	DB_ETH
#define	ETH_DBP(fmt,arg...)	DBP(fmt,##arg)
#else
#define	ETH_DBP(fmt,arg...)
#endif

#ifdef	DB_ARP
#define	ARP_DBP(fmt,arg...)	DBP(fmt,##arg)
#else
#define	ARP_DBP(fmt,arg...)
#endif

#ifdef	DB_IP
#define	IP_DBP(fmt,arg...)	DBP(fmt,##arg)
#else
#define	IP_DBP(fmt,arg...)
#endif

#ifdef	DB_ICMP
#define	ICMP_DBP(fmt,arg...)	DBP(fmt,##arg)
#else
#define	ICMP_DBP(fmt,arg...)
#endif

#ifdef	DB_UDP
#define	UDP_DBP(fmt,arg...)	DBP(fmt,##arg)
#else
#define	UDP_DBP(fmt,arg...)
#endif

#ifdef	DB_TCP
#define	TCP_DBP(fmt,arg...)	DBP(fmt,##arg)
#else
#define	TCP_DBP(fmt,arg...)
#endif

#ifdef	DB_HTTP
#define	HTTP_DBP(fmt,arg...)	DBP(fmt,##arg)
#else
#define	HTTP_DBP(fmt,arg...)
#endif

#ifdef	DB_HTCP
#define	HTCP_DBP(fmt,arg...)	DBP(fmt,##arg)
#else
#define	HTCP_DBP(fmt,arg...)
#endif






// htonl/ntohl - big endian/little endian byte swapping macros for
// 32-bit (long) values

#ifndef htonl
    #define htonl(a)                    \
        ((((a) >> 24) & 0x000000ff) |   \
         (((a) >>  8) & 0x0000ff00) |   \
         (((a) <<  8) & 0x00ff0000) |   \
         (((a) << 24) & 0xff000000))
#endif


// htons/ntohs - big endian/little endian byte swapping macros for
// 16-bit (short) values

#ifndef htons
    #define htons(a)                \
        ((((a) >> 8) & 0x00ff) |    \
         (((a) << 8) & 0xff00))
#endif



#ifndef u8
typedef unsigned char	u8;
typedef unsigned short	u16;
typedef unsigned int	u32;
#endif



#endif


