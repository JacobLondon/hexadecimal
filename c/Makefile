
RM_F = rm -rf

CC = gcc
CFLAGS = -Wall -Wextra -lm

TRASH = *.o *.a *.so *.exe

LIB_O = convert.o main.o

TEST_T = test

TARGETS = $(TEST_T)

.PHONY: clean

all: debug
release: CFLAGS += -O2 -DNDEBUG
release: $(TARGETS)
debug: CFLAGS += -O0 -ggdb
debug: $(TARGETS)

clean:
	$(RM_F) $(TRASH)

$(TEST_T): $(LIB_O)
	$(CC) -o $@ $^ $(CFLAGS)

check: $(TEST_T)
	./test
