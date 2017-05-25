# 
# compile the server
#

CC = g++
CFLAGS = -Wall -g -c
LDFLAGS = -lpthread

# objects
SERVER_OBJS = server.o 

all: $(SERVER_OBJS)

server.o: server.cpp server.h
	$(CC) $(CFLAGS) server.cpp

clean:
	rm -f *.o;
