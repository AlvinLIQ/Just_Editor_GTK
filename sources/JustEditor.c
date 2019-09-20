#include "headers/JustEditor.h"

int main (int argc, char **argv)
{
	int status;
	GtkApplication *JustEditor;
	JustEditor = gtk_application_new ("Just.Editor", G_APPLICATION_FLAGS_NONE);
	g_signal_connect (JustEditor, "activate", G_CALLBACK (Init), NULL);
	status = g_application_run (G_APPLICATION (JustEditor), argc, argv);

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
//	gtk_container_add (GTK_CONTAINER (mWindow), mGrid);
	
	gtk_grid_attach (GTK_GRID (mGrid), label_template ("File", 0), 0, 0, 1, 1);
	
	tBtn = button_template ("Open File");
	g_signal_connect (tBtn, "clicked", G_CALLBACK (openfBtn_clicked), NULL);
	gtk_grid_attach (GTK_GRID (mGrid), tBtn, 0, 1, 1, 1);

	tBtn = button_template ("New File");
	g_signal_connect (tBtn, "clicked", G_CALLBACK (newfBtn_clicked), NULL);
	gtk_grid_attach (GTK_GRID (mGrid), tBtn, 1, 1, 1, 1);

	gtk_grid_attach (GTK_GRID (mGrid), label_template ("Network", 0), 0, 2, 1, 1);

	tBtn = button_template ("Client");
	g_signal_connect (tBtn, "clicked", G_CALLBACK (newrBtn_clicked), NULL);
	gtk_grid_attach (GTK_GRID (mGrid), tBtn, 0, 3, 1, 1);

	tBtn = button_template ("Server");
	g_signal_connect (tBtn, "clicked", G_CALLBACK (srvBtn_clicked), NULL);
	gtk_grid_attach (GTK_GRID (mGrid), tBtn, 1, 3, 1, 1);
	
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
//	onConn (httpReq);
	newfBtn_clicked (NULL, (gpointer)"&Request");
}

void srvBtn_clicked (GtkWidget *openfBtn, gpointer sender)
{
	onConn (httpRes, NULL);
}
