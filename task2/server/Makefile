CC      = g++
CFLAGS  = -c -Wall -std=c++11
LIBS    = -lboost_system -lpthread -lcrypto
OBJS    = main.o game.o server.o session.o websocketTools.o base64.o

all: server

server: $(OBJS)
	$(CC) $(LIBS) $(OBJS) -o server

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

game.o: game.h igame.h game.cpp server.h
	$(CC) $(CFLAGS) game.cpp

server.o: server.h igame.h session.h server.cpp
	$(CC) $(CFLAGS) server.cpp

session.o: session.h websocketTools.h session.cpp
	$(CC) $(CFLAGS) session.cpp

websocketTools.o: base64.h websocketTools.h websocketTools.cpp
	$(CC) $(CFLAGS) websocketTools.cpp

base64.o: base64.h base64.cpp
	$(CC) $(CFLAGS) base64.cpp

clean:
	rm -rf *o server