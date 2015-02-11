#ifndef HEADER_STRUCTS_HPP_
#define HEADER_STRUCTS_HPP_

#define IP_DF          0x4000  /* don't fragment flag  */

#define TCP_TSTAMP     0x08    /* Timestamp */
#define TCP_SACKOK     0x04    /* Selective ACK */
#define TCP_WSCALE     0x03    /* Window scaling */
#define TCP_MSS        0x02    /* Maxium Segment Size */
#define TCP_NOP        0x01    /* Nothing */
#define TCP_EOL        0x00    /* End of option */

/* IP header */
struct IPheader {
  u_char   ihl:4,    /* header len */
           version:4;
  u_char   tos;      /* type of service */
  u_short  tot_len,  /* total length */
           id,       /* identification */
           frag_off; /* fragment offset + DF/MF */
  u_char   ttl,      /* time to live */
           protocol; /* protocol */
  u_short  check;    /* checksum */
  u_int    saddr,    /* source */
           daddr;    /* destination */
};

/* TCP header */
struct TCPheader {
  u_short  source,  /* source port */
           dest;    /* destination port */
  u_int    seq,     /* sequence number */
           ack_seq; /* acknowledgment number */
#if __BYTE_ORDER == __LITTLE_ENDIAN
  u_short  res1:4;  /* reserved */
  u_short  doff:4;  /* data offset */
  u_short  fin:1,
           syn:1,
           rst:1,
           psh:1,
           ack:1,
           urg:1;
  u_short  res2:2;  /* reserved */
#elif __BYTE_ORDER == __BIG_ENDIAN
  u_char   doff:4;  /* data offset */
  u_short  res1:4,  /* reserved */
           res2:2;
  u_short  fin:1,
           syn:1,
           rst:1,
           psh:1,
           ack:1,
           urg:1;
#endif
  u_short  window;  /* WSS */
  u_short  check;   /* checksum */
  u_short  urg_ptr; /* urgent pointer */
};

#endif
