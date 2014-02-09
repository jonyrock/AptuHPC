CC      = g++
CFLAGS  = -c -Wall -std=c++11
LIBS    = -lboost_system -lpthread -lcrypto
OBJS    = main.o base64.o session.o handshake.o

all: server

server: $(OBJS)
	$(CC) $(LIBS) $(OBJS) -o server

main.o: main.cpp session.h	
	$(CC) $(CFLAGS) main.cpp

session.o: handshake.h session.h session.cpp 
	$(CC) $(CFLAGS) session.cpp

handshake.o: base64.h handshake.h handshake.cpp
	$(CC) $(CFLAGS) handshake.cpp

base64.o: base64.h base64.cpp
	$(CC) $(CFLAGS) base64.cpp

clean:
	rm -rf *o server