CC = g++

SYSTEMC ?= /usr/local/systemc-2.3.1
ARCH = macosx64

CPPFLAGS = -isystem $(SYSTEMC)/include
CXXFLAGS = -Wall -Wextra -Werror -g
LDFLAGS = -L$(SYSTEMC)/lib-$(ARCH)
LDLIBS = -lsystemc

TARGET = hello
OBJ = hello.o

$(TARGET): $(OBJ)

clean:
	rm -f $(TARGET) $(OBJ)
