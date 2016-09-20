CC=clang
CFLAGS=-g -c -Wall
LDFLAGS=
SOURCES=list.c lexer.c parser.c ast.c kuma.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=lexer

TEST_SOURCES=test/list_test.c
TEST_OBJECTS=$(TEST_SOURCES:.c=.o)
TEST_EXECUTABLES=$(TEST_OBJECTS:.o=)

all: $(SOURCES) $(EXECUTABLE)

test: $(TEST_EXECUTABLES)

$(TEST_EXECUTABLES): $(TEST_OBJECTS)
	$(CC) $(LDFLAGS) $(TEST_OBJECTS) -o $@

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm lexer $(OBJECTS)

run: all
	cat test3.ks | ./lexer
