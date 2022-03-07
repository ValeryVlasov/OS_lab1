NAME	=	archiver

SRC		=	main.c archive.c utils.c

OBJ		=	$(SRC:.c=.o)

HDRS	=	archive.h

CC		=	gcc

CFLAGS	=	#-Wall -Wextra -Werror #-fsanitize=address

.PHONY:	all clean fclean re

all:	$(NAME)

$(NAME):	$(OBJ) Makefile
			$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o:	%.c $(HRDS)
			$(CC) $(CFLAGS) -c $< -o ${<:.c=.o}

clean:
			rm -f *.o

fclean:	clean
			rm -f $(NAME)

re:		clean fclean all