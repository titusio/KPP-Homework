CC = gcc
CFLAGS = -O3 -Wall -Wextra -std=gnu11

OBJ := tpool.o
SEQ_SRC := mmul_seq.c
SEQ_BIN := mmul_seq
VALIDATE_SRC := validate.c
VALIDATE_BIN := validate

.PHONY: all clean

all: $(OBJ) $(SEQ_BIN) $(VALIDATE_BIN)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

$(SEQ_BIN): $(SEQ_SRC)
	$(CC) $(CFLAGS) $(SEQ_SRC) -o $@

$(VALIDATE_BIN): $(VALIDATE_SRC)
	$(CC) $(CFLAGS) $(VALIDATE_SRC) -o $@ -lm

clean:
	rm -f $(OBJ) $(SEQ_BIN) $(VALIDATE_BIN)
