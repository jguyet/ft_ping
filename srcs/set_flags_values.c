/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_flags_values.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/30 08:06:18 by jguyet            #+#    #+#             */
/*   Updated: 2017/05/30 08:06:19 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

static BOOLEAN		print_error(char *error)
{
	ft_fprintf(1, "ft_ping: %s\n", error);
	return (false);
}

static BOOLEAN		print_error_args(char *error, int args)
{
	printf(error, args);
	return (false);
}

BOOLEAN				set_flags_values(t_ping *ping)
{
	if (ping->flags[1]->actif)//"G"
		ping->sweepmaxsize = ft_atoi(ping->flags[1]->value);
	if (ping->flags[2]->actif)//"g"
		ping->sweepminsize = ft_atoi(ping->flags[2]->value);
	if (ping->flags[3]->actif)//"h"
	{
		if (F_SWEEPMIN == false)
			ping->sweepminsize = 0;
		if (F_SWEEPMAX == false)
			return (print_error("Maximum sweep size must be specified"));
		ping->sweepincrsize = ft_atoi(ping->flags[3]->value);
	}
	if (ping->sweepminsize < 0)
		ping->sweepminsize = 0;
	if (ping->sweepmaxsize < ping->sweepminsize)
		return (print_error("Maximum packet size must be greater than the minimum packet size"));
	if (ping->flags[4]->actif)//"ttl"
		ping->ttl = ft_atoi(ping->flags[4]->value);
	if (ping->ttl > 255 || ping->ttl < 0)
		return (print_error_args("ft_ping: invalid TTL: `%d'\n", ping->ttl));
	if (ping->flags[5]->actif)//"timeout"
		ping->timeout.tv_sec = ft_atoi(ping->flags[5]->value);
	if (ping->timeout.tv_sec > 3600)
		return (print_error_args("ft_ping: invalid TTL: `%d' > 3600\n", ping->ttl));
	if (ping->timeout.tv_sec < 1)
		return (print_error_args("ft_ping: invalid TTL: `%d'\n", ping->ttl));
	return (true);
}
