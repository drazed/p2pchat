#
#   Compile the client
#

CC = g++
CFLAGS = -Wall -g -c
LDFLAGS = `pkg-config --cflags gtk+-2.0 gthread-2.0`

# objects
CLIENT_OBJS = client.o new_client_dialog.o

all: $(CLIENT_OBJS)

client.o: client.cpp client.h
	$(CC) $(CFLAGS) client.cpp $(LDFLAGS)

new_client_dialog.o: new_client_dialog.cpp new_client_dialog.h
	$(CC) $(CFLAGS) new_client_dialog.cpp $(LDFLAGS)

clean:
	rm -f *.o;
