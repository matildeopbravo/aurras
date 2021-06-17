OBJDIR := obj
SRCDIR := src
CFLAGS += -Wall -O0 -g -flto -march=native -mtune=native -Isrc -Iinclude/

all: client server

server: bin/aurrasd

client: bin/aurras

bin/aurras : obj/aurras.o obj/stdprs.o obj/primary_functions.o obj/filtros.o  obj/auxiliary.o
	gcc -o "$@" $^ $(CFLAGS)

bin/aurrasd : obj/aurrasd.o obj/filtros.o obj/auxiliary.o
	gcc -o "$@" $^ $(CFLAGS)

obj/aurras.o: src/aurras.c
	gcc -c $< -o "$@" $(CFLAGS)

obj/aurrasd.o: src/aurrasd.c
	gcc -c $< -o "$@" $(CFLAGS)

obj/stdprs.o: src/stdprs.c
	gcc -c $< -o "$@" $(CFLAGS)

obj/primary_functions.o: src/primary_functions.c
	gcc -c $< -o "$@" $(CFLAGS)

obj/auxiliary.o: src/auxiliary.c
	gcc -c $< -o "$@" $(CFLAGS)

obj/filtros.o: src/filtros.c
	gcc -c $< -o "$@" $(CFLAGS)

clean:
	@rm -f $(OBJDIR)/* tmp/* bin/{aurras,aurrasd}

test:
	bin/aurras samples/sample-1.mp3 tmp/sample-1.mp3
	bin/aurras samples/sample-2.mp3 tmp/sample-2.mp3
