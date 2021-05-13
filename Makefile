OBJDIR := obj
CFLAGS += -Wall -Ofast -flto -march=native -mtune=native -Isrc -Iinclude/

all: server client

server: bin/aurrasd

client: bin/aurras

bin/aurra%: $(OBJDIR)/aurra%.o
	$(CC) -g $^ -o "$@"

$(OBJDIR)/%.o : src/%.c
	mkdir -p bin obj
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	@rm -f $(OBJDIR)/* tmp/* bin/{aurras,aurrasd}

bin:
test:
	bin/aurras
	bin/aurras status
	bin/aurras transform samples/sample-1.m4a output.m4a alto eco rapido
