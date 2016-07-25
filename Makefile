CC=clang++
CFLAGS=-g -c -Wall
LDFLAGS=
SOURCES=lexer.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=lexer

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm lexer $(OBJECTS)

test: all
	cat test3.ks | ./lexer
