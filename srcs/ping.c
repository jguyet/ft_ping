/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/06/06 01:48:05 by jguyet            #+#    #+#             */
/*   Updated: 2017/06/06 01:48:06 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

t_ping		*singleton_ping(void)
{
	static t_ping	*ping = NULL;

	if (ping != NULL)
		return (ping);
	if (!(ping = (t_ping*)malloc(sizeof(t_ping))))
		return (NULL);
	ping->sequence = 0;
	ping->shost = NULL;
	ping->destip = NULL;
	ping->port = -1;
	ping->launch = process_icmp_request;
	ping->received = 0;
	ping->send = 0;
	ping->timeout.tv_sec = 1;
	ping->pid = getpid() & 0xFFFF;
	ping->ttl = 64;
	ping->sweepincrsize = 0;
	ping->sweepminsize = PACKET_X64;
	ping->sweepmaxsize = PACKET_X64;
	ping->mintime = 0;
	ping->totaltime = 0;
	ping->maxtime = 0;
	ping->retry = true;
	ft_bzero(&ping->addr, sizeof(ping->addr));
	return (ping);
}

void		destruct_ping(t_ping *ping)
{
	if (ping->shost != NULL)
		ft_strdel(&ping->shost);
	free(ping);
}

BOOLEAN		send_icmp_message(t_ping *ping)
{
	void *packet;

	ping->send++;
	packet = prepare_packet_to_send(ping, ping->sweepminsize);
	ping->start_time = get_current_time_millis();
	if (sendto(ping->sock, packet, sizeof(t_packet) + ping->sweepminsize, MSG_DONTWAIT, (struct sockaddr*)&ping->addr, sizeof(ping->addr)) <= 0)
	{
		printf("ft_ping: error to send\n");
		exit(0);
		return (false);
	}
	return (true);
}

BOOLEAN		wait_icmp_response(t_ping *ping)
{
	t_packet_received *packet_r;

	packet_r = prepare_packet_receiver(ping, 4096);
	ping->retry = !icmp_handle_message(ping, packet_r);
	destruct_packet_receiver(packet_r);
	return (true);
}

BOOLEAN		process_icmp_request(t_ping *ping)
{
	if (F_INCREMENT)
		printf("PING %s (%s): (%d ... %d) data bytes\n", ping->shost, ping->destip, ping->sweepminsize, ping->sweepmaxsize);
	else
		printf("PING %s (%s): %d data bytes\n", ping->shost, ping->destip, ping->sweepminsize);
	while (1)
	{
		ping->retry = false;
		send_icmp_message(ping);
		wait_icmp_response(ping);
		ping->sequence++;
		if (F_INCREMENT)
		{
			if (ping->sweepminsize >= ping->sweepmaxsize)
			{
				ping_result();
				return (true);
			}
			ping->sweepminsize += ping->sweepincrsize;
		}
		if (ping->retry)
			return (ping->launch(ping));
		ft_sleep(1);
	}
	return (true);
}