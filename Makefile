NAME	= webserv

SRC		= main.cpp

OBJ     = $(SRC:.cpp=.o)

CC      = c++
FLAGS   = -Wall -Wextra -Werror -std=c++98
RM      = rm -rf

# Colors
GREEN   = \e[92;5;118m
YELLOW  = \e[93;5;226m
GRAY    = \e[33;2;37m
RESET   = \e[0m
CURSIVE = \e[33;3m

.PHONY: all clean fclean re

%.o: %.cpp
	@$(CC) $(FLAGS) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJ)
	@printf "$(CURSIVE)$(GRAY) 	- Compiling $(NAME)... $(RESET)\n"
	@$(CC) $(FLAGS) $(OBJ) -o $(NAME)
	@printf "$(GREEN) - Executable ready.\n$(RESET)"

clean:
	@$(RM) $(OBJ)
	@printf "$(YELLOW) - Object files removed.$(RESET)\n"

fclean: clean
	@$(RM) $(NAME)
	@printf "$(YELLOW) - Executable removed.$(RESET)\n"

re: fclean all
