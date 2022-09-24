# Stuff

CC			= gcc
SFLAGS		= -std=c11
TFLAGS		= -pthread
CFLAGS		= ${SFLAGS} ${TFLAGS}
PROJECT		= helicopta

.PHONY: all clean

all: ${PROJECT}

${PROJECT}: main.c process.c recommend.c
	${CC} ${CFLAGS} -o $@ $^

clean:
	rm ${PROJECT}
