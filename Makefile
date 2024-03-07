NAME = webserv

SRC =   ./srcs/main.cpp  ./srcs/Server.cpp ./srcs/Client.cpp ./srcs/Config.cpp ./srcs/Webserv.cpp \
	./srcs/Linker.cpp  ./srcs/GET.cpp  ./srcs/Response.cpp ./srcs/POST.cpp ./srcs/Utils.cpp ./srcs/Cgi.cpp ./srcs/DELETE.cpp

OBJ = $(SRC:.cpp=.o)
CXX = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 
RM = rm -f


all: $(NAME)

$(NAME): $(OBJ) 
	 $(CXX) $(CPPFLAGS) $(OBJ) -o $(NAME)


clean:
	 $(RM) $(OBJ)

fclean: clean
	 $(RM) $(NAME)

re: fclean $(NAME)