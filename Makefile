CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -Isrc/include
EXEC = uhttpd

debug: CFLAGS += -g
debug: $(EXEC)

release: CFLAGS += -s -Os
release: $(EXEC)

package: clean release
	chmod 755 $(EXEC)
	mv $(EXEC) deb/$(EXEC)/usr/bin/
	dpkg-deb --root-owner-group --build deb/$(EXEC)

uhttpd: $(addprefix src/, uhttpd.o tcp.o tokenizer.o handle.o http.o)
	$(CC) $(CFLAGS) $^ -o $@

%/uhttpd.o: $(addprefix src/, uhttpd.c) $(addprefix src/include/, tcp.h handle.h)
	$(CC) $(CFLAGS) -c $< -o $@

%/tcp.o: $(addprefix src/, tcp.c) $(addprefix src/include/, tcp.h)
	$(CC) $(CFLAGS) -c $< -o $@

%/tokenizer.o: $(addprefix src/, tokenizer.c) $(addprefix src/include/, tokenizer.h)
	$(CC) $(CFLAGS) -c $< -o $@

%/handle.o: $(addprefix src/, handle.c) $(addprefix src/include/, handle.h tcp.h tokenizer.h darray.h http.h)
	$(CC) $(CFLAGS) -c $< -o $@

%/http.o: $(addprefix src/, http.c) $(addprefix src/include/, http.h)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(EXEC) deb/$(EXEC)/usr/bin/$(EXEC) deb/$(EXEC).deb
