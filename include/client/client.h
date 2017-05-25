#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <string>
#include <pthread.h>
#include <semaphore.h>
#include <gtk/gtk.h>
#include <glib.h>
#include "../sockets/ClientSocket.h"
#include "../sockets/SocketException.h"

using namespace sockets;

class Client
{
private:
    /* socket */
    ClientSocket *client_socket;

    /* gtk widgets */
    GtkWidget *client_table;
    GtkWidget *entry_table;
    GtkWidget *scroll_win;
    GtkWidget *text_box;
    GtkWidget *text_entry;
    GtkWidget *send_button;

    /* threads */
    pthread_t *t_client;
    pthread_mutex_t *client_mutex;

    /* attached info */
    int timeout_id;
    GtkWidget *tab_button;
    std::string *name;

public:
    Client(std::string *, int, std::string *);
   ~Client(); 

    /* get/set functions */
    ClientSocket *get_sock() { return client_socket; }
    GtkWidget *get_table() { return client_table; }
    int get_timeout_id() { return timeout_id; }
    void set_timeout_id(int id) { timeout_id = id; }
    GtkWidget *get_tab_button() { return tab_button; }
    void set_tab_button(GtkWidget *button) { tab_button = button; }
    std::string *get_name() { return name; }

    /* drawing functions */
    void show_widgets();
    void hide_widgets();
    void refresh();

    /* io functions */
    const Client& operator << (const std::string& s) const;
    const Client& operator >> (std::string& s) const;

    /* wrapper functions */
    void send_clicked();
    pthread_t *runThread();
};

#endif

