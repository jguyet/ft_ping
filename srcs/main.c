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

void		test(t_ping *ping)
{
	int i = 1;
	int sequence = 0;
	t_packet packet;
	struct sockaddr_in r_addr;

	for (;;) {
		long start;
		
		start = get_current_time_millis();
        socklen_t len = sizeof(r_addr);

        
        if (recvfrom(ping->sock, &packet, sizeof(packet), 0, (struct sockaddr*)&r_addr, &len) > 0)
		{
			printf("%d bytes from %s: icmp_seq=%d time=0.%1.3ld ms\n", (int)sizeof(packet.msg), inet_ntoa(ping->addr.sin_addr), sequence, get_current_time_millis() - start);
		}
		else
		{
			printf("Request timeout for icmp_seq %d\n", sequence);
		}
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
			printf("PING %s (%s):\n", ping->shost, inet_ntoa(ping->addr.sin_addr));
			icmp_initialize_connection(ping);
			test(ping);
		}
		destruct_ping(ping);
	}
	return (0);
}
