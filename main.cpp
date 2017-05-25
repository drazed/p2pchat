#include <gtk/gtk.h>
#include <glib.h>
#include <iostream>
#include <string>
#include <list>
#include "include/client/new_client_dialog.h"
#include "include/client/client.h"
#include "include/server/server.h"

/* this is a timed refresh to avoid the stresses of gtk+threads :) */
gint timed_refresh(gpointer data);      
gint destroyapp(GtkWidget *, gpointer);
void close_client_button(GtkWidget *widget, gpointer gdata);
void new_client_button(GtkWidget *widget, gpointer gdata);
void client_button(GtkWidget *widget, gpointer gdata);

std::list<Client*> client_list;
std::list<GtkWidget*> tab_button_list;
GtkWidget *window;
GtkWidget *main_table;
Client *current = NULL;

int main(int argc, char *argv[])
{
    GtkWidget *tab_buttons;

    /* gtk is thread aware, but not thread safe.  we must do this */
    g_thread_init(NULL);
    gdk_threads_enter();

    /*--  Initialize GTK --*/
    gtk_init(&argc, &argv);

    Server *server_deamon;
    server_deamon = new Server();
    server_deamon->run_server();
    /* TODO don't sleep to wait for server, much better to use a condition */
    sleep(1);

    /*-- Declare the GTK Widgets used in the program --*/

    main_table = gtk_table_new(1,2,FALSE);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    /* create the initial buttons,  new button and a client button */
    tab_buttons = gtk_table_new(2,1, FALSE);
    GtkWidget *close_button = gtk_button_new_with_label(" x ");
    gtk_signal_connect((GtkObject*)close_button, "clicked", GTK_SIGNAL_FUNC(close_client_button), NULL);
    GtkWidget *new_button = gtk_button_new_with_label("new");
    gtk_signal_connect((GtkObject*)new_button, "clicked", GTK_SIGNAL_FUNC(new_client_button), tab_buttons);
    gtk_table_attach((GtkTable*)tab_buttons, close_button, 0,1,0,1, GTK_SHRINK, GTK_SHRINK, 0, 0);
    gtk_table_attach((GtkTable*)tab_buttons, new_button, 1,2,0,1, GTK_SHRINK, GTK_SHRINK, 0, 0);
    tab_button_list.push_front(close_button);
    tab_button_list.push_front(new_button);

    /* make the close button red and the new button green */
    GdkColor colorRed, colorGreen;

    gdk_color_parse("red", &colorRed);
    gdk_color_parse("green", &colorGreen);

    gtk_widget_modify_base(close_button, GTK_STATE_NORMAL, &colorRed);
    gtk_widget_modify_fg(close_button, GTK_STATE_NORMAL, &colorRed);
    gtk_widget_modify_bg(close_button, GTK_STATE_NORMAL, &colorRed);
    gtk_widget_modify_bg(close_button, GTK_STATE_PRELIGHT, &colorRed);

    gtk_widget_modify_base(new_button, GTK_STATE_NORMAL, &colorGreen);
    gtk_widget_modify_fg(new_button, GTK_STATE_NORMAL, &colorGreen);
    gtk_widget_modify_bg(new_button, GTK_STATE_NORMAL, &colorGreen);
    gtk_widget_modify_bg(new_button, GTK_STATE_PRELIGHT, &colorGreen);

    /* create a client */
    gdk_threads_leave();
    new_client_button(new_button, tab_buttons);
    gdk_threads_enter();

    /* main window table */
    gtk_table_attach((GtkTable*)main_table, tab_buttons, 0,1,1,2, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), GTK_SHRINK, 0, 0);
    
    /* Create the window and objects and layout */
    gtk_signal_connect((GtkObject*)window, "delete_event", GTK_SIGNAL_FUNC(destroyapp), NULL);

    /*-- Add the table to the window --*/
    gtk_container_add((GtkContainer*)window, main_table);

    /*-- Set border to zero so that client takes up the whole window --*/
    gtk_container_border_width((GtkContainer*)window, 0);

    /*-- Set the window to be 640 x 480 pixels --*/
    gtk_window_set_default_size((GtkWindow*)window, 640, 400);

    /*-- Set the window title --*/
    gtk_window_set_title((GtkWindow*)window, "Client Node");

    /*-- Display the widgets --*/
    gtk_widget_show_all(window);

    /*-- Start the GTK event loop --*/
    gtk_main();
    gdk_threads_leave();

     /* delete all the clients */
    for(; client_list.size();)
    {
        Client *tmp = client_list.back();
        client_list.pop_back();

        /* stop the timed_refresh for this client */
        g_source_remove(tmp->get_timeout_id());
        delete tmp;
    }    
    delete server_deamon;

    return 0;
}

/* this refreshes our text_box. must be called from within the main thread */
gint timed_refresh(gpointer data)
{
    Client *cli = (Client*)data;
    if(cli == current)
    {
        cli->refresh();
        return TRUE;
    }
    return FALSE;
}

/*-- This function allows the program to exit properly when the window is closed --*/
gint destroyapp (GtkWidget *widget, gpointer gdata)
{
    g_print ("Quitting...\n");
    gtk_main_quit();
    return (FALSE);
}

void close_client_button(GtkWidget *widget, gpointer gdata)
{
    /* we don't want to close the last windo */
    if(client_list.size() > 1)
    {
        Client *kill_client = current;
        Client *new_current;

        /* remove the client form the list and set current to the next client */
        for(int i = client_list.size(); i > 0; i--)
        {
            Client *tmp = client_list.back();
            client_list.pop_back();
            if(tmp != kill_client)
                client_list.push_front(tmp); 
            else
                new_current = client_list.back();
        }

        /* remove the button */
        gtk_widget_hide(kill_client->get_tab_button());
        gtk_widget_destroy(kill_client->get_tab_button());

        /* put the new client into focus */
        client_button(new_current->get_tab_button(), new_current);
        
        /* remove the dead client */
        g_source_remove(kill_client->get_timeout_id());
        delete kill_client;
    }
}

void new_client_button(GtkWidget *widget, gpointer gdata)
{
    NewClientDialog *nc_dialog = new NewClientDialog((GtkWindow*)window);

    gdk_threads_enter();
    if(nc_dialog->get_accepted())
    {
        /* create a new client and all that */
        Client *client = new Client(nc_dialog->get_host(), nc_dialog->get_port(), nc_dialog->get_name());
        client->runThread();
        g_object_ref(client->get_table()); /* ref to allow safe table detach */
        client_list.push_front(client);
        client->set_timeout_id(g_timeout_add(500, timed_refresh, client)); /* refresh ever 0.5 seconds */

        /* create a new button */
        std::string *name = new std::string("client: ");
        *name += *(nc_dialog->get_name());
        GtkWidget *tab_button = gtk_button_new_with_label(name->c_str());
        client->set_tab_button(tab_button);
        gtk_signal_connect((GtkObject*)tab_button, "clicked", GTK_SIGNAL_FUNC(client_button), client);
        tab_button_list.push_front(tab_button);
        int num_buttons = tab_button_list.size();
        gtk_table_resize((GtkTable*)gdata, num_buttons, 1);
        gtk_table_attach((GtkTable*)gdata, tab_button, num_buttons-1,num_buttons,0,1, GTK_SHRINK, GTK_SHRINK, 0, 0);
        gtk_widget_show(tab_button);

        /* run the client_button func to make the new client focused */
        client_button(tab_button, client);
    }
    gdk_threads_leave();
    delete nc_dialog;
}

void client_button(GtkWidget *widget, gpointer gdata)
{
    /* hide previous client */
    if(current != NULL)
    {
        current->hide_widgets();
        gtk_container_remove((GtkContainer*)main_table,current->get_table());
    }

    /* show button attached clients */
    current = (Client*)gdata;

    /* attach the client to the table */
    gtk_table_attach((GtkTable*)main_table, current->get_table(), 0,1,0,1, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), 0, 0);
    current->show_widgets();
}

