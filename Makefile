TARGET=hd
CXX=g++
CXXFLAGS=\
	-Wall -Wextra \
	-Wno-switch \
	-Wno-ignored-qualifiers \
	-std=c++11 \
	-march=native \
	-pipe


found = False
ifeq ($(OS),Windows_NT)
	found = True
	UNAME_S = Windows
else
	UNAME_S = $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		found = True
		CXXFLAGS += \
			-lm \
			-Wno-cast-function-type
	endif
	ifeq ($(UNAME_S),Darwin)
		found = True
	endif
endif

default_target = $(TARGET)
ifeq ($(found),False)
	default_target = abort
endif

ifdef PREFIX
MYPREFIX=$(PREFIX)
else
MYPREFIX=/usr/local
endif

MYOBJS=util.o hd.o

.PHONY: clean install uninstall

debug: CXXFLAGS += -ggdb -O0
debug: $(default_target)
release: CXXFLAGS += -O2
release: $(default_target)

MID_OBJS=rpn_include.o
$(MID_OBJS): rpn.cc

$(TARGET): $(MYOBJS) $(MID_OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	rm -f $(TARGET) *.o a.out hd.exe hd

install: $(default_target)
	cp -f $(TARGET) $(MYPREFIX)/bin/

uninstall:
	rm -f $(MYPREFIX)/bin/$(TARGET)

abort:
	@echo ERROR: Your OS isn't supported
	@false
