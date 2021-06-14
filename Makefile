OBJDIR := obj
SRCDIR := src
CFLAGS += -Wall -Ofast -flto -march=native -mtune=native -Isrc -Iinclude/ -g

all: client server

server: bin/aurrasd

client: bin/aurras

bin/aurras : obj/aurras.o obj/stdprs.o obj/primary_functions.o
	gcc -o "$@" $^

bin/aurrasd : obj/aurrasd.o
	gcc -o "$@" $^

obj/aurras.o: src/aurras.c
	gcc -c $< -o "$@"

obj/aurrasd.o: src/aurrasd.c
	gcc -c $< -o "$@"

obj/stdprs.o: src/stdprs.c
	gcc -c $< -o "$@"

obj/primary_functions.o: src/primary_functions.c
	gcc -c $< -o "$@"

clean:
	@rm -f $(OBJDIR)/* tmp/* bin/{aurras,aurrasd}

test:
	bin/aurras samples/sample-1.mp3 tmp/sample-1.mp3
	bin/aurras samples/sample-2.mp3 tmp/sample-2.mp3
