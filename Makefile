CC = gcc

CFLAGS = -Wall

LFLAGS =

TARGET = cman

all: cman
	@echo "make all finished!"

cman: main.c src/*.h src/*.c
	@echo "Building 'cman'..."
	$(CC) $(CFLAGS) main.c src/*.c -o $(TARGET) $(LFLAGS)

clean:
	@rm -rf $(TARGET) *.o *.a *.out test_*
