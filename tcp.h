#ifndef __TCP_H
#define __TCP_H

#pragma pack(1)

typedef struct
{
    u16 src_port;   // 源端口                         
    u16 dst_port;   // 目的端口
    u32 seq_num;    // 序号 本报文段数据首字节的序号
    u32 ack_num;    // 确认号 期望收到对方下一报文的数据首字节的序号 
                    // 同时也是对接收到的最高序号的确认可同数据一起发送
    u8  hdr_len;    // 首部长度 也称数据偏移 高4位 单位4字节
    u8  flag;       // 标志位 低6位 URG、ACK、PSH、RST、SYN、FIN
    u16 win_size;   // 窗口长度 通知对方发送窗口的上限 单位字节
    u16 chk_sum;    // 校验 包括首部、数据、伪首部
    u16 urgent;     // 紧急指针
}udp_frm;

#pragma pack()

typedef struct sck_tcp
{
    u16     local_port;     
    u16     remote_port;    
    ip_addr remote_ip;      
    u32     sequence;       // 序号
    u32     retrans_tmr;    // 重传定时器
    u8      state;          // 当前状态
    void    (*recv)(u8* data, u16 len, struct sck_tcp *sck);
}tcp_socket;

enum tcp_state
{
    TCP_CLOSED,
    TCP_LISTEN,
    TCP_SYN_RCVD,
    TCP_SYN_SENT,
    TCP_ESTABLISHED,
    TCP_CLOSE_WAIT,
    TCP_LAST_ACK,
    TCP_FIN_WAIT1,
    TCP_CLOSING,
    TCP_TIME_WAIT,
    TCP_FIN_WAIT2
};

// flag 标志位
#define URG 0x20    // 紧急 与紧急指针配合发送紧急数据
#define ACK 0x10    // 确认 ACK=1 确认字段有效，反之无效
#define PSH 0x08    // 推送 PSH=1 希望接收方尽快响应
#define RST 0x04    // 复位 RST=1 需释放并重建连接

#define SYN 0x02    // 同步 在建立连接时用来同步序号 
                    //      SYN=1、ACK=0时表明这是一个连接请求报文
                    //      对方若同意建立连接 则在响应报文中SYN=1、ACK=1

#define FIN 0x01    // 终止 FIN=1 发送端数据已发送完毕 要求释放连接

void tcp_accept(const ip_frm *ip_recv);

#endif
