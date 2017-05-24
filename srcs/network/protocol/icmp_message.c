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
	//char					*buffer;
	char					buffer[5000];

	(void)size;
	if (!(packet = (t_packet_received*)malloc(sizeof(t_packet_received))))
		return (NULL);
	//if (!(packet->header = (struct msghdr*)malloc(sizeof(struct msghdr))))
	//	return (NULL);
	//if (!(buffer = ft_strnew(size)))
	//	return (NULL);
	packet->iov[0].iov_base = &buffer;
	packet->iov[0].iov_len = sizeof(buffer);
	packet->header.msg_name = (void*)&ping->addr;
	packet->header.msg_namelen = sizeof(ping->addr);
	packet->header.msg_iov = packet->iov;
	packet->header.msg_iovlen = 1;
	packet->header.msg_control = 0;
	packet->header.msg_controllen = 0;
	//packet->msg = &buffer;
	return (packet);
}

void					destruct_packet_receiver(t_packet_received *packet)
{
	//if (packet->msg != NULL)
	//	ft_strdel(&packet->msg);
	free(packet);
}