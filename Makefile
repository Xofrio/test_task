# Stuff

CC			= gcc
CFLAGS		= -std=c11 -pthread
PROJECT		= t

.PHONY: all clean

all: ${PROJECT}

${PROJECT}: main.c process.c recommend.c
	${CC} ${CFLAGS} -o $@ $^

clean:
	rm *.o ${PROJECT}
