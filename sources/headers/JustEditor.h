#include "DrnGtk.h"
#include "DrnTools.h"

GtkWidget *mWindow, *tabCon;

void Init (GtkApplication *JustEditor, gpointer sender);
void newfBtn_clicked (GtkWidget *newfBtn, gpointer sender);
void tabClsBtn_clicked (GtkWidget *tab);
void tabTitle_rightClicked (GtkLabel *tabTitle, TabPtr tab);

GtkWidget *tab_template (gchar *title, GtkWidget *pageCon)
{
	GtkWidget *clsBtn, *tabTitle;
	struct DrnTab tabItem;

	tabItem.content = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);

	tabItem.title = title;
	tabTitle = label_template (title, 0.01);
	

	gtk_container_add (GTK_CONTAINER (tabItem.content), tabTitle);

	clsBtn = button_template ("x");
	g_signal_connect_swapped (clsBtn, "clicked", G_CALLBACK (tabClsBtn_clicked), pageCon);
	gtk_container_add (GTK_CONTAINER (tabItem.content), clsBtn);

	gtk_widget_show_all (tabItem.content);
	return tabItem.content;
}

GtkWidget *a_page ()
{
	return gtk_label_new ("+");
}

void tabClsBtn_clicked (GtkWidget *tab)
{
	gtk_notebook_detach_tab (GTK_NOTEBOOK (tabCon), tab);
}
