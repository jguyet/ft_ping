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

/*
** ICMP message HEADER
*/
struct icmphdr
{
	u_char 					type;		/* message type		*/
	u_char 					code;		/* type sub-code	*/
	u_short 				checksum;	/* sum of msglength	*/
	union
	{
		struct
		{
			u_short			id;
			u_short			sequence;	/* sequence id		*/
		}					echo;		/* echo datagram	*/
		unsigned int		gateway;	/* gateway address	*/
	}						un;			/* union			*/
};

/*
** ICMP MESSAGE TYPES
*/
#define ICMP_ECHOREPLY		0	/* Echo Reply			    */
#define ICMP_DEST_UNREACH	3	/* Destination Unreachable	*/
#define ICMP_SOURCE_QUENCH	4	/* Source Quench			*/
#define ICMP_REDIRECT		5	/* Redirect (change route)	*/
#define ICMP_ECHO			8	/* Echo Request				*/
#define ICMP_TIME_EXCEEDED	11	/* Time Exceeded			*/
#define ICMP_PARAMETERPROB	12	/* Parameter Problem		*/
#define ICMP_TIMESTAMP		13	/* Timestamp Request		*/
#define ICMP_TIMESTAMPREPLY	14	/* Timestamp Reply			*/
#define ICMP_INFO_REQUEST	15	/* Information Request		*/
#define ICMP_INFO_REPLY		16	/* Information Reply		*/
#define ICMP_ADDRESS		17	/* Address Mask Request		*/
#define ICMP_ADDRESSREPLY	18	/* Address Mask Reply		*/
#define NR_ICMP_TYPES		18

/*
** icmp packet struct
*/
# define ICMP_HEADER_SIZE	sizeof(struct icmphdr)
# define PACKET_X64 		(72 - ICMP_HEADER_SIZE)

typedef struct 				s_packet
{
	struct icmphdr			header;				/* header of message send 	*/
	char 					msg[PACKET_X64];	/* content of message		*/
}							t_packet;


/*
** ping struct
*/
typedef struct				s_ping
{
	char					*shost;					/* string hostargs			*/
	int						port;					/* port of connection		*/
	struct hostent			*hname;					/* hostname					*/
	int						sock;					/* socket descriptor ID		*/
	struct sockaddr_in		addr;					/* sockaddr of destination	*/
	int						pid;					/* pid of current program	*/
}							t_ping;

BOOLEAN						icmp_initialize_connection(t_ping *ping);

#endif
