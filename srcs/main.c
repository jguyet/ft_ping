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
#include "sig.h"

#include <sys/time.h>

#include <arpa/inet.h>

t_ping		*singleton_ping(void)
{
	static t_ping	*ping = NULL;

	if (ping != NULL)
		return (ping);
	if (!(ping = (t_ping*)malloc(sizeof(t_ping))))
		return (NULL);
	ping->sequence = 0;
	ping->shost = NULL;
	ping->hname = NULL;
	ping->port = -1;
	ping->launch = start_ping;
	ping->received = 0;
	ping->datalen = PACKET_X64;
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
		return (ret);
	}
	return (MESSAGE_RECEIVED_ERROR);
}

void		ft_sleep(int t)
{
	alarm(t);
	while(g_breakflag == 0)
		;
	g_breakflag = 0;
}

void	unpack(t_ping *ping, t_packet_received *packet, int cc, long start)
{
	struct iphdr	*ip;

	ip = (struct iphdr*)packet->iov[0].iov_base;

	if (cc < ping->datalen + ICMP_MINLEN)
	{
		if (F_VERBOSE)
			ft_fprintf(1, "ft_ping: packet too short (%d bytes) from %s\n", cc, inet_ntoa(ip->src));
		else if (F_VERBOSE == false)
			ft_fprintf(1, "F_VERBOSE == false\n");
		return ;
	}
	cc -= sizeof(*ip);
	printf("ttl : %d\n", ip->ttl);
	printf("len : %d\n", ip->len);
	printf("cc : %d\n", cc);
	printf("src : %s\n", inet_ntoa(ip->src));
	printf("dest : %s\n", inet_ntoa(ip->dest));
	if (cc != -1)
	{
		printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",\
			cc, inet_ntoa(ip->src),\
			ping->sequence, ip->ttl,\
			((float)(get_current_time_millis() - start)) / 1000);
		ping->received++;
	}
	else
		printf("Request timeout for icmp_seq %d\n", ping->sequence);
}

BOOLEAN		start_ping(t_ping *ping)
{
	int i = 1;
	long start;
	int ret;

	while (true)
	{
		t_packet_received *packet_r;
		t_packet packet;

		ft_bzero(&packet, sizeof(packet));
		packet.header.type = ICMP_ECHO;
		packet.header.un.echo.id = ping->pid;
		for (i = 0; i < (int)sizeof(packet.msg) - 1; i++)
			packet.msg[i] = i + '0';
		packet.msg[i] = 0;
		packet.header.un.echo.sequence = ping->sequence;
		packet.header.checksum = checksum(&packet, sizeof(packet));
		start = get_current_time_millis();
		if (sendto(ping->sock, &packet,\
			sizeof(packet), 0, (struct sockaddr*)&ping->addr,\
			sizeof(ping->addr)) <= 0)
			printf("error to send");
		packet_r = prepare_packet_receiver(ping, 5000);
		ret = wait_message(ping, packet_r);
		unpack(ping, packet_r, ret, start);
		destruct_packet_receiver(packet_r);
		ping->sequence++;
		ft_sleep(1);
	}
	return (true);
}

t_flag	*newflag(t_flag *f)
{
	t_flag *n;

	n = malloc(sizeof(t_flag));
	n->actif = f->actif;
	n->name = ft_strdup(f->name);
	n->special = f->special;
	n->help = ft_strdup(f->help);
	n->value = NULL;
	n->type = f->type;
	n->error = ft_strdup(f->error);
	return (n);
}

void	load_flag_list(t_ping *ping)
{
	ping->flags = malloc(sizeof(t_flag) * FLAGS_SIZE);
	ping->flags[0] = newflag(&(t_flag){false, "v", false, NULL, NULL, 0, NULL});
	ping->flags[1] = newflag(&(t_flag){false, "G", true, "[-G sweepmaxsize]", NULL, 1, "invalid packet size: `%s'"});
	ping->flags[2] = newflag(&(t_flag){false, "g", true, "[-g sweepminsize]", NULL, 1, "invalid packet size: `%s'"});
	ping->flags[3] = newflag(&(t_flag){false, "h", true, "[-h sweepincrsize]", NULL, 1 , "invalid increment size: `%s'"});
	ping->flags[4] = newflag(&(t_flag){false, "m", true, "[-m ttl]", NULL, 1, "invalid TTL: `%s'"});
	ping->flags[5] = newflag(&(t_flag){false, "t", true, "[-t timeout]", NULL, 1, "invalid timeout: `%s'"});
}

int	main(int argc, char **argv)
{
	t_ping	*ping;

	if (argc > 1)
	{
		ping = singleton_ping();
		if (ping == NULL)
			return (0);
		load_flag_list(ping);
		if (!load_flags(ping, argc, argv))
			return (0);
		g_breakflag = 0;
		signal(SIGINT, ctrlc);
		signal(SIGTSTP, ctrlz);
		signal(SIGQUIT, ctrlq);
		signal(SIGALRM, sig_alarm);
		ping->pid = getpid() & 0xFFFF;
		ping->ttl = 55;
		if (ping->hname)
		{
			icmp_initialize_connection(ping, ping->ttl);
			printf("PING %s (%s): %lu data bytes\n", ping->shost, inet_ntoa(ping->addr.sin_addr), 64 - ICMP_HEADER_SIZE);
			ping->launch(ping);
		}
		destruct_ping(ping);
	}
	return (0);
}
