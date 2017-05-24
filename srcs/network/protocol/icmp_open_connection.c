/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp_open_connection.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/24 02:30:43 by jguyet            #+#    #+#             */
/*   Updated: 2017/05/24 02:30:45 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

#include <arpa/inet.h>

static BOOLEAN		socket_connection_is_estabilised(int fd)
{
	if (fd < 0)
	{
		printf("Erreur : Socket connection has failed.\n");
		return (false);
	}
	return (true);
}



BOOLEAN		icmp_initialize_connection(t_ping *ping)
{
	int val;

	val = 255;
	ping->sock = socket(PROT_INTERNET_IPV4, NETWORK_FLUX, ICMP_PROTOCOL);
	if (!socket_connection_is_estabilised(ping->sock))
		return (false);
	if (setsockopt(ping->sock, 0, IP_TTL, &val, sizeof(val)) != 0)
		return (false);
	ft_bzero(&ping->addr, sizeof(ping->addr));
	ping->addr.sin_family = PROT_INTERNET_IPV4;
	ft_memcpy((char*)&ping->addr.sin_addr.s_addr,\
		(char*) ping->hname->h_addr, ping->hname->h_length);
	ping->addr.sin_port = 0;
	ping->addr.sin_addr.s_addr = *(long*)ping->hname->h_addr;
	return (true);
}
