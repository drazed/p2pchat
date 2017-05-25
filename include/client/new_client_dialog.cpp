#include "new_client_dialog.h"

NewClientDialog::NewClientDialog(GtkWindow *win) : accept(false)
{
    gdk_threads_enter();

    /* create our widgets */
    GtkWidget *host_label = gtk_label_new("host name:   ");
    GtkWidget *host_entry = gtk_entry_new();
    gtk_entry_set_text((GtkEntry*)host_entry, HOST_NAME_ENTRY);
    GtkWidget *port_label = gtk_label_new("port number: ");
    GtkObject *port_entry = gtk_adjustment_new((gdouble)HOST_PORT_ENTRY,(gdouble)1,(gdouble)65535,(gdouble)1,(gdouble)1,(gdouble)1);
    GtkWidget *port_spin = gtk_spin_button_new((GtkAdjustment*)port_entry,(gdouble)1,(guint)0);
    GtkWidget *name_label = gtk_label_new("nickname:    ");
    GtkWidget *name_entry = gtk_entry_new();
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Message",win,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_STOCK_OK,GTK_RESPONSE_ACCEPT,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);

    /* Ensure that the dialog box is destroyed when the user responds. */
 
    /* Add the label, and show everything we've added to the dialog. */
    gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox),host_label);
    gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox),host_entry);
    gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox),port_label);
    gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox),port_spin);
    gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox),name_label);
    gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox),name_entry);
    gtk_widget_show_all(dialog);

    ret_sig = gtk_dialog_run((GtkDialog*)dialog);
    if(ret_sig == GTK_RESPONSE_ACCEPT)
    {
        accept = true;
        host = new std::string((char*)gtk_entry_get_text((GtkEntry*)host_entry));
        name = new std::string((char*)gtk_entry_get_text((GtkEntry*)name_entry));
        port = gtk_spin_button_get_value_as_int((GtkSpinButton*)port_spin);
    }
    gtk_widget_destroy(dialog);
    gdk_threads_leave();
}

NewClientDialog::~NewClientDialog()
{
    /* should probably clean up more than I did. Oh well, I'll get to it TODO */
}

