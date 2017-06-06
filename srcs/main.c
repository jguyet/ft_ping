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
		signal(SIGALRM, sig_alarm);
		if (ping->destip)
		{
			icmp_initialize_connection(ping, ping->ttl);
			ping->launch(ping);
		}
		else
		{
			ft_fprintf(1, "ft_ping: cannot resolve %s: Unknow host\n", ping->shost);
		}
		destruct_ping(ping);
	}
	return (0);
}
