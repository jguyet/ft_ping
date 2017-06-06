/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_str_inet_addr.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/06/06 04:04:34 by jguyet            #+#    #+#             */
/*   Updated: 2017/06/06 04:04:35 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

char *get_hostname_ipv4(struct in_addr *addr)
{
	char ipstr[INET6_ADDRSTRLEN];

	inet_ntop(PROT_INTERNET_IPV4, addr, ipstr, sizeof(ipstr));

	return (ft_strdup(ipstr));
}

char *get_hostname_ipv6(struct in6_addr *addr)
{
	char ipstr[INET6_ADDRSTRLEN];

	inet_ntop(PROT_INTERNET_IPV6, addr, ipstr, sizeof(ipstr));

	return (ft_strdup(ipstr));
}

struct sockaddr_in *get_sockaddr_in_ipv4(char *host)
{
	struct	addrinfo hints;
	struct	addrinfo *res;
	struct	sockaddr_in *ipv4;
	char	ipstr[INET6_ADDRSTRLEN];

	ft_memset(&hints, 0, sizeof(hints));
	hints.ai_family = PROT_INTERNET_IPV4;
	hints.ai_socktype = SOCK_DGRAM;
	if (getaddrinfo(host, NULL, &hints, &res) != 0)
		return (NULL);
	while (res)
	{
		if (res->ai_family != PROT_INTERNET_IPV4)
		{
			res = res->ai_next;
			continue ;
		}
		ipv4 = (struct sockaddr_in*)res->ai_addr;
		inet_ntop(PROT_INTERNET_IPV4, &(ipv4->sin_addr), ipstr, sizeof(ipstr));

		if (ipstr[0] != 0)
			return (ipv4);
		res = res->ai_next;
	}
	return (NULL);
}
