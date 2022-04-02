
RM_F = rm -rf

CC = gcc
CFLAGS = -Wall -Wextra -lm

TRASH = *.o *.a *.so *.exe

LIB_O = convert.o
LIB_C = convert.c

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

$(TEST_T): $(LIB_C)
	$(CC) -o $@ $^ -DCONVERT_TEST $(CFLAGS)

check: $(TEST_T)
	./test
