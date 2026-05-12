CC = $(shell xcrun -f clang)
SDK = $(shell xcrun --sdk macosx --show-sdk-path)
CFLAGS = -std=c11 -isysroot $(SDK)

all: structures-O0 structures-O3 arrays-O0 arrays-O3

structures-O0: structures.c
	$(CC) $(CFLAGS) -O0 structures.c -o structures-O0

structures-O3: structures.c
	$(CC) $(CFLAGS) -O3 structures.c -o structures-O3

arrays-O0: arrays.c
	$(CC) $(CFLAGS) -O0 arrays.c -o arrays-O0

arrays-O3: arrays.c
	$(CC) $(CFLAGS) -O3 arrays.c -o arrays-O3

run: all
	@echo "=== structures -O0 ===" && time ./structures-O0
	@echo "=== structures -O3 ===" && time ./structures-O3
	@echo "=== arrays -O0 ===" && time ./arrays-O0
	@echo "=== arrays -O3 ===" && time ./arrays-O3

clean:
	rm -f structures-O0 structures-O3 arrays-O0 arrays-O3
