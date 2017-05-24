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

/*
** retourne la somme de total d'octet libre de (void* b)
** sur la longueur (int len)
*/
unsigned short checksum(void *b, int len)
{
	u_short	*buf;
	u_short	low;
	u_short	high;
	u_int	sum;

	buf = b;
	sum = 0;
	while (len > 1)
	{
		sum += *buf++;
		len -= 2;
	}
	if (len == 1)
		sum += buf[0] & 0xFF;
	low = (sum & 0xFFFF);			/* prend les 2 octet de droite					*/
	high = (sum >> 16);				/* prend les 2 octet de gauche					*/
	sum = high + low;				/* aditionne le tout							*/
	return ((unsigned short)~sum);	/* inverse les bits actif et cast en unsigned	*/
}

/*
** retourne le temps actuel en millisecondes
*/
long		get_current_time_millis()
{
	struct timeval time_v;

	if (gettimeofday(&time_v, NULL) == -1)
		return (0);
	return (time_v.tv_usec);
}

/*
** read le message icmp header-packet
*/
int			wait_message(t_ping *ping, t_packet_received *packet_r)
{
	int	ret;

	if ((ret = recvmsg(ping->sock, &packet_r->header, 0)) != -1)
	{
		if (packet_r->header.msg_flags & MSG_TRUNC)
		{
			printf("MSG_TRUNC\n");
			return (MESSAGE_RECEIVED_TRUC);
		}
		return (MESSAGE_RECEIVED_SUCCES);
	}
	return (MESSAGE_RECEIVED_ERROR);
}

void		test(t_ping *ping)
{
	int i = 1;
	int sequence = 0;
	long start;
	int ret;
	t_packet_received *packet_r;

	while(42)
	{
		t_packet packet;

		ft_bzero(&packet, sizeof(packet));
		packet.header.type = ICMP_ECHO;
		packet.header.un.echo.id = ping->pid;
		for (i = 0; i < (int)sizeof(packet.msg) - 1; i++)
			packet.msg[i] = i + '0';
		packet.msg[i] = 0;
		packet.header.un.echo.sequence = sequence;
		packet.header.checksum = checksum(&packet, sizeof(packet));
		start = get_current_time_millis();
		if (sendto(ping->sock, &packet, sizeof(packet), 0, (struct sockaddr*)&ping->addr, sizeof(ping->addr)) <= 0)
			printf("error to send");
		packet_r = prepare_packet_receiver(ping, 5000);
		ret = wait_message(ping, packet_r);
		if (ret == MESSAGE_RECEIVED_SUCCES || ret == MESSAGE_RECEIVED_TRUC)
			printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", (int)sizeof(packet.msg), inet_ntoa(ping->addr.sin_addr), sequence, ping->ttl, ((float)(get_current_time_millis() - start)) / 1000);
		else
			printf("Request timeout for icmp_seq %d\n", sequence);
		//printf("msg : %s\n", packet_r->msg);
		destruct_packet_receiver(packet_r);
		sleep(1);
		sequence++;
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
		ping->ttl = 55;
		if (ping->hname)
		{
			icmp_initialize_connection(ping, ping->ttl);
			printf("PING %s (%s): %lu data bytes\n", ping->shost, inet_ntoa(ping->addr.sin_addr), 64 - ICMP_HEADER_SIZE);
			test(ping);
		}
		destruct_ping(ping);
	}
	return (0);
}
