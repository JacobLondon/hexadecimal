
CXX = g++
CXXFLAGS = \
	-Wall -Wextra \
	-std=c++11 \

RM_F = rm -rf
TRASH = *.o *.a

HD_T = hd
HD_O = \
./hd.o \
./rpn.o

.PHONY: clean

all: $(HD_T)

$(HD_T): $(HD_O)
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	$(RM_F) $(TRASH) $(HD_T)
