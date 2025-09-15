CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic
#CFLAGS += -g
CFLAGS += -s -Os
EXEC = uhttpd

all: $(EXEC)

uhttpd: uhttpd.o tcp.o tokenizer.o handle.o http.o
	$(CC) $(CFLAGS) $^ -o $@

uhttpd.o: uhttpd.c tcp.h handle.h
	$(CC) $(CFLAGS) -c $< -o $@

tcp.o: tcp.c tcp.h
	$(CC) $(CFLAGS) -c $< -o $@

tokenizer.o: tokenizer.c tokenizer.h
	$(CC) $(CFLAGS) -c $< -o $@

handle.o: handle.c handle.h tcp.h tokenizer.h darray.h http.h
	$(CC) $(CFLAGS) -c $< -o $@

http.o: http.c http.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(EXEC)
