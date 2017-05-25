# 
#   Compile all the Socket objects
#

CC = g++
CFLAGS = -Wall -g -c
LDFLAGS = -lpthread

# objects
SOCKET_OBJS = Socket.o ServerSocket.o ClientSocket.o 

all: $(SOCKET_OBJS)

Socket.o: Socket.cpp Socket.h
	$(CC) $(CFLAGS) Socket.cpp

ServerSocket.o: ServerSocket.cpp ServerSocket.h
	$(CC) $(CFLAGS) ServerSocket.cpp

ClientSocket.o: ClientSocket.cpp ClientSocket.h
	$(CC) $(CFLAGS) ClientSocket.cpp

clean:
	rm -f *.o;
