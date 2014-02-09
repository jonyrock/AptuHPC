CC = g++
CFLAGS = -c -Wall
LIBS = -lboost_system -lpthread -lcrypto

all: server

server: main.o
	$(CC) $(LIBS) main.o base64.o -o server

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

base64.o: base64.cpp
	$(CC) $(CFLAGS) base64.cpp

clean:
	rm -rf *o server