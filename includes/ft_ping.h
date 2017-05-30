/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/24 02:45:20 by jguyet            #+#    #+#             */
/*   Updated: 2017/05/24 02:50:31 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PING_H
# define FT_PING_H

# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <stdlib.h>
# include <termios.h>
# include <stdio.h>
# include <fcntl.h>
# include <libft.h>
# include <printf.h>
# include <mapft.h>
# include <unistd.h>
# include <netdb.h>

/*
** INTERNET PROTOCOL IP
*/
# define PROT_INTERNET_IPV4 AF_INET
# define PROT_INTERNET_IPV6 AF_INET6

/*
** Fluxs types
*/
# define BINARY_SOCK_FLUX SOCK_STREAM
# define INTERNAL_NETWORK_FLUX SOCK_RAW
# define SMALL_NETWORK_FLUX SOCK_DGRAM

# ifdef __linux__
/*
** ON linux OS INTERNAL_NETWORK
*/
#  define NETWORK_FLUX INTERNAL_NETWORK_FLUX
# else
/*
** On other OS SMALL_NETWORK_FLUX
*/
#  define NETWORK_FLUX SMALL_NETWORK_FLUX
# endif

/*
** SOCK PROTOCOL
*/
# define DEFAULT_PROTOCOL 0
# define ICMP_PROTOCOL IPPROTO_ICMP

/*
** setsockopt options :
*/
# define EPROTONOSUPPORT 93
# define EAFNOSUPPORT    97

# define SOL_TCP 6
# define SOL_IP 0

# define TCP_KEEPCNT 5
# define TCP_KEEPIDLE 5
# define TCP_KEEPINTVL 1

/*
** unsigned typedef
*/
typedef	unsigned char		u_char;
typedef	unsigned short		u_short;
typedef	unsigned int		u_int;
typedef	unsigned long		u_long;


#ifdef __unix__
	#if BYTE_ORDER == BIG_ENDIAN
		#define htons(n) (n)
	#else
		#define htons(n) (((((unsigned short)(n) & 0xFF)) << 8) | (((unsigned short)(n) & 0xFF00) >> 8))
	#endif
#endif

/*
** ICMP message HEADER
*/
struct icmphdr
{
	u_char					type;		/* message type		*/
	u_char					code;		/* type sub-code	*/
	u_short					checksum;	/* sum of msglength	*/
	union
	{
		struct
		{
			u_short			id;			/* current processid*/
			u_short			sequence;	/* sequence id		*/
		}					echo;		/* echo datagram	*/
		unsigned int		gateway;	/* gateway address	*/
	}						un;			/* union			*/
};

struct iphdr
{
#if BYTE_ORDER == LITTLE_ENDIAN
	u_char  hl:4,			/* header length */
			version:4;		/* version */
#endif
#if BYTE_ORDER == BIG_ENDIAN
	u_char  version:4,		/* version */
			hl:4;			/* header length */
#endif
	u_char  service;		/* type of service */
	u_short len;			/* total length */
	u_short pid;			/* identification */
	u_short off;			/* fragment offset field */
#define	IP_DF 0x4000		/* dont fragment flag */
#define	IP_MF 0x2000		/* more fragments flag */
	u_char  ttl;			/* time to live */
	u_char  protocol;		/* protocol */
	u_short checksum;		/* checksum */
	struct in_addr src;		/* source and dest address */
	struct in_addr dest;	/* source and dest address */
};

/*
** ICMP MESSAGE TYPES
*/
# define ICMP_ECHOREPLY		0	/* Echo Reply			    */
# define ICMP_DEST_UNREACH	3	/* Destination Unreachable	*/
# define ICMP_SOURCE_QUENCH	4	/* Source Quench			*/
# define ICMP_REDIRECT		5	/* Redirect (change route)	*/
# define ICMP_ECHO			8	/* Echo Request				*/
# define ICMP_TIME_EXCEEDED	11	/* Time Exceeded			*/
# define ICMP_PARAMETERPROB	12	/* Parameter Problem		*/
# define ICMP_TIMESTAMP		13	/* Timestamp Request		*/
# define ICMP_TIMESTAMPREPLY 14	/* Timestamp Reply			*/
# define ICMP_INFO_REQUEST	15	/* Information Request		*/
# define ICMP_INFO_REPLY	16	/* Information Reply		*/
# define ICMP_ADDRESS		17	/* Address Mask Request		*/
# define ICMP_ADDRESSREPLY	18	/* Address Mask Reply		*/
# define NR_ICMP_TYPES		18

#define ICMP_MINLEN			28

/*
** Changes the default value set by the TCP/IP service provider in the
** TTL field of the IP header in outgoing datagrams.
** IP_TTL support is not required;
** to check whether IP_TTL is supported,
** use getsockopt to get current options.
** If getsockopt fails, IP_TTL is not supported.
*/
# define TCP_IP_PACKET_HEADER_SERVICE IP_TTL

/*
** icmp packet struct
*/
# define ICMP_HEADER_SIZE	sizeof(struct icmphdr)
# define IPHDR_SIZE 		sizeof(struct iphdr)
# define PACKET_X64 		(64 - ICMP_HEADER_SIZE)

typedef struct				s_packet
{
# ifdef __linux__
	struct iphdr			ip;
# endif
	struct icmphdr			header;				/* header of message send 	*/
}							t_packet;

typedef struct				s_packet_received
{
	struct msghdr			header;		/* header of message received	*/
	struct iovec			iov_buffers;/* container of messages 		*/
	struct iovec			iov[1];		/* first container of messages 	*/
}							t_packet_received;

/*
** Flags
*/
typedef struct				s_flag
{
	BOOLEAN					actif;		/* is actif flag				*/
	char					*name;		/* name of flag					*/
	BOOLEAN					special;	/* has special arguments		*/
	char					*help;		/* text view on --help			*/
	char					*value;		/* arguments					*/
	int						type;		/* type of value				*/
	char					*error;		/* error message				*/
}							t_flag;

# define FLAGS_SIZE			5

/*
** ping struct
*/
typedef struct				s_ping
{
	char					*shost;		/* string hostargs				*/
	int						port;		/* port of connection			*/
	struct hostent			*hname;		/* hostname						*/
	int						sock;		/* socket descriptor ID			*/
	struct sockaddr_in		addr;		/* sockaddr of destination		*/
	int						pid;		/* pid of current program		*/
	int						ttl;		/* time to live  				*/
	int						sequence;	/* sequence id  				*/
	int						received;	/* total received messages  	*/
	int						send;		/* total sended messages  		*/
	int						sweepmaxsize;
	int						sweepminsize;
	int						sweepincrsize;
	t_flag					**flags;	/* map of flags					*/
	BOOLEAN					(*launch)();/* pointer of function launch	*/
	long					start_time;	/* timer						*/
	struct timeval			timeout;	/* timeout						*/
	int						mintime;	/* mintime						*/
	long					totaltime;	/* medium time					*/
	int						maxtime;	/* maxtime						*/
}							t_ping;

# define F_VERBOSE			ping->flags[0]->actif
# define F_SWEEPMAX			ping->flags[1]->actif
# define F_SWEEPMIN			ping->flags[2]->actif
# define F_INCREMENT		ping->flags[3]->actif

BOOLEAN						load_flags(t_ping *ping, int argc, char **argv);
void						load_flag_list(t_ping *ping);
BOOLEAN						icmp_initialize_connection(t_ping *ping, int ttl);
BOOLEAN						set_flags_values(t_ping *ping);

BOOLEAN						start_ping(t_ping *ping);
t_ping						*singleton_ping(void);

void						ping_result(void);

/*
** Messages
*/
t_packet_received			*prepare_packet_receiver(t_ping *ping, size_t size);
void						destruct_packet_receiver(t_packet_received *packet);

void						*prepare_packet_to_send(t_ping *ping, size_t size);
void						destruct_packet_send(t_packet *packet);

/*
** Handler
*/
BOOLEAN						icmp_handle_message(t_ping *ping, t_packet_received *packet_r);
BOOLEAN						icmp_process_received_packet(int readed, t_ping *ping, struct iphdr *ip, t_packet_received *packet);

BOOLEAN						icmp_error_rcvmsg(void);

/*
** Utils
*/
unsigned short				checksum(void *b, int len);
long						get_current_time_millis();

# define MESSAGE_RECEIVED_TRUC		0
# define MESSAGE_RECEIVED_SUCCES	1
# define MESSAGE_RECEIVED_ERROR		-1

#endif
