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
	return (ping);
}

void		destruct_ping(t_ping *ping)
{
	if (ping->shost != NULL)
		ft_strdel(&ping->shost);
	free(ping);
}

BOOLEAN		start_ping(t_ping *ping)
{
	BOOLEAN retry;

	while (1)
	{
		if (F_INCREMENT)
		{
			if (ping->sweepminsize >= ping->sweepmaxsize)
			{

				return (true);
			}
			ping->sweepminsize += ping->sweepincrsize;
		}
		retry = false;
		t_packet_received *packet_r;
		void *packet;

		ping->send++;
		packet = prepare_packet_to_send(ping, ping->sweepminsize);
		ping->start_time = get_current_time_millis();
		if (sendto(ping->sock, packet, sizeof(t_packet) + ping->sweepminsize, MSG_DONTWAIT, (struct sockaddr*)&ping->addr, sizeof(ping->addr)) <= 0)
		{
			printf("ft_ping: error to send\n");
			exit(0);
		}
		packet_r = prepare_packet_receiver(ping, 5000);
		retry = !icmp_handle_message(ping, packet_r);
		destruct_packet_receiver(packet_r);
		ping->sequence++;
		if (retry)
			return (ping->launch(ping));
		ft_sleep(1);
	}
	return (true);
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
		if (!set_flags_values(ping))
			return (0);
		g_breakflag = 0;
		signal(SIGINT, ctrlc);
		signal(SIGTSTP, ctrlz);
		signal(SIGQUIT, ctrlq);
		signal(SIGALRM, sig_alarm);
		if (ping->hname)
		{
			icmp_initialize_connection(ping, ping->ttl);
			if (F_INCREMENT)
				printf("PING %s (%s): (%d ... %d) data bytes\n", ping->shost, inet_ntoa(ping->addr.sin_addr), ping->sweepminsize, ping->sweepmaxsize);
			else
				printf("PING %s (%s): %d data bytes\n", ping->shost, inet_ntoa(ping->addr.sin_addr), ping->sweepminsize);
			ping->launch(ping);
		}
		destruct_ping(ping);
	}
	return (0);
}
