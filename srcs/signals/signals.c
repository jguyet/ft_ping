/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sig.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/03/21 15:38:30 by jguyet            #+#    #+#             */
/*   Updated: 2017/03/21 15:38:32 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "sig.h"
#include "ft_ping.h"

void	ctrlc(int i)
{
	t_ping	*ping;
	int		packets;
	int		received;
	int		loss;

	if (i == SIGINT)
	{
		ping = singleton_ping();
		packets = ping->sequence;
		received = ping->received;
		loss = 100 * (packets - received) / packets;
		printf("\n--- %s ping statistics ---\n", ping->shost);
		printf("%d  packets transmitted, %d  packets received, %.1f%% packet loss\n", packets, received, ((float)loss));

		exit(0);
	}
}

void	ctrlz(int i)
{
	if (i == SIGTSTP)
	{

	}
}

void	ctrlq(int i)
{
	if (i == SIGQUIT)
	{

	}
}

void	sig_alarm(int i)
{
	if (i == SIGALRM)
	{
		g_breakflag = 1;
	}
}