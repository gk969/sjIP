#include "sjip.h"
#include "ip.h"
#include "udp.h"
#include "mem.h"
#include "arp.h"

/*
拥塞控制，即窗口大小、超时重传时间动态选择，暂时不予实现。

TCP协议是面向字节的。TCP将所要传送的整个报文（可能包含许多报文段）看成一个个字节组成的数据流，并使每一个字节对应一个序号。在连接建立时，双方要商定初始序号。TCP每次发送的报文段首部中的序号表示该报文段中数据部分第一个字节的序号。

TCP的确认是对接收到数据的最高序号（接收到数据报文中最后一个字节的序号）表示确认。接收端返回的确认号是已接收数据的最高序号加1，即接收端期望下次接收到数据首字节的序号。

TCP发送一段报文时，同时再自己的重传队列中存放一个副本，若收到确认则删除此副本。若重传计时结束之前还未收到确认，则重传此副本。

TCP长数据分段发送时，收到确认之后才能发送下一报文段。

为了保证TCP的效率。接收端最好不要提供一个很小的窗口，发送端也最好不要发送一个很短的数据。


    连接建立，三次握手：
    
    客户端                                  服务器
    连接请求--------- SYN,SEQ=x ---------->
           <--- SYN,ACK,SEQ=y,ACK=x+1 ----  确认
        确认---- ACK,SEQ=x+1,ACK=y+1 ----->
        
    
    连接释放：
    客户端                                  服务器
    单向释放--------- FIN,SEQ=x ---------->
    停止发送<----ACK,SEQ=y,ACK=x+1--------- 确认
此时连接处于半关闭状态，发起结束一方不再发送数据，但能仍能接收数据，只要正确接收到数据仍需发送确认。
    
           <----FIN,ACK,SEQ=y,ACK=x+1----- 
       确认 ----ACK,SEQ=x+1,ACK=y+1-------> 停止发送
    

*/
//CH1993to2013bbc
void tcp_accept(const ip_frm *ip_recv)
{
    
}