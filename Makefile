# Makefile for the client and server
#

CC = g++
CFLAGS = -Wall -g
LDFLAGS = `pkg-config --cflags gtk+-2.0 gthread-2.0`


# objects
SUBDIRS = include
SOCKET = include/sockets/Socket.o
SERVER_SOCKET = include/sockets/ServerSocket.o
CLIENT_SOCKET = include/sockets/ClientSocket.o
SERVER = include/server/server.o
CLIENT = include/client/client.o include/client/new_client_dialog.o

CLIENTNODE_OBJS = $(SOCKET) $(SERVER_SOCKET) $(CLIENT_SOCKET) $(SERVER) $(CLIENT) main.o

all: includes chat 

includes:
	@set -e; for i in $(SUBDIRS); do (cd $$i; make) || exit 1; done

chat: $(CLIENTNODE_OBJS) 
	$(CC) $(CFLAGS) -o chat $(CLIENTNODE_OBJS) -lpthread `pkg-config --libs gtk+-2.0 gthread-2.0`

main.o: $(SERVER) $(CLIENT) main.cpp
	$(CC) $(CFLAGS) -c main.cpp $(LDFLAGS)

clean_subdirs:
	@set -e; for i in $(SUBDIRS); do (cd $$i; make clean) || exit 1; done

clean: clean_subdirs
	rm -f *.o chat; 
