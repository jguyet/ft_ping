/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp_handler.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/30 05:09:41 by jguyet            #+#    #+#             */
/*   Updated: 2017/05/30 05:09:42 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

/*
** read le message icmp header-packet
*/
BOOLEAN			icmp_handle_message(t_ping *ping, t_packet_received *packet)
{
	int	ret;

	if ((ret = recvmsg(ping->sock, &packet->header, 0)) != -1)
	{
		icmp_process_received_packet(ret, ping, (struct iphdr*)packet->header.msg_iov[0].iov_base, packet);
		return (true);
	}
	return (icmp_error_rcvmsg());
}


BOOLEAN		icmp_process_received_packet(int readed, t_ping *ping, struct iphdr *ip, t_packet_received *packet)
{
	int time_of;

	time_of = (get_current_time_millis() - ping->start_time);
	(void)packet;
	if ((readed < (int)sizeof(struct iphdr)) || (F_VERBOSE && readed < (int)(ping->sweepminsize + sizeof(struct iphdr) + sizeof(struct icmphdr))))
	{
		ft_fprintf(1, "ft_ping: packet too short (%d bytes) from %s\n", readed, get_hostname_ipv4(&ip->src));
		ping->received++;
		return (true);
	}
	readed -= sizeof(struct iphdr);
	if (readed != -1)
	{
		printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",\
			readed, get_hostname_ipv4(&ip->src), ping->sequence, ip->ttl, ((float)(time_of) / 1000));
		ping->totaltime += time_of;
		if (ping->mintime == 0 || ping->mintime > time_of)
			ping->mintime = time_of;
		if (ping->maxtime == 0 || ping->maxtime < time_of)
			ping->maxtime = time_of;
		ping->received++;
#ifdef __APPLE__
		if ((int)(readed - ICMP_HEADER_SIZE) < ping->sweepminsize)
			printf("wrong total length %d instead of %d\n", readed, (int)(ping->sweepminsize + ICMP_HEADER_SIZE + sizeof(struct iphdr)));
#endif
	}
	else
		printf("Request timeout for icmp_seq %d\n", ping->sequence);
	return (true);
}