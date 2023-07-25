NAME     = ircserv
COMPILER = c++
FLAGS    = -std=c++98 -Wall -Wextra -Werror
SRC      = main.cpp Server/Utils.cpp Server/Server.cpp Channels/Channel.cpp \
           Channels/Channel_utils.cpp Server/user.cpp Server/sockets.cpp \
           Server/messages.cpp

OBJ      = $(SRC:.cpp=.o)
INC      =

DEFAULT  = '\033[0m'
GREEN    = '\033[1;32m'
ORANGE   = '\033[1;33m'
GHOST   = '👻'

.PHONY: all clean fclean re

all: compiling $(NAME) compiled

irssi: rm_irssi
	docker run -it --name my-running-irssi -e TERM -u $(id -u):$(id -g) \
	--log-driver=none \
    -v ${HOME}/.irssi:/home/user/.irssi:ro \
    irssi

rm_irssi:
	@docker rm -f my-running-irssi 2> /dev/null || exit 0

%.o: %.cpp $(INC)
	@$(COMPILER) $(FLAGS) -o $@ -c $<
	@printf "\033[1K\rCompiling $<"

$(NAME): $(OBJ)
	@$(COMPILER) $(FLAGS) $(OBJ) -o $(NAME)
	@printf "\033[1K\r$(GREEN)$(GHOST) Ft_IRC is ready! $(GHOST)$(DEFAULT)\n"

clean:
	@rm -f $(OBJ)
	@rm -f */*.out
	@rm -f *.out
	@rm -f */*.gch
	@rm -f *.gch
	@rm -rf *.dSYM
	@rm -rf */*.dSYM
	@find . -name ".DS_Store" -type f -delete
	@printf "$(ORANGE)Cleaning up refuse $(DEFAULT)"
	@printf "\n"

fclean: clean
	@rm -f $(NAME)
	@printf "$(GREEN)All clean! $(DEFAULT)\n"

re: fclean all

compiling:
	@printf "$(ORANGE)Ft_IRC is being compiled... $(DEFAULT)"

compiled:
	@printf "$(GREEN)Ft_IRC is ready! $(DEFAULT)\n"
	@printf "\n"
	@printf "                👻👻👻\033[1;97m THE SPOOKY IRC SERVER CREATED SUCCESSFULLY 👻👻👻\033[0m\n"
	@printf "\n"
