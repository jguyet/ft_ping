/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   flags.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/24 21:26:39 by jguyet            #+#    #+#             */
/*   Updated: 2017/05/24 21:26:41 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

BOOLEAN			has_help(int argc, char **argv)
{
	int i;

	i = 1;
	while (i < argc)
	{
		if (ft_strcmp(argv[i], "--help") == 0)
		{
			printf("ft_ping: illegal option -- -\n");
			return (true);
		}
		i++;
	}
	return (false);
}

BOOLEAN			print_help(t_ping *ping)
{
	int		i;
	BOOLEAN	stack;

	i = 0;
	stack = true;
	printf("usage: ping [-");
	while (i < FLAGS_SIZE)
	{
		if (ping->flags[i]->special && stack == true)
			printf("]");
		if (ping->flags[i]->special)
			stack = false;
		if (stack)
			printf("%s", ping->flags[i]->name);
		else
			printf(" %s", ping->flags[i]->help);
		i++;
	}
	printf("\n");
	return (false);
}

BOOLEAN			activ_flags(t_ping *ping, char *arg)
{
	int		i;
	int		o;

	i = 0;
	if (arg[0] != '-')
		return (true);
	while (i < FLAGS_SIZE)
	{
		o = 1;
		if (ping->flags[i]->special == true)
		{
			i++;
			continue ;
		}
		while (o < (int)ft_strlen(arg))
		{
			if (ft_strncmp(ping->flags[i]->name, arg + o, ft_strlen(ping->flags[i]->name)) == 0)
			{
				ping->flags[i]->actif = true;
				break ;
			}
			o++;
		}
		i++;
	}
	return (true);
}

BOOLEAN			select_value_special_flags(t_ping *ping, int currentid, char *arg, char **argv, int argc)
{
	int		i;

	i = 0;
	if (arg[0] != '-')
		return (false);
	while (i < FLAGS_SIZE)
	{
		if (ping->flags[i]->special == false\
			|| ft_strcmp(ping->flags[i]->name, arg + 1) != 0)
		{
			i++;
			continue ;
		}
		if (currentid >= argc)
			return (false);
		if (ping->flags[i]->type == 1 && !ft_is_string_numeric(argv[currentid + 1]))
		{
			char *error = ft_strjoin("ft_ping: ", ping->flags[i]->error);
			printf(error, argv[currentid + 1]);
			ft_strdel(&error);
			exit(0);
		}
		ping->flags[i]->actif = true;
		ping->flags[i]->value = ft_strdup(argv[currentid + 1]);
		return (true);
		i++;
	}
	return (false);
}

BOOLEAN			load_host(t_ping *ping, char *arg)
{
	if (arg[0] == '-')
		return (false);
	if (ping->shost != NULL)
	{
		print_help(ping);
		exit(0);
	}
	ping->shost = ft_strdup(arg);
	ping->hname = gethostbyname(ping->shost);
	return (true);
}

BOOLEAN			load_flags(t_ping *ping, int argc, char **argv)
{
	int i;

	i = 1;
	if (has_help(argc, argv))
		return (print_help(ping));
	while (i < argc)
	{
		load_host(ping, argv[i]);
		activ_flags(ping, argv[i]);
		if (select_value_special_flags(ping, i, argv[i], argv, argc))
			i++;
		i++;
	}
	return (true);
}

static t_flag	*newflag(t_flag *f)
{
	t_flag *n;

	n = malloc(sizeof(t_flag));
	n->actif = f->actif;
	n->name = ft_strdup(f->name);
	n->special = f->special;
	n->help = ft_strdup(f->help);
	n->value = NULL;
	n->type = f->type;
	n->error = ft_strdup(f->error);
	return (n);
}

void			load_flag_list(t_ping *ping)
{
	ping->flags = malloc(sizeof(t_flag) * FLAGS_SIZE);
	ping->flags[0] = newflag(&(t_flag){false, "v", false, NULL, NULL, 0, NULL});
	ping->flags[1] = newflag(&(t_flag){false, "G", true, "[-G sweepmaxsize]", NULL, 1, "invalid packet size: `%s'"});
	ping->flags[2] = newflag(&(t_flag){false, "g", true, "[-g sweepminsize]", NULL, 1, "invalid packet size: `%s'"});
	ping->flags[3] = newflag(&(t_flag){false, "h", true, "[-h sweepincrsize]", NULL, 1 , "invalid increment size: `%s'"});
	ping->flags[4] = newflag(&(t_flag){false, "m", true, "[-m ttl]", NULL, 1, "invalid TTL: `%s'"});
	ping->flags[5] = newflag(&(t_flag){false, "t", true, "[-t timeout]", NULL, 1, "invalid timeout: `%s'"});
}
