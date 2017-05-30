/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_result.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/30 09:19:07 by jguyet            #+#    #+#             */
/*   Updated: 2017/05/30 09:19:09 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

static void	primary_stats(t_ping *ping)
{
	int		packets;
	int		received;
	int		loss;

	packets = ping->send;
	received = ping->received;
	loss = 100 * (packets - received) / packets;
	printf("%d  packets transmitted, %d  packets received, %.1f%% packet loss\n", packets, received, ((float)loss));
}

static void	coef_stats(t_ping *ping)
{
	float	mintime;
	float	mediumtime;
	float	maxtime;

	mintime = 0;
	mediumtime = 0;
	maxtime = 0;
	if (ping->mintime != 0)
		mintime = (((float)ping->mintime) / 1000);
	if (ping->received != 0)
		mediumtime = (ping->totaltime / ping->received) == 0 ? 0 : (((float)(ping->totaltime / ping->received)) / 1000);
	if (ping->maxtime != 0)
		maxtime = (((float)ping->maxtime) / 1000);
	printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", mintime, mediumtime, maxtime, maxtime - mintime);
}

void		ping_result(void)
{
	t_ping	*ping;

	ping = singleton_ping();
	printf("\n--- %s ping statistics ---\n", ping->shost);
	primary_stats(ping);
	coef_stats(ping);
	exit(0);
}
