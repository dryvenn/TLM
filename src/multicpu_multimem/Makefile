CC = g++

SYSTEMC ?= /usr/local/systemc-2.3.1
ARCH ?= linux64

CPPFLAGS = -isystem $(SYSTEMC)/include
CXXFLAGS = -Wall -Wextra -Werror -g
LDFLAGS = -L$(SYSTEMC)/lib-$(ARCH)
LDLIBS = -lsystemc

TARGET = main
OBJ = main.o master.o memory.o cpu.o ram.o bus.o gpio.o

$(TARGET): $(OBJ)

clean:
	rm -f $(TARGET) $(OBJ)
