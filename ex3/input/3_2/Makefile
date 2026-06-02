CC=gcc
CFLAGS=-g 
LDFLAGS=-lm

all: prime-sieve-sequential prime-sieve-pthread prime-sieve-openmp

prime-sieve-sequential: prime-sieve-openmp.c
	${CC} ${CFLAGS} -o prime-sieve-sequential prime-sieve-openmp.c ${LDFLAGS}

prime-sieve-pthread: prime-sieve-pthread.c
	${CC} ${CFLAGS} -lpthread -o prime-sieve-pthread prime-sieve-pthread.c ${LDFLAGS}

prime-sieve-openmp: prime-sieve-openmp.c
	${CC} ${CFLAGS} -o prime-sieve-openmp prime-sieve-openmp.c ${LDFLAGS}

clean:
	rm -f prime-sieve-sequential prime-sieve-pthread prime-sieve-openmp
