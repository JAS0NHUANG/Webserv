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
CFLAGS		=	-Wall -Wextra -Werror -std=c++98 -g3
INCS		=	-Iincs
RMFLAGS		=	-rf

ifeq ($(VERBOSE), true)
	CFLAGS	+= -DVERBOSE
endif

# **************************************************************************** #
#       SOURCES                                                                #
# **************************************************************************** #
SRCS		=	srcs/main.cpp \
				srcs/config/Location.cpp \
				srcs/config/Config.cpp \
				srcs/config/config_parsing.cpp \
				srcs/config/utils_parsing.cpp \
				srcs/config/save_file.cpp \
				srcs/config/do_parsing.cpp \
				srcs/socket/Socket.cpp \
				srcs/server/run_server.cpp \
				srcs/utils/utils.cpp \
				srcs/client/Client.cpp \
				srcs/client/Client_recv.cpp \
				srcs/client/Client_post.cpp \
				srcs/response/Response.cpp \
				srcs/response/Cgi.cpp

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
