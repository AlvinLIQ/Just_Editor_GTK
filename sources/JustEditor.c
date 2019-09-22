#include "headers/JustEditor.h"

const GCallback home_callbck_list[] = {NULL, G_CALLBACK (openfBtn_clicked), G_CALLBACK (newfBtn_clicked), NULL, G_CALLBACK (newrBtn_clicked),
					G_CALLBACK (srvBtn_clicked), G_CALLBACK (shdBtn_clicked), G_CALLBACK (consolePipe)};

const gchar home_title_list[][10] = {"File", "Open File", "New File", "Network", "Client", "Server", "_shadow", "hey"};

int main (int argc, char **argv)
{
	int status;

	GtkApplication *JustEditor;
	JustEditor = gtk_application_new ("Just.Editor", G_APPLICATION_FLAGS_NONE);
	g_signal_connect (JustEditor, "activate", G_CALLBACK (Init), argc == 2 ? argv[1] : NULL);
	
	status = g_application_run (G_APPLICATION (JustEditor), 1, argv);

	g_object_unref (JustEditor);
	return status;
}

void Init (GtkApplication *JustEditor, gpointer sender)
{
	mWindow = gtk_application_window_new (JustEditor);
	window_template (mWindow, 640, 480, true, "Just Editor");

	GtkWidget *mGrid, *tBtn;
	
	mGrid = gtk_grid_new ();
	gtk_widget_set_halign (mGrid, GTK_ALIGN_START);
	gtk_widget_set_size_request (mGrid, 250, 0);
	gtk_grid_set_row_spacing (GTK_GRID (mGrid), 5);

	for (int i = 0, x = 0, y = 0; i < 8; i++) //load left menu
	{
		if (home_callbck_list[i] == NULL)
			gtk_grid_attach (GTK_GRID (mGrid), label_template (home_title_list[i], 0), x, y++, 1, 1);
		else
		{
			tBtn = button_template (home_title_list[i]);
			g_signal_connect (tBtn, "clicked", home_callbck_list[i], NULL);
			gtk_grid_attach (GTK_GRID (mGrid), tBtn, x, y, 1, 1);
			if (x == 0)
			{
				x++;
			}
			else
			{
				x = 0;
				y++;
			}
		}
	}
	if (sender != NULL)
	{
		struct hostent *hp;
		if ((hp = gethostbyname ((char*)sender)) != NULL)
			shd_addr = *(u_long*)hp->h_addr_list[0];
	}

	tabCon = gtk_notebook_new ();
	gtk_notebook_append_page (GTK_NOTEBOOK (tabCon), mGrid, a_page ());
	gtk_container_add (GTK_CONTAINER (mWindow), tabCon);
	g_signal_connect_swapped (tabCon, "switch-page", G_CALLBACK (tabPage_switched), NULL);
	css_default ();
	gtk_widget_show_all (mWindow);
}

void newfBtn_clicked (GtkWidget *newfBtn, gpointer sender)
{
	GtkWidget *scr;

	scr = gtk_scrolled_window_new (GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS);

	struct DrnTab tTab;
	if (sender == NULL)
	{
		tTab = tab_template ("New File", scr);
		tTab.path = NULL;
	}
	else if (((uchar*)sender)[0] != '&')
	{
		tTab = tab_template (afterLast ((uchar*)sender, path_split), scr);
		tTab.path = (uchar*)sender;
		uchar *nText = readFileFrStr (tTab.path);

		GtkTextBuffer *nBuf = gtk_text_buffer_new (NULL);
		gtk_text_buffer_set_text (nBuf, nText, -1);
		gtk_text_view_set_buffer (GTK_TEXT_VIEW (tTab.editor), nBuf);
		free (nText);
	}
	else
	{
		tTab = tab_template ((uchar*)sender, scr);
	}
	gtk_container_add (GTK_CONTAINER (scr), tTab.editor);
	gtk_widget_show_all (scr);

	gtk_notebook_set_current_page (GTK_NOTEBOOK (tabCon), gtk_notebook_insert_page (GTK_NOTEBOOK (tabCon), scr, tTab.content, gtk_notebook_get_n_pages (GTK_NOTEBOOK (tabCon)) - 1));
}

void openfBtn_clicked (GtkWidget *openfBtn, gpointer sender)
{
	newfBtn_clicked (NULL, (gpointer)openFileDialog (mWindow));
}

void newrBtn_clicked (GtkWidget *newrBtn, gpointer sender)
{
	newfBtn_clicked (NULL, (gpointer)"&Request");
}

void srvBtn_clicked (GtkWidget *openfBtn, gpointer sender)
{
	indialog_template ("Server Configuration", "Please input your port", "OK", &startServer);
//	onConn (httpRes, NULL);
}

void shdBtn_clicked (GtkWidget *openfBtn, gpointer sender)
{
	indialog_template ("_Shadow", "Port", "OK", &shdServer);
}

