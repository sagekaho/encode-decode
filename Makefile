CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -g
OBJECTS =  huffman.o stack.o 

.PHONY: all encode decode
all: encode decode

encode: $(OBJECTS) queue.o encode.o
	$(CC) $(CFLAGS) $(OBJECTS) queue.o encode.o -o encode

decode: $(OBJECTS) decode.o
	$(CC) $(CFLAGS) $(OBJECTS) decode.o -o decode

encode.o: encode.c
	$(CC) $(CFLAGS) -c encode.c

decode.o: decode.c
	$(CC) $(CFLAGS) -c decode.c

queue.o: queue.c
	$(CC) $(CFLAGS) -c queue.c

huffman.o: huffman.c
	$(CC) $(CFLAGS) -c huffman.c

stack.o: stack.c
	$(CC) $(CFLAGS) -c stack.c

.PHONY: clean
clean:
	rm -f $(OBJECTS) decode queue.o decode.o encode decode encode.o
