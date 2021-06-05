TARGET=hd
CC=g++
CFLAGS=\
	-Wall \
	-Wextra \
	-ggdb \
	-lm

FILES=*.cpp

.PHONY: clean all $(TARGET)

all: $(TARGET)

$(TARGET):
	$(CC) -o $@ $(FILES)

clean:
	rm -f $(TARGET)
