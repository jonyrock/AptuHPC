CC = g++
CFLAGS = -c -Wall
LIBS = -lboost_system -lpthread -lcrypto
OBJS = main.o base64.o session.o

all: server

server: $(OBJS)
	$(CC) $(LIBS) $(OBJS) -o server

main.o: main.cpp session.h
	
	$(CC) $(CFLAGS) main.cpp

session.o: session.cpp session.h
	$(CC) $(CFLAGS) session.cpp

base64.o: base64.cpp
	$(CC) $(CFLAGS) base64.cpp

clean:
	rm -rf *o server