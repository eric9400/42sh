# Makefilei
CC= gcc
CFLAGS= -Wall -Wextra -Werror -std=c99 -pedantic -fsanitize=address -g
LDFLAGS= -lasan
CRI= -lcriterion

SRC= ${shell find . -name '*.c'}
OBJ= ${SRC:.c=.o}
BIN= main

.PHONY: all check clean crit

all: ${OBJ}
	${CC} ${CFLAGS} -o ${BIN} ${OBJ}

check: ${OBJ}
	${CC} ${CFLAGS} ${EFLAGS} ${LDFLAGS} -o ${BIN} ${OBJ}

crit: ${OBJ}
	${CC} ${CFLAGS} ${EFLAGS} ${CRI} ${LDFLAGS} -o ${BIN} ${OBJ}

clean:
	echo "${RM} ${OBJ} ${BIN}"
	rm -I ${OBJ} ${BIN}
# END
