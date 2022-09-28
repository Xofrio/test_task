CC			= gcc
SFLAGS		= -std=c11
TFLAGS		= -pthread
MFLAGS		= -lm
OFLAG1		= -O2
OFLAG2		= -ftree-vectorize
WFLAGS		= -Wno-unused-result
OFLAGS		= ${OFLAG1} ${OFLAG2}
CFLAGS		= ${SFLAGS} ${TFLAGS} ${MFLAGS} ${OFLAGS} ${WFLAGS}
PROJECT		= helicopta

.PHONY: all clean

all: ${PROJECT}

${PROJECT}: main.c process.c recommend.c
	${CC} -o $@ $^ ${CFLAGS}

clean:
	rm ${PROJECT}
