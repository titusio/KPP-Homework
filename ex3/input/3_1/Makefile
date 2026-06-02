CC = gcc
CFLAGS = -O3 -Wall -Wextra -std=gnu11

OBJ := tpool.o
SEQ_SRC := mmul_seq.c
SEQ_BIN := mmul_seq
THREAD1_SRC := mmul_thread1.c
THREAD1_BIN := mmul_thread1
THREAD2_SRC := mmul_thread2.c
THREAD2_BIN := mmul_thread2
VALIDATE_SRC := validate.c
VALIDATE_BIN := validate

.PHONY: all clean

all: $(OBJ) $(SEQ_BIN) $(THREAD1_BIN) $(THREAD2_BIN) $(VALIDATE_BIN)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

$(SEQ_BIN): $(SEQ_SRC)
	$(CC) $(CFLAGS) $(SEQ_SRC) -o $@

$(THREAD1_BIN): $(THREAD1_SRC) $(OBJ)
	$(CC) $(CFLAGS) $(THREAD1_SRC) $(OBJ) -o $@ -lpthread

$(THREAD2_BIN): $(THREAD2_SRC) $(OBJ)
	$(CC) $(CFLAGS) $(THREAD2_SRC) $(OBJ) -o $@ -lpthread

$(VALIDATE_BIN): $(VALIDATE_SRC)
	$(CC) $(CFLAGS) $(VALIDATE_SRC) -o $@ -lm

clean:
	rm -f $(OBJ) $(SEQ_BIN) $(THREAD1_BIN) $(THREAD2_BIN) $(VALIDATE_BIN)
