#include "sjip.h"
#include "ip.h"
#include "icmp.h"
#include "mem.h"


void icmp_accept(const ip_frm *ip_recv)
{
    icmp_frm *icmp_recv = (icmp_frm *)(ip_recv->data);
    u16 ip_frmlen=htons(ip_recv->frmlen);
    u16 icmp_frmlen = ip_frmlen - IP_HDRLEN;
    ip_frm *ipfrm_icmprep;
    icmp_frm *icmp_rep;

    ICMP_DBP("ICMP Type %02X\r\n", icmp_recv->type);

    if((icmp_frmlen > ICMP_HDRLEN) && (chksum_short((u16 *)icmp_recv, icmp_frmlen) == 0))
    {
        if(icmp_recv->type == ICMP_ECHO_REQ)
        {
            ipfrm_icmprep = ip_malloc(ip_frmlen);

            if(ipfrm_icmprep != NULL)
            {
                ipfrm_icmprep->vsn = IPV4_HDR20B;
                ipfrm_icmprep->tos = ip_recv->tos;
                ipfrm_icmprep->frmlen = ip_recv->frmlen;
                ipfrm_icmprep->id = htons(get_ipid());
                ipfrm_icmprep->ofs = 0;
                ipfrm_icmprep->ttl = ICMP_REP_TTL;
                ipfrm_icmprep->pt = IP_ICMP;
                ipfrm_icmprep->chksum = 0;
                ipfrm_icmprep->src.inw = ip_recv->dst.inw;
                ipfrm_icmprep->dst.inw = ip_recv->src.inw;
                ipfrm_icmprep->chksum = htons(chksum_short((u16 *)ipfrm_icmprep, IP_HDRLEN));

                icmp_rep = (icmp_frm *)(ipfrm_icmprep->data);
                icmp_rep->type = ICMP_ECHO_REP;
                icmp_rep->code = icmp_recv->code;
                icmp_rep->chksum = 0;
                mem_copy((u8 *)(icmp_rep->opt), (u8 *)(icmp_recv->opt),
                         ip_frmlen - (IP_HDRLEN + ICMP_HDRLEN - sizeof(icmp_rep->opt)));
                icmp_rep->chksum = htons(chksum_short((u16 *)icmp_rep, icmp_frmlen));
                
                ip_push(ipfrm_icmprep);
                
                
                LED_NOT(LED_PORT, LED_ICMP);
            }
        }
    }
}


