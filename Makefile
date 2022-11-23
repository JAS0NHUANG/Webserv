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
INCS		=	-Iincs
RMFLAGS		=	-rf

ifeq ($(VERBOSE), true)
	CFLAGS	+= -DVERBOSE
endif

# **************************************************************************** #
#       SOURCES                                                                #
# **************************************************************************** #
SRCS		=	main.cpp \
				srcs/config/Location.cpp \
				srcs/config/Config.cpp \
				srcs/config/configParsing.cpp \
				srcs/config/utilsParsing.cpp \
				srcs/config/saveFile.cpp \
				srcs/config/doParsing.cpp \
				srcs/socket/Socket.cpp \
				srcs/server/run_server.cpp \
				srcs/utils/utils.cpp \
				srcs/client/Client.cpp \
				srcs/client/Client_recv.cpp \
				srcs/client/Client_send.cpp \
				Response.cpp 

# **************************************************************************** #
#       RULES                                                                  #
# **************************************************************************** #
OBJS		=	$(SRCS:.cpp=.o)

%.o			:	%.cpp
				$(CPP) $(CFLAGS) $(INCS) -c $< -o $@

$(NAME)		:	$(OBJS)
				$(CPP) $(CFLAGS) $(INCS) -o $@ $(OBJS)

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
