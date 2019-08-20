#include "DrnGtk.h"

#ifdef linux
#define path_split '/'
#else
#define path_split '\\'
#endif

GtkWidget *mWindow, *tabCon;

void Init (GtkApplication *JustEditor, gpointer sender);
void newfBtn_clicked (GtkWidget *newfBtn, gpointer sender);
void openfBtn_clicked (GtkWidget *openfBtn, gpointer sender);
void newrBtn_clicked (GtkWidget *newrBtn, gpointer sender);
void tabClsBtn_clicked (GtkWidget *tab);
void tabTitle_pressed (GtkLabel *tabTitle, GdkEventKey *args, GtkWidget *tab);
void sendBtn_clicked(gpointer sender);

GtkWidget *getEditor ()
{
	GtkWidget *tEditor, *tItem, *tIP;
	tEditor = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);

	gtk_container_add (GTK_CONTAINER (tEditor), gtk_label_new ("IP Address"));
	gtk_container_add (GTK_CONTAINER (tEditor), tIP = gtk_entry_new ());
	gtk_container_add (GTK_CONTAINER (tEditor), gtk_label_new ("Header & Body"));
	gtk_container_add (GTK_CONTAINER (tEditor), gtk_text_view_new ());

	gtk_container_add (GTK_CONTAINER (tEditor), tItem = button_template ("Send"));
	g_signal_connect_swapped (tItem, "clicked", G_CALLBACK (sendBtn_clicked), tEditor);

	gtk_container_add (GTK_CONTAINER (tEditor), gtk_label_new ("Response"));
	gtk_container_add (GTK_CONTAINER (tEditor), gtk_label_new (""));
	
	return tEditor;
}

struct DrnTab tab_template (gchar *title, GtkWidget *pageCon)
{
	GtkWidget *clsBtn, *tabTitle;
	struct DrnTab tabItem = {gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4), title, NULL, title[0] == '&' ? getEditor (): gtk_text_view_new (), NULL};

	tabTitle = label_template (title, 0.01);
	gtk_container_add (GTK_CONTAINER (tabItem.content), tabTitle);

	clsBtn = button_template ("x");
	g_signal_connect_swapped (clsBtn, "clicked", G_CALLBACK (tabClsBtn_clicked), pageCon);
	gtk_container_add (GTK_CONTAINER (tabItem.content), clsBtn);

	g_signal_connect (tabItem.content, "key-press-event", G_CALLBACK (tabTitle_pressed), pageCon);
	gtk_widget_show_all (tabItem.content);
	return tabItem;
}

GtkWidget *a_page ()
{
	return gtk_label_new ("+");
}

void tabTitle_pressed (GtkLabel *tabTitle, GdkEventKey *args, GtkWidget *tab)
{
	if (args->keyval == GDK_BUTTON_MIDDLE)
		tabClsBtn_clicked (tab);
	else if (args->keyval == GDK_BUTTON_SECONDARY)
		g_print ("test\n");
}

void tabClsBtn_clicked (GtkWidget *tab)
{
	gtk_notebook_detach_tab (GTK_NOTEBOOK (tabCon), tab);
}

//http request
#ifdef linux
void *httpReq (void *sender)
{

#else
DWORD WINAPI httpReq (LPVOID sender)
{
#endif
	int s_fd = initSocket ();
	g_print ("connecting\n");
	if (sockConn (s_fd, ((httpRequest *)sender)->target_addr) != 0)
		goto failed;
	char *s = ((httpRequest *)sender)->hbStr;

	char r_str[10240];
	int r_len;
	g_print ("connected\n");
	if (send (s_fd, s, strlen(s), 0) >= 0)
	{
		free (s);
		s = NULL;
		((httpRequest *)sender)->hbStr = NULL;
		g_print ("sent\n");
		while ((r_len = recv (s_fd, r_str, 10240, 0)) > 0)
		{
			r_str [r_len] = '\0';
			gtk_label_set_text (((httpRequest *)sender)->resBox, r_str);
		}
		g_print ("over\n");
	}
	else
		goto failed;
	g_print ("_close\n");
	closeSocket (s_fd);
#ifdef linux
	return (void *)0;
#else
	return 0;
#endif
failed:
	g_print ("failed\n");
#ifdef linux
	return (void *)0;
#else
	return -1;
#endif
}

void sendBtn_clicked(gpointer sender)
{
	GList *tList = gtk_container_get_children (GTK_CONTAINER (sender));

	GtkTextIter st, ed;
	GtkTextBuffer *buffer = gtk_text_view_get_buffer (g_list_nth_data (tList, 3));

	gtk_text_buffer_get_start_iter (buffer, &st);
	gtk_text_buffer_get_end_iter (buffer, &ed);
//	replace_str (gtk_text_buffer_get_text (buffer, &st, &ed, FALSE), "\n", "\r\n", 0, true);
	httpRequest tReq = {initAddr (gtk_entry_get_text (g_list_nth_data (tList, 1)), 80), replace_str (gtk_text_buffer_get_text (buffer, &st, &ed, FALSE), "\n", "\r\n", 0, true), (void *)g_list_nth_data (tList, 6)};

	onConn (httpReq, (void *)&tReq);
}

