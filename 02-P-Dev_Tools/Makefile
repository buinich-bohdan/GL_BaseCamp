TARGET = main
DEPS:=$(addsuffix .o, $(TARGET))
SRC = main.c

CC=gcc
CFLAGS=-O3 -o

.PHONY: all clean

all: $(TARGET)

clean:
	rm -rf $(TARGET) *.o

$(TARGET): main.o
	$(CC) $(CFLAGS) $(TARGET) $(DEPS)

main.o: main.c
	gcc -c -o $(DEPS) $(SRC)
