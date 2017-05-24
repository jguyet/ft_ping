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

t_packet_received		*prepare_packet_receiver(size_t size)
{
	t_packet_received		*packet;
	struct iovec			iov[1];
	char					*buffer;
	struct sockaddr_storage	src_addr;

	if (!(packet= (t_packet_received*)malloc(sizeof(t_packet_received))))
		return (NULL);
	if (!(buffer = ft_strnew(size)))
		return (NULL);
	iov[0].iov_base = buffer;
	iov[0].iov_len = sizeof(buffer);
	packet->header.msg_name = &src_addr;
	packet->header.msg_namelen = sizeof(src_addr);
	packet->header.msg_iov = iov;
	packet->header.msg_iovlen = 1;
	packet->header.msg_control = 0;
	packet->header.msg_controllen = 0;
	return (packet);
}