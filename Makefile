NAME = pipex

OBJ_DIR = obj/
SRC_DIR = src/
INC_DIR = include/

SRCS = pipex.c\
	   child.c\
	   clean.c\
	   file.c\
	   parent.c

OBJS = $(SRCS:.c=.o)
OBJS_PREFIXED = $(addprefix $(OBJ_DIR), $(OBJS))

CC = gcc
CFLAGS = -Wall -Werror -Wextra

LIBFT = ./libft/
LIBFT_LIB = $(addprefix $(LIBFT), libft.a)
LIBFT_INC = -I ./libft/include


all: libft $(NAME)

libft:
	@make -C $(LIBFT)

$(OBJ_DIR)%.o : $(SRC_DIR)%.c $(INC_DIR)pipex.h
	@mkdir -p $(OBJ_DIR)
	@echo "Compiling: $<"
	@$(CC) -g $(CFLAGS) -I $(INC_DIR) $(LIBFT_INC) -c -o $@ $<

$(NAME): $(OBJS_PREFIXED)
	@echo "Compiling main executable"
	@$(CC) -g $(CFLAGS) $(OBJS_PREFIXED) $(LIBFT_LIB) -o $(NAME)

clean:
	@rm -rf $(OBJ_DIR)
	@echo "Done cleaning $(CURDIR)/$(OBJ_DIR)"
	@make -C $(LIBFT) clean

fclean: clean
	@rm -f $(NAME)
	@echo "Done cleaning executable $(CURDIR)/$(NAME)"
	@make -C $(LIBFT) fclean

re: fclean all

.PHONY: all clean fclean re libft
