# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2016/03/25 15:18:38 by jguyet            #+#    #+#              #
#    Updated: 2017/03/19 04:47:40 by jguyet           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	ft_p

PROG1		=	ft_ping

NAMEBASE    =   $(shell basename $(NAME) .a)

LENGTHNAME	=	`printf "%s" $(NAMEBASE) | wc -c`

MAX_COLS	=	$$(echo "$$(tput cols)-20-$(LENGTHNAME)"|bc)

CC			=	gcc

FLAGS		=	-Wall -Wextra -Werror -ggdb

SRCDIR		=

OBJDIR		=	.objs/

INCDIR		=	includes/

LIBFTDIR	=	libft/

INCDIRLIBFT	=	$(LIBFTDIR)/includes/

SRCSPROG1	=	\
				srcs/main.c																	\
				srcs/signals/signals.c														\
				srcs/network/protocol/icmp_open_connection.c

DSRCSPROG1	=	$(addprefix $(SRCDIR), $(SRCSPROG1))

OBJSPROG1	=	$(addprefix $(OBJDIR), $(DSRCSPROG1:.c=.o))

.SILENT:

all:
	if test -f $(PROG1) ; then																\
		echo "make: Nothing to be done for \`all\`.";										\
	else																					\
		$(MAKE) -j $(NAME);																	\
	fi

$(NAME):
	make -C $(LIBFTDIR)
	$(MAKE) $(PROG1)

$(PROG1):	$(OBJDIR) $(OBJSPROG1)
	$(CC) $(FLAGS) -o $(PROG1) $(OBJSPROG1) -L $(LIBFTDIR) -lftprintf
	echo "MAKE   [$(PROG1)]"

$(OBJDIR):
	mkdir -p $(OBJDIR)
	mkdir -p $(dir $(OBJSPROG1))

$(OBJDIR)%.o : $(SRCDIR)%.c | $(OBJDIR)
	$(CC) $(FLAGS) -MMD -c $< -o $@															\
		-I $(INCDIR) -I $(INCDIRLIBFT)
	printf "\r\033[38;5;117m%s%*.*s\033[0m\033[K"											\
	"MAKE   "$(NAMEBASE)" plz wait ..."														\
		$(MAX_COLS) $(MAX_COLS) "($(@)))"

clean:
	if [[ `rm -R $(OBJDIR) &> /dev/null 2>&1; echo $$?` == "0" ]]; then						\
		echo -en "\r\033[38;5;101mCLEAN  "													\
		"[\033[0m$(NAMEBASE)\033[38;5;101m]\033[K";											\
	else																					\
		printf "\r";																		\
	fi
	make -C $(LIBFTDIR) fclean

fclean:		clean
	if [[ `rm $(NAME) &> /dev/null 2>&1; echo $$?` == "0" ]]; then							\
		echo -en "\r\033[38;5;124mFCLEAN "													\
		"[\033[0m$(NAMEBASE)\033[38;5;124m]\033[K";											\
	else																					\
		printf "\r";																		\
	fi
	make -C $(LIBFTDIR) fclean
	rm -rf $(PROG1)
	rm -rf $(OBJDIR)

re:			fclean all

.PHONY:		fclean clean re

-include $(OBJS:.o=.d)

.PHONY: all clean fclean re
