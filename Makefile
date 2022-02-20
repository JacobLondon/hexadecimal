TARGET=hd
CXX=g++
CXXFLAGS=\
	-Wall -Wextra \
	-Wno-switch \
	-Wno-cast-function-type \
	-Wno-ignored-qualifiers \
	-std=c++11 \
	-march=native \
	-pipe \
	-lm

ifdef PREFIX
MYPREFIX=$(PREFIX)
else
MYPREFIX=/usr/local
endif

MYOBJS=util.o token.o hd.o

.PHONY: clean install uninstall

all: $(TARGET)

MID_OBJS=rpn32.o rpn64.o
$(MID_OBJS): rpn.cc

$(TARGET): $(MYOBJS) $(MID_OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	rm -f $(TARGET) *.o a.out hd.exe hd

install: $(TARGET)
	cp -f $(TARGET) $(MYPREFIX)/bin/
	cp -f hexa.py $(MYPREFIX)/bin/
	cp -f hexadecimal $(MYPREFIX)/bin/

uninstall:
	rm -f $(MYPREFIX)/bin/$(TARGET)
	rm -f $(MYPREFIX)/bin/hexa.py
	rm -f $(MYPREFIX)/bin/hexadecimal
