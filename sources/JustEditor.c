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
	
	gtk_grid_attach (GTK_GRID (mGrid), label_template ("New", 0), 0, 0, 1, 1);
	
	tBtn = button_template ("New File");
	g_signal_connect (tBtn, "clicked", G_CALLBACK (newfBtn_clicked), NULL);
	gtk_grid_attach (GTK_GRID (mGrid), tBtn, 0, 1, 1, 1);
	
	tabCon = gtk_notebook_new ();
	gtk_notebook_append_page (GTK_NOTEBOOK (tabCon), mGrid, a_page ());
	gtk_container_add (GTK_CONTAINER (mWindow), tabCon);
	css_default ();
	gtk_widget_show_all (mWindow);
}

void newfBtn_clicked (GtkWidget *newfBtn, gpointer sender)
{
	GtkWidget *scr; //, *nWindow;
//	nWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
//	window_template (nWindow, 512, 384, true, "new");
	scr = gtk_scrolled_window_new (GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS);
	gtk_container_add (GTK_CONTAINER (scr), gtk_text_view_new ());
	gtk_widget_show_all (scr);

	gtk_notebook_set_current_page (GTK_NOTEBOOK (tabCon), gtk_notebook_insert_page (GTK_NOTEBOOK (tabCon), scr, tab_template ("New File", scr), gtk_notebook_get_n_pages (GTK_NOTEBOOK (tabCon)) - 1));
//	gtk_container_add (GTK_CONTAINER (nWindow), scr);
//	gtk_widget_show_all (nWindow);
}
