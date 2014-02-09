CC      = g++
CFLAGS  = -c -Wall -std=c++11
LIBS    = -lboost_system -lpthread -lcrypto
OBJS    = main.o session.o websocketTools.o base64.o

all: server

server: $(OBJS)
	$(CC) $(LIBS) $(OBJS) -o server

main.o: main.cpp session.h	
	$(CC) $(CFLAGS) main.cpp

session.o: websocketTools.h session.h session.cpp
	$(CC) $(CFLAGS) session.cpp

websocketTools.o: base64.h websocketTools.h websocketTools.cpp
	$(CC) $(CFLAGS) websocketTools.cpp

base64.o: base64.h base64.cpp
	$(CC) $(CFLAGS) base64.cpp

clean:
	rm -rf *o server