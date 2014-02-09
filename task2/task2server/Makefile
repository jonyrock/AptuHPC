CC = g++
CFLAGS = -c -Wall
LIBS = -lboost_system -lpthread

all: server

server: main.o
	$(CC) $(LIBS) main.o -o server

main.o: main.cpp web_socket_connection.h
	$(CC) $(CFLAGS) main.cpp

clean:
	rm -rf *o server