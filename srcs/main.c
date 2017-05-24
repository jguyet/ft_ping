/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/24 01:50:56 by jguyet            #+#    #+#             */
/*   Updated: 2017/05/24 01:51:33 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

#include <sys/time.h>

#include <arpa/inet.h>

t_ping		*new_ping(void)
{
	t_ping	*ping;

	if (!(ping = (t_ping*)malloc(sizeof(t_ping))))
		return (NULL);
	ping->shost = NULL;
	ping->port = -1;
	return (ping);
}

void		destruct_ping(t_ping *ping)
{
	if (ping->shost != NULL)
		ft_strdel(&ping->shost);
	free(ping);
}

unsigned short checksum(void *b, int len)
{
	ushort	*buf;
	u_int	sum;

	buf = b;
	sum = 0;
	while (len > 1)
	{
		sum += *buf++;
		len -= 2;
	}
	if (len == 1)
		sum += buf[0] & 0xFF;//*(unsigned char*)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	return (~sum);
}

long		get_current_time_millis()
{
	struct timeval time_v;

	if (gettimeofday(&time_v, NULL) == -1)
		return (0);
	return (time_v.tv_usec);
}

int update_buffer(struct msghdr* msg, size_t bytes) {
  while (msg->msg_iovlen > 0) {
    if (bytes < msg->msg_iov[0].iov_len) {
      msg->msg_iov[0].iov_len -= bytes;
      msg->msg_iov[0].iov_base += bytes;
      return 1;
    }
    bytes -= msg->msg_iov[0].iov_len;
    ++msg->msg_iov;
    --msg->msg_iovlen;
  }
  return 0;
}

int			wait_message(t_ping *ping, t_packet_received *packet_r)
{
	char buffer[5000];
	struct iovec iov[1];                       /* Data array */
	struct msghdr msg;                      /* Message header */
	//struct cmsghdr *cmsg;                   /* Control related data */
	//struct sock_extended_err *sock_err;     /* Struct describing the error */ 
	//struct icmphdr icmph;                   /* ICMP header */
	//struct sockaddr_in remote;              /* Our socket */

	(void)packet_r;
	ft_bzero(buffer, 5000);
	iov[0].iov_base = &buffer;
    iov[0].iov_len = sizeof(buffer);
    msg.msg_name = (void*)&ping->addr;
    msg.msg_namelen = sizeof(ping->addr);
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    msg.msg_flags = 0;
    msg.msg_control = 0;//buffer;
    msg.msg_controllen = 0;//sizeof(buffer);

    int ret;
	if ((ret = recvmsg(ping->sock, &msg, 0)) != -1)
	{
		if (msg.msg_flags & MSG_TRUNC)
		{
			printf("MSG_TRUNC ret : %d\n", ret);
			printf("msg : %s\n", buffer);
			return (MESSAGE_RECEIVED_TRUC);
		}
		printf("msg : %s\n", buffer);
		return (MESSAGE_RECEIVED_SUCCES);
	}
	printf("ret1 : %d\n", ret);
	return (MESSAGE_RECEIVED_ERROR);
}

int			wait_message_two(t_ping *ping, t_packet_received *packet_r)
{
    int ret;

	if ((ret = recvmsg(ping->sock, &packet_r->header, 0)) != -1)
	{
		if (packet_r->header.msg_flags & MSG_TRUNC)
		{
			printf("MSG_TRUNC ret : %d\n", ret);
			return (MESSAGE_RECEIVED_TRUC);
		}
		printf("msg : %s\n", packet_r->iov[0].iov_base);
		return (MESSAGE_RECEIVED_SUCCES);
	}
	printf("ret1 : %d\n", ret);
	return (MESSAGE_RECEIVED_ERROR);
}

void		test(t_ping *ping)
{
	int i = 1;
	int sequence = 0;
	t_packet packet;
	long start;
	int ret;
	t_packet_received *packet_r;

	//packet_r = prepare_packet_receiver(&ping->addr, 400);
	//wait_message(ping);
	while(42)
	{
		start = get_current_time_millis();
		ft_bzero(&packet, sizeof(packet));
		packet.header.type = ICMP_ECHO;
		packet.header.un.echo.id = ping->pid;
		for (i = 0; i < (int)sizeof(packet.msg) - 1; i++)
			packet.msg[i] = i + '0';
		packet.msg[i] = 0;
		packet.header.un.echo.sequence = sequence++;
		packet.header.checksum = checksum(&packet, sizeof(packet));
		if (sendto(ping->sock, &packet, sizeof(packet), 0, (struct sockaddr*)&ping->addr, sizeof(ping->addr)) <= 0 )
			perror("sendto");
		
		packet_r = prepare_packet_receiver(ping, 5000);
		
		ret = wait_message_two(ping, packet_r);
		if (ret == MESSAGE_RECEIVED_SUCCES || ret == MESSAGE_RECEIVED_TRUC)
			printf("%d bytes from %s: icmp_seq=%d time=0.%3ld ms\n", (int)sizeof(packet.msg), inet_ntoa(ping->addr.sin_addr), sequence, get_current_time_millis() - start);
		else
			printf("Request timeout for icmp_seq %d\n", sequence);
		//printf("msg : %s\n", packet_r->msg);
		//destruct_packet_receiver(packet_r);
		sleep(1);
    }
}

int	main(int argc, char **argv)
{
	t_ping	*ping;

	if (argc  == 2)
	{
		ping = new_ping();
		if (ping == NULL)
			return (1);
		ping->shost = ft_strdup(argv[1]);
		ping->hname = gethostbyname(ping->shost);
		ping->pid = getpid();
		if (ping->hname)
		{
			icmp_initialize_connection(ping);
			printf("PING %s (%s):\n", ping->shost, inet_ntoa(ping->addr.sin_addr));
			test(ping);
		}
		destruct_ping(ping);
	}
	return (0);
}
