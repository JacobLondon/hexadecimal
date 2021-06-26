TARGET=hd
CXX=g++
CXXFLAGS=\
	-std=c++11 \
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
	$(CXX) -o $@ $(FILES) $(CXXFLAGS)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	cp -f $(TARGET) $(MYPREFIX)/bin/

uninstall:
	rm -f $(MYPREFIX)/bin/$(TARGET)
