# **************************************************************************** #
#       COMANDS                                                                #
# **************************************************************************** #
CPP			=	c++
RM			=	rm

# **************************************************************************** #
#       TITLE                                                                  #
# **************************************************************************** #
NAME		=	webserv
TEST		=	Webserv_test

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
				BaseConf.cpp \
				ServerConf.cpp \
				configParsing.cpp \
				utilsParsing.cpp \
				saveFile.cpp \
				doParsing.cpp


TEST_SRCS	=	tests/test.cpp \

# **************************************************************************** #
#       RULES                                                                  #
# **************************************************************************** #
OBJS		=	$(SRCS:.cpp=.o)

TEST_OBJS	=	$(TEST_SRCS:.cpp=.o)

%.o			:	%.cpp
				$(CPP) $(CFLAGS) -c $< -o $@

$(NAME)		:	$(OBJS)
				$(CPP) $(CFLAGS) -o $@ $(OBJS)

$(TEST)		:	$(TEST_OBJS)
				$(CPP) $(CFLAGS) -o $@ $(TEST_OBJS)

all			:	$(NAME)

test		:	$(TEST)

clean		:
				$(RM) $(RMFLAGS) $(OBJS)
				$(RM) $(RMFLAGS) $(TEST_OBJS)

fclean		:	clean
				$(RM) $(RMFLAGS) $(NAME)
				$(RM) $(RMFLAGS) $(TEST)

re			:	fclean all

# **************************************************************************** #
#       PHONY                                                                  #
# **************************************************************************** #
.PHONY		:	all test clean fclean re