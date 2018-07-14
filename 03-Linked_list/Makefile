TARGET = main
DEPS = list
DEPS:=$(addsuffix .o, $(DEPS))
SRC = main.c


CC=gcc
CFLAGS=-O0

.PHONY: all clean

all:	clean | $(TARGET)	## clean & build ordinary(dynamic) executable

%.o: %.c
	$(CC) $(CFLAGS) -c $<

$(TARGET): $(DEPS)		## build ordinary(dynamic) executable
	$(CC) $(CFLAGS) $(DEPS) $(addsuffix .c, $(TARGET)) -o $@

clean:
	rm -rf $(TARGET) *.o
