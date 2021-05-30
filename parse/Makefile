TARGET=hd
CC=g++
FILES=\
	hd.cpp\
	rpn.cpp
CFLAGS=\
	-Wall \
	-Wextra \
	-ggdb

FILES=*.cpp

.PHONY: clean all $(TARGET)

all: $(TARGET)

$(TARGET):
	$(CC) -o $@ $(FILES)

clean:
	rm -f $(TARGET)
