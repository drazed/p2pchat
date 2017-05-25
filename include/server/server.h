#ifndef SERVER_H
#define SERVER_H

#include "../sockets/ServerSocket.h"
#include "../sockets/ClientSocket.h"
#include "../sockets/SocketException.h"
#include <unistd.h>
#include <iostream>
#include <string>
#include <pthread.h>
#include <list>

#define SOCKET_PORT 42352
 
using namespace sockets;

class Server
{
private:
    std::list<ServerSocket*> connected_sock_list;
    std::list<pthread_t*> connected_thread_list;
    pthread_t *t_server;
    pthread_mutex_t *server_mutex;

public:
    Server() 
    {
        server_mutex = new pthread_mutex_t;
        pthread_mutex_init(server_mutex,NULL);
    }
    ~Server();
    const Server& operator << (const std::string&);

    std::list<pthread_t*> get_thread_list() { return connected_thread_list; }

    pthread_mutex_t *get_server_mutex() 
    { 
        return server_mutex; 
    }
    void add_to_list(ServerSocket* sock)
    { 
        connected_sock_list.push_front(sock); 
    }
    ServerSocket *get_newest_socket()
    { 
        return connected_sock_list.front(); 
    }
    void add_sock_to_list(ServerSocket *sock) 
    { 
        connected_sock_list.push_front(sock); 
    }
    pthread_t *run_server();
};

#endif

