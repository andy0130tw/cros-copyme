CC        ?= gcc
TARGET     = copyme
GTK_CFLAGS = $(shell pkg-config --cflags gtk+-3.0)
GTK_LIBS   = $(shell pkg-config --libs gtk+-3.0)

CFLAGS=-g -Wall $(GTK_CFLAGS)
LDLIBS=$(GTK_LIBS)

all: $(TARGET)

clean:
	rm -f $(TARGET)
