NAME		=	webserv

SRCS		=	srcs/Config.cpp \
				srcs/ConfigServer.cpp \
				srcs/Location.cpp


OBJS		=	${SRCS:.cpp=.o}

HEADERS 	=	include/Config.hpp \
				include/ConfigServer.hpp \
				include/Location.hpp

CXX			=	c++

CXXFLAGS	=	-Wall -Wextra -Werror -std=c++98 -I./include

RM			=	rm -f

all:		${NAME}

${NAME}:	${OBJS}
			${CXX} ${OBJS} -o ${NAME}

%.o:		%.cpp ${HEADERS}
			${CXX} ${CXXFLAGS} -c $< -o ${<:.cpp=.o}

clean:
			${RM} ${OBJS}

fclean:		clean
			${RM} ${NAME}

re:			fclean all

.PHONY:		all clean fclean re
