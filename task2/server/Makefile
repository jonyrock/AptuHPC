CC      = g++
CFLAGS  = -c -Wall -std=c++11
LIBS    = -lboost_system -lpthread -lcrypto
OBJS    = main.o game.o server.o session.o websocketTools.o base64.o

all: server

server: $(OBJS)
	$(CC) $(LIBS) $(OBJS) -o server

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

game.o: game.h game.cpp server.o
	$(CC) $(CFLAGS) game.cpp

server.o: server.h session.o
	$(CC) $(CFLAGS) server.cpp

session.o: session.h websocketTools.o
	$(CC) $(CFLAGS) session.cpp

websocketTools.o: base64.o websocketTools.h websocketTools.cpp
	$(CC) $(CFLAGS) websocketTools.cpp

base64.o: base64.h base64.cpp
	$(CC) $(CFLAGS) base64.cpp

clean:
	rm -rf *o server