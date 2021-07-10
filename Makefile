NAME	=	philo

# -------  ADD -Werror ! -------

FLAG	= 	-Wall -Wextra -I ./inc
FLAG 	+=	-g -ggdb -fsanitize=address
SRCS	=	src/philo.c src/utils.c

CC		=	gcc

OBJS	=	$(SRCS:.c=.o)

all		:	$(NAME)

$(NAME)	:	$(OBJS) 
			$(CC) $(FLAG) -o $(NAME) $(OBJS) -pthread

%.o		:	%.c
			$(CC) $(FLAG) -o $@ -c $<  

clean	:
			rm -f $(OBJS) 

fclean	:	clean 
			rm -f $(NAME)

re		:	fclean all
