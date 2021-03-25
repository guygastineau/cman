CC = gcc

CFLAGS = -Wall

LFLAGS =

TARGET = cman

all: cman
	@echo "make all finished!"

cman: cargs $(TARGET).c src/*.h src/*.c
	@echo "Building 'cman'..."
	$(CC) -c $(CFLAGS) $(TARGET).c src/*.c
	$(CC) *.o -o $(TARGET) $(LFLAGS)

CARGS_INCLUDE = vendor/cargs/include
CARGS_SRC = vendor/cargs/src

cargs: $(CARGS_INCLUDE) $(CARGS_SRC)/cargs.c
	@echo "Building 'cargs'..."
	$(CC) -c $(CFLAGS) $(CARGS_SRC)/cargs.c -I$(CARGS_INCLUDE)

hex: hex.c
	$(CC) $(CFLAGS) hex.c -o bin/hex

clean:
	@rm -rf *.o *.a *.out test_* cargs.* *.hex project
