/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp_error_reception.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/30 05:12:36 by jguyet            #+#    #+#             */
/*   Updated: 2017/05/30 05:12:37 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

BOOLEAN			icmp_error_rcvmsg(void)
{
	t_ping *ping;

	ping = singleton_ping();
	ft_fprintf(1, "Request timeout for icmp_seq %d\n", ping->sequence);
	return (false);
}
