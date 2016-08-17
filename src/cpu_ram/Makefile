CC = g++

SYSTEMC ?= /usr/local/systemc-2.3.1
ARCH ?= linux64

CPPFLAGS = -isystem $(SYSTEMC)/include
CXXFLAGS = -Wall -Wextra -Werror -g
LDFLAGS = -L$(SYSTEMC)/lib-$(ARCH)
LDLIBS = -lsystemc

TARGET = main
OBJ = main.o initiator.o memory.o ram.o

$(TARGET): $(OBJ)

clean:
	rm -f $(TARGET) $(OBJ)
