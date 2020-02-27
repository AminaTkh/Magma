CC=g++
СFLAGS=-c -Wall -O2 

all: magma

magma: main.cpp
		$(CC) $(CFLAGS) main.cpp -o magma
		
clean:
		rm -rf *.o magma