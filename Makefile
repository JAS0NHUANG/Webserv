# **************************************************************************** #
#       COMANDS                                                                #
# **************************************************************************** #
CPP			=	c++
RM			=	rm

# **************************************************************************** #
#       TITLE                                                                  #
# **************************************************************************** #
NAME		=	webserv

# **************************************************************************** #
#       FLAGS                                                                  #
# **************************************************************************** #
CFLAGS		=	-Wall -Wextra -Werror -std=c++98
RMFLAGS		=	-rf

ifeq ($(VERBOSE), true)
	CFLAGS	+= -DVERBOSE
endif

# **************************************************************************** #
#       SOURCES                                                                #
# **************************************************************************** #
SRCS		=	main.cpp \
				srcs/config/Location.cpp \
				srcs/config/Server.cpp \
				srcs/config/configParsing.cpp \
				srcs/config/utilsParsing.cpp \
				srcs/config/saveFile.cpp \
				srcs/config/doParsing.cpp \
				srcs/socket/Socket.cpp \
				srcs/epoll/epoll.cpp \
				srcs/utils/utils.cpp \
				srcs/client/Client.cpp \

# **************************************************************************** #
#       RULES                                                                  #
# **************************************************************************** #
OBJS		=	$(SRCS:.cpp=.o)

%.o			:	%.cpp
				$(CPP) $(CFLAGS) -c $< -o $@

$(NAME)		:	$(OBJS)
				$(CPP) $(CFLAGS) -o $@ $(OBJS)

all			:	$(NAME)

clean		:
				$(RM) $(RMFLAGS) $(OBJS)

fclean		:	clean
				$(RM) $(RMFLAGS) $(NAME)

re			:	fclean all

# **************************************************************************** #
#       PHONY                                                                  #
# **************************************************************************** #
.PHONY		:	all test clean fclean re
