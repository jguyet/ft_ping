#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;
typedef unsigned long   u_long;
typedef unsigned short  ushort;

struct icmphdr
{
  u_char        type;       /* message type */
  u_char        code;       /* type sub-code */
  u_short       checksum;
  union
  {
    struct
    {
      u_short   id;
      u_short   sequence;
    } echo;             /* echo datagram */
    unsigned int        gateway;    /* gateway address */
    struct
    {
      //u_short __unused;
      u_short   mtu;
    } frag;             /* path mtu discovery */
  } un;
};

#define ICMP_ECHOREPLY      0   /* Echo Reply           */
#define ICMP_DEST_UNREACH   3   /* Destination Unreachable  */
#define ICMP_SOURCE_QUENCH  4   /* Source Quench        */
#define ICMP_REDIRECT       5   /* Redirect (change route)  */
#define ICMP_ECHO       8   /* Echo Request         */
#define ICMP_TIME_EXCEEDED  11  /* Time Exceeded        */
#define ICMP_PARAMETERPROB  12  /* Parameter Problem        */
#define ICMP_TIMESTAMP      13  /* Timestamp Request        */
#define ICMP_TIMESTAMPREPLY 14  /* Timestamp Reply      */
#define ICMP_INFO_REQUEST   15  /* Information Request      */
#define ICMP_INFO_REPLY     16  /* Information Reply        */
#define ICMP_ADDRESS        17  /* Address Mask Request     */
#define ICMP_ADDRESSREPLY   18  /* Address Mask Reply       */
#define NR_ICMP_TYPES       18

//note, to allow root to use icmp sockets, run:
//sysctl -w net.ipv4.ping_group_range="0 0"

void ping_it(struct in_addr *dst)
{
    struct icmphdr icmp_hdr;
    struct sockaddr_in addr;
    int sequence = 0;
    int sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_ICMP);
    if (sock < 0) {
        perror("socket");
        return ;
    }

    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr = *dst;

    memset(&icmp_hdr, 0, sizeof icmp_hdr);
    icmp_hdr.type = ICMP_ECHO;
    icmp_hdr.un.echo.id = 1234;//arbitrary id

    for (;;) {
        unsigned char data[2048];
        int rc;
        struct timeval timeout = {1, 0}; //wait max 3 seconds for a reply
        fd_set read_set;
        socklen_t slen;
        struct icmphdr rcv_hdr;

        icmp_hdr.un.echo.sequence = sequence++;
        memcpy(data, &icmp_hdr, sizeof icmp_hdr);
        //memcpy(data + sizeof icmp_hdr, "hello", 5); //icmp payload
        rc = sendto(sock, data, sizeof icmp_hdr,
                        0, (struct sockaddr*)&addr, sizeof addr);
        if (rc <= 0) {
            perror("Sendto");
            break;
        }
        puts("Sent ICMP\n");

        memset(&read_set, 0, sizeof read_set);
        FD_SET(sock, &read_set);

        //wait for a reply with a timeout
        rc = select(sock + 1, &read_set, NULL, NULL, &timeout);
        if (rc == 0) {
            puts("Got no reply\n");
            continue;
        } else if (rc < 0) {
            perror("Select");
            break;
        }

        //we don't care about the sender address in this example..
        slen = 0;
        rc = recvfrom(sock, data, sizeof data, 0, NULL, &slen);
        if (rc <= 0) {
            perror("recvfrom");
            break;
        } else if (rc < sizeof rcv_hdr) {
            printf("Error, got short ICMP packet, %d bytes\n", rc);
            break;
        }
        memcpy(&rcv_hdr, data, sizeof rcv_hdr);
        if (rcv_hdr.type == ICMP_ECHOREPLY) {
            printf("ICMP Reply, id=0x%x, sequence =  0x%x\n",
                            icmp_hdr.un.echo.id, icmp_hdr.un.echo.sequence);
        } else {
            printf("Got ICMP packet with type 0x%x ?!?\n", rcv_hdr.type);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("usage: %s destination_ip\n", argv[0]);
        return 1;
    }
    struct in_addr dst;

    if (inet_aton(argv[1], &dst) == 0)
    {
        perror("inet_aton");
        printf("%s isn't a valid IP address\n", argv[1]);
        return 1;
    }

    ping_it(&dst);
    return 0;
}