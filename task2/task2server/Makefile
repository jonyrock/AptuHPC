CC = g++
CFLAGS = -c -Wall
LIBS = -lboost_system -lpthread

all: server

server: main.o
	$(CC) $(LIBS) main.o -o server

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

clean:
	rm -rf *o server