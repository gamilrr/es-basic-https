# This is the makefile to compile ssl-https-embedded
SRCSDIR = ./sources
SOURCES := $(shell find $(SRCSDIR) -name '*.c')
OBJS := $(SOURCES:%.c=%.o)
INCDIR = -I./includes -I/usr/include -I/usr/local/include
BINARY = embHTTPS

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra $(INCDIR)

LDLIBS =-lcrypto -lssl -lcurl
LDFLAGS =

.PHONY: all clean debug

all: $(BINARY)

debug: CFLAGS += -g -DDEBUG
debug: $(BINARY)

$(BINARY): $(OBJS)
	$(CC) -o $(BINARY) $(OBJS) $(LDLIBS) $(LDFLAGS)

$(SRCSDIR)/%.o: $(SRCSDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BINARY) $(OBJS)

