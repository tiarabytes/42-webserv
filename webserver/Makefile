# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                            :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tching <tching@student.42kl.edu.my>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/01/17 12:02:51 by tching            #+#    #+#              #
#    Updated: 2026/01/18 18:39:22 by tching           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= webserv

INC_PATH	=./inc/
SRC_PATH	=./src/
OBJ_PATH	=./obj/

CC			= c++ -fsanitize=address
CFLAGS		= -Wall -Wextra -Werror -std=c++98

RM			= rm -rf

HEADER		= -I $(INC_PATH)

SRC 		= $(wildcard $(SRC_PATH)*.cpp)
			
OBJ 		= $(addprefix $(OBJ_PATH),$(notdir $(SRC:.cpp=.o)))

$(NAME): 	$(OBJ)
			$(CC) $(CFLAGS) $(OBJ) $(HEADER) -o $(NAME)
			@echo "$(NAME) compiled successfully.\n\n"

$(OBJ_PATH)%.o: $(SRC_PATH)%.cpp
			mkdir -p $(OBJ_PATH)
			$(CC) $(CFLAGS) -c $< -o $@ $(HEADER)
			@echo "Compiled: $< -> $@\n"

all:		$(NAME)

re:			fclean all
			@echo "Rebuilt $(NAME).\n\n"
clean:
			$(RM) $(OBJ) $(OBJ_PATH)
			@echo "Cleaned object files.\n\n"

fclean:		clean
			$(RM) $(NAME)
			@echo "Removed executable $(NAME).\n\n"

.PHONY: all clean fclean re