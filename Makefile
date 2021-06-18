TARGET=hd
CC=g++
CFLAGS=\
	-Wall \
	-Wextra \
	-ggdb \
	-lm

ifdef PREFIX
MYPREFIX=$(PREFIX)
else
MYPREFIX=/usr/local
endif
FILES=*.cpp

.PHONY: all $(TARGET) clean install uninstall

all: $(TARGET)

$(TARGET):
	$(CC) -o $@ $(FILES) $(CFLAGS)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	cp -f $(TARGET) $(MYPREFIX)/bin/

uninstall:
	rm -f $(MYPREFIX)/bin/$(TARGET)
