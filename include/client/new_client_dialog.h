#ifndef NEW_CLIENT_DIALOG_H
#define NEW_CLIENT_DIALOG_H
#include <iostream>
#include <gtk/gtk.h>
#include <glib.h>

#define HOST_NAME_ENTRY "127.0.0.1"
#define HOST_PORT_ENTRY 42352

class NewClientDialog
{
private:
    std::string *host;
    std::string *name;
    gint port;
    gint ret_sig;
    bool accept;
public:
    NewClientDialog(GtkWindow*);
    ~NewClientDialog(); 

    bool get_accepted(){ return accept; }
    std::string *get_host(){ return host; }
    std::string *get_name(){ return name; }
    gint get_port(){ return port; }
};

#endif
