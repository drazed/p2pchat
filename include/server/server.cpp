#include "server.h"

void *connected_thread(void *server)
{
    Server *serv = (Server*)server;
    pthread_mutex_t *lock_m = serv->get_server_mutex();
    ServerSocket *sock = serv->get_newest_socket();

    /* we want to connect to the main server and forward all from this socket to that socket and back again */
    ClientSocket *c_sock;    
/*    try
    {
        c_sock = new ClientSocket("localhost", 42351);
    }
    catch(SocketException& e)
    {
        std::cout << "Exception was caught:" << e.description() << "\n";
    }
*/
    try
    {
        while (true)
        {
            std::string data;
            *sock >> data;

            /* forward to the main server and back again */
            /* TODO, why does this block??? */
  //          *c_sock << data;
 //           *c_sock >> data;
   //         *sock << data;

            /* send to all nodes on server */
            /* this is for the p2pchat part */
             pthread_mutex_lock(lock_m);
            *serv << data;
            pthread_mutex_unlock(lock_m); 
        }
    }
    catch(SocketException&){ }
    return NULL;
}

void *server_thread(void *serv)
{
    std::cout << "Deamon Running....\n";
    std::list<pthread_t*> connected_thread_list = ((Server*)serv)->get_thread_list();
    try
    {
        // Create the socket
        ServerSocket server(SOCKET_PORT);

        while(true)
	    {
	        ServerSocket *new_sock = new ServerSocket;
    	    server.accept(*new_sock);
            ((Server*)serv)->add_to_list(new_sock);

            pthread_t *t_connect = new pthread_t;
            pthread_create(t_connect, 0, connected_thread, serv);
            connected_thread_list.push_front(t_connect);

        }
    }
    catch(SocketException& e)
    {
        std::cout << "Exception was caught:" << e.description() << "\nExiting Deamon.\n";
    }
    return NULL;
}

Server::~Server()
{
    /* stop all connected threads */
    for(; connected_thread_list.size();)
    {
        pthread_t *next = connected_thread_list.back();
        connected_thread_list.pop_back();
        pthread_cancel(*next);
        pthread_join(*next, NULL);
        delete next;
    }
    /* delete all our sockets */
    for(; connected_sock_list.size();)
    {
        ServerSocket* serv_sock = connected_sock_list.back();
        connected_sock_list.pop_back();
        delete serv_sock; 
    }

    /* stop the server thread */
    pthread_cancel(*t_server);
    pthread_join(*t_server, NULL);
    delete t_server;

    /* destroy the mutex */
    pthread_mutex_destroy(server_mutex);
    delete server_mutex;
}

pthread_t *Server::run_server()
{
    t_server = new pthread_t;
    pthread_create(t_server, 0, server_thread, this);
    return t_server;
}

const Server& Server::operator << (const std::string& s)
{
    try
    {
        for(int size = connected_sock_list.size(); size > 0; size--)
        {
            ServerSocket* serv_sock = connected_sock_list.back();
            connected_sock_list.pop_back();
            *serv_sock << s;
            connected_sock_list.push_front(serv_sock);
        }
    }
    catch(SocketException&) { std::cout << "server send failed\n"; }

    return *this;
}

