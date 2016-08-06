CC=clang
CFLAGS=-g -c -Wall
LDFLAGS=
SOURCES=lexer.c parser.c ast.c kuma.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=lexer

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm lexer $(OBJECTS)

test: all
	cat test3.ks | ./lexer
