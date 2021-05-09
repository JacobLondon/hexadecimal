TARGET=hd
CC=gcc
FILES=\
	hd.c
CFLAGS=\
	-Wall \
	-Wextra \
	-lm
INSTALLDIR=/usr/bin

.PHONY: $(TARGET) all clean install uninstall

all: $(TARGET)

$(TARGET):
	$(CC) -o $@ $(FILES) $(CFLAGS)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	cp -f $(TARGET) $(INSTALLDIR)

uninstall:
	rm -f $(INSTALLDIR)/$(TARGET)

