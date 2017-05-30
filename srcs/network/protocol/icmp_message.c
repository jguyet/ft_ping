/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp_message.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/24 04:19:25 by jguyet            #+#    #+#             */
/*   Updated: 2017/05/24 04:19:27 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

t_packet_received		*prepare_packet_receiver(t_ping *ping, size_t size)
{
	t_packet_received		*packet;
	char					buffer[size];

	if (!(packet = (t_packet_received*)malloc(sizeof(t_packet_received))))
		return (NULL);
	packet->iov[0].iov_base = &buffer;
	packet->iov[0].iov_len = sizeof(buffer);
	packet->header.msg_name = (void*)&ping->addr;
	packet->header.msg_namelen = sizeof(ping->addr);
	packet->header.msg_iov = packet->iov;
	packet->header.msg_iovlen = 1;
	packet->header.msg_control = 0;
	packet->header.msg_controllen = 0;
	return (packet);
}

#ifdef __linux__
static void				prepare_iphdr(t_packet *packet, t_ping *ping)
{
	packet->ip.src.s_addr = INADDR_ANY;

	if (!(inet_pton(AF_INET, ping->shost, &packet->ip.dest)))
	{
		printf("ft_ping: Can't set destination network address\n");
		exit(EXIT_FAILURE);
	}

	packet->ip.ttl = 64;
	packet->ip.protocol = IPPROTO_ICMP;
	packet->ip.version = 4;//ipv4
	packet->ip.hl = sizeof(struct iphdr) >> 2;
	packet->ip.pid = ping->pid;
	packet->ip.service = 0;
	packet->ip.off = 0;
	packet->ip.len = sizeof(packet);
	packet->ip.checksum = 0;
	packet->ip.checksum = checksum(&packet->ip, sizeof(struct iphdr));
}
#endif

static void				prepare_header(t_packet *packet, t_ping *ping)
{
	packet->header.type = ICMP_ECHO;
	packet->header.un.echo.id = ping->pid;
	packet->header.un.echo.sequence = 1000;
	packet->header.checksum = 0;
}

void		*prepare_packet_to_send(t_ping *ping, size_t size)
{
	t_packet 	*packet;
	char		*pck;

	if (!(packet = (t_packet*)malloc(sizeof(t_packet))))
		return (NULL);
	ft_bzero(packet, sizeof(*packet));
#ifdef __linux__
	prepare_iphdr(packet, ping);
#endif

	prepare_header(packet, ping);

#ifdef __linux__
	pck = ft_strnew(sizeof(struct iphdr) + sizeof(struct icmphdr) + size);
	ft_memcpy(pck, (const void*)&packet.ip, sizeof(struct iphdr));
	ft_memcpy(pck + sizeof(struct icmphdr), (const void*)&packet->header, sizeof(struct icmphdr));
	ft_memset(pck + sizeof(struct iphdr) + sizeof(struct icmphdr), '0', size);
	packet->header.checksum = checksum(pck + sizeof(struct iphdr), sizeof(struct icmphdr) + size);
	ft_memcpy(pck + sizeof(struct icmphdr), (const void*)&packet->header, sizeof(struct icmphdr));
#else
	pck = ft_strnew(sizeof(struct icmphdr) + size);
	ft_memcpy(pck, (const void*)&packet->header, sizeof(struct icmphdr));
	ft_memset(pck + sizeof(struct icmphdr), '0', size);
	packet->header.checksum = checksum(pck, sizeof(struct icmphdr) + size);
	ft_memcpy(pck, (const void*)&packet->header, sizeof(struct icmphdr));
#endif
	return (pck);
}

void					destruct_packet_receiver(t_packet_received *packet)
{
	free(packet);
}

void					destruct_packet_send(t_packet *packet)
{
	free(packet);
}