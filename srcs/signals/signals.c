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
	if (i == SIGINT)
	{
		ping_result();
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

void	ft_sleep(int t)
{
	alarm(t);
	while(g_breakflag == 0)
		;
	g_breakflag = 0;
}