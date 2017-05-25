#include "client.h" 

void send_clicked_wrapper(GtkWidget *widget, gpointer gdata)
{
    ((Client*)gdata)->send_clicked();
}

void *client_thread(void *client)
{
    try
    {
        *(((Client*)client)->get_sock()) << "CONNECT\r\n"; 
        /* set the nickname only if it was provided */
        if(((Client*)client)->get_name()->size())
        {
            *(((Client*)client)->get_sock()) << "NICK " << *(((Client*)client)->get_name()) << "\r\n"; 
        }

        std::string reply;

        while(1)
        {
            try
            {
                *(((Client*)client)->get_sock()) >> reply;
                *((Client*)client) >> reply; 
            }
            catch(SocketException&) {}
        }
    }
    catch ( SocketException& e )
    {
        std::cout << "Exception was caught:" << e.description() << "\n";
    }
    return NULL;
}

Client::Client(std::string *hostname, int hostport, std::string *nickname) : name(nickname)
{ 
    client_mutex = new pthread_mutex_t;
    pthread_mutex_init(client_mutex, NULL);
    try 
    {
        client_socket = new ClientSocket(*hostname, hostport); 

    }
    catch(SocketException& e)
    {
        std::cout << "Exception was caught:" << e.description() << "\n";
    }

    gdk_threads_enter();

    /* setup text_box, this is where our chat will be displayed */
    text_box = gtk_text_view_new();
    gtk_text_view_set_editable((GtkTextView*)text_box, FALSE);

    /* put the text_box into a scrollable window */
    scroll_win = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy((GtkScrolledWindow*)scroll_win, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add((GtkContainer*)scroll_win, text_box);

    /* setup the entry_box, this is where we enter commands */
    text_entry = gtk_entry_new();
    gtk_signal_connect((GtkObject*)text_entry, "activate", GTK_SIGNAL_FUNC(send_clicked_wrapper), this);

    /* setup the send_button */
    send_button = gtk_button_new_with_label("Send");
    gtk_signal_connect((GtkObject*)send_button, "clicked", GTK_SIGNAL_FUNC(send_clicked_wrapper), this);

    /* setup the layout tables */
    client_table = gtk_table_new(1,2,FALSE);
    entry_table = gtk_table_new(2,1,FALSE);

    gtk_table_attach((GtkTable*)client_table, scroll_win, 0, 1, 0, 1, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), 0, 0);
    gtk_table_attach((GtkTable*)client_table, entry_table,0,1,1,2,(GtkAttachOptions)(GTK_EXPAND | GTK_FILL), GTK_SHRINK, 0, 0); 
    gtk_table_attach((GtkTable*)entry_table, text_entry,0,1,0,1,(GtkAttachOptions)(GTK_EXPAND | GTK_FILL), GTK_SHRINK, 0, 0); 
    gtk_table_attach((GtkTable*)entry_table, send_button,1,2,0,1,GTK_SHRINK, GTK_SHRINK, 0, 0); 

    gdk_threads_leave();
}

/* Cleanup */
Client::~Client()
{
    /* kill the client thread */
    pthread_cancel(*t_client);
    pthread_join(*t_client, NULL);

    gdk_threads_enter();
    /* destroy the widgets */
    //gtk_widget_destroy((GtkWidget*)send_button);
    //gtk_widget_destroy((GtkWidget*)text_entry);
    //gtk_widget_destroy((GtkWidget*)text_box);
    //gtk_widget_destroy((GtkWidget*)scroll_win);
    //gtk_widget_destroy((GtkWidget*)entry_table);
    /* since all the widgets are contianed in this table, destroying it should automatically destroy the rest.  TODO check that this is correct */
    gtk_widget_destroy((GtkWidget*)client_table);
    gdk_threads_leave();

    /* clean up everything else */
    pthread_mutex_destroy(client_mutex);
    delete client_mutex;
    delete client_socket;
}

/* send the buffer to the server, clear the entry, and cleanup */
void Client::send_clicked()
{
    gdk_threads_enter();

    std::string *buffer =  new std::string((char*)gtk_entry_get_text((GtkEntry*)text_entry)); 
    /* don't send empty strings */
    if(!buffer->size()) { delete buffer; return; }

    /* send to server */
    *this << *buffer; 

    gtk_entry_set_text((GtkEntry*)text_entry, "");

    gdk_threads_leave();
    delete buffer;
}

/* run the client thread */
pthread_t *Client::runThread()
{
    t_client = new pthread_t;
    pthread_create(t_client, 0, client_thread, this);
    return t_client;
}

const Client& Client::operator << (const std::string& s) const
{
    try
    {
        *client_socket << s << "\r\n";
    }
    catch(SocketException&) { }

    return *this;
}

const Client& Client::operator >> (std::string& s) const
{
    /* we need to lock so that the client doesn't refresh while we update the
     * buffer 
     */
    pthread_mutex_lock(client_mutex);
    gdk_threads_enter();
    GtkTextIter mend;
    gchar *strbuff = (gchar*)s.c_str();
    GtkTextBuffer *text_buff = gtk_text_view_get_buffer((GtkTextView*)text_box);
    gtk_text_buffer_get_end_iter(text_buff,&mend);
    gtk_text_buffer_insert(text_buff, &mend, strbuff, std::strlen(strbuff));

    /* scroll to the bottom of the text_box */
    GtkTextMark *last_pos;
    gtk_text_buffer_get_end_iter(text_buff,&mend);
    last_pos = gtk_text_buffer_create_mark(text_buff, "last_pos", &mend, FALSE);
    gtk_text_view_scroll_mark_onscreen((GtkTextView*)text_box, last_pos);
    gdk_threads_leave();
    pthread_mutex_unlock(client_mutex);

    return *this;
}

void Client::show_widgets()
{
    gtk_widget_show(scroll_win);
    gtk_widget_show(text_box);
    gtk_widget_show(text_entry);
    gtk_widget_show(send_button);
    gtk_widget_show(client_table);
    gtk_widget_show(entry_table);
}

void Client::hide_widgets()
{
    gtk_widget_hide(scroll_win);
    gtk_widget_hide(text_box);
    gtk_widget_hide(text_entry);
    gtk_widget_hide(send_button);
    gtk_widget_hide(client_table);
    gtk_widget_hide(entry_table);
}

void Client::refresh()
{
    /* no refresh while other threads are updating widgets */
    pthread_mutex_lock(client_mutex);
    gtk_widget_queue_draw(text_box);
    pthread_mutex_unlock(client_mutex);
}
