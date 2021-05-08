TARGET=hd
CC=gcc
FILES=\
	hd.c
CFLAGS=\
	-Wall \
	-Wextra
INSTALLDIR=/usr/bin

.PHONY: clean install uninstall

all: $(TARGET)

$(TARGET):
	$(CC) -o $@ $(FILES) $(CFLAGS)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	cp -f $(TARGET) $(INSTALLDIR)

uninstall:
	rm -f $(INSTALLDIR)/$(TARGET)

