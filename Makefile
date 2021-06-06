OBJDIR := obj
SRCDIR := src
CFLAGS += -Wall -Ofast -flto -march=native -mtune=native -Isrc -Iinclude/
SRC = $(wildcard $(SRCDIR)/*.c)
OBJS += $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))

all: client 

server: bin/aurrasd

client: bin/aurras

bin/aurras: aurras.o stdprs.o
	gcc -g -o "$@" $^

$(OBJS): $(SRC)
	mkdir -p bin obj
	gcc -c $< $(CFLAGS)

stdprs.o: 
	gcc -c src/stdprs.c

clean:
	@rm -f $(OBJDIR)/* tmp/* bin/{aurras,aurrasd}

bin:
test:
	bin/aurras
	bin/aurras status
	bin/aurras transform samples/sample-1.m4a output.m4a alto eco rapidoS
