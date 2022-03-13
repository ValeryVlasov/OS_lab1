NAME	=	archiver

SRC		=	main.c archive.c utils.c

OBJ		=	$(patsubst %.c, %.o, $(SRC))

HDRS	=	archive.h

CC		=	gcc

CFLAGS	=	-Wall -Wextra -Werror #-fsanitize=address

OPTFLAGS	=	-O2

.PHONY:	all clean fclean re

all:	$(NAME)

$(NAME):	$(OBJ)
			$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o:	%.c $(HRDS) Makefile
		$(CC) $(CFLAGS) -c $< -o $@

clean:
			rm -f *.o

fclean:	clean
			rm -f $(NAME)

re:		fclean all