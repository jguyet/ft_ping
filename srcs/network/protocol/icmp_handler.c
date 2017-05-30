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
		/*if (((struct iphdr*)packet_r->iov[0].iov_base)->pid != getpid())
		{
			printf("PID : %d\n", ((struct iphdr*)packet_r->iov[0].iov_base)->pid);
			printf("MYPID : %d\n", getpid());
			return (wait_message(ping, packet_r));
		}*/
		icmp_process_received_packet(ret, ping, (struct iphdr*)packet->iov[0].iov_base, packet);
		return (true);
	}
	return (icmp_error_rcvmsg());
}


BOOLEAN		icmp_process_received_packet(int readed, t_ping *ping, struct iphdr *ip, t_packet_received *packet)
{
	(void)packet;
	if (readed < ping->datalen + ICMP_MINLEN)
	{
		if (F_VERBOSE)
			ft_fprintf(1, "ft_ping: packet too short (%d bytes) from %s\n", readed, inet_ntoa(ip->src));
		return (true);
	}
	readed -= sizeof(struct iphdr);
	if (readed != -1)
	{
		printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",\
			readed, inet_ntoa(ip->src),\
			ping->sequence, ip->ttl,\
			((float)(get_current_time_millis() - ping->start_time)) / 1000);
		ping->received++;
	}
	else
		printf("Request timeout for icmp_seq %d\n", ping->sequence);
	return (true);
}