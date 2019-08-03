#include "DrnGtk.h"

#ifdef _LINUX
#define path_split '/'
#else
#define path_split '\\'
#endif

GtkWidget *mWindow, *tabCon;

void Init (GtkApplication *JustEditor, gpointer sender);
void newfBtn_clicked (GtkWidget *newfBtn, gpointer sender);
void openfBtn_clicked (GtkWidget *newfBtn, gpointer sender);
void tabClsBtn_clicked (GtkWidget *tab);
void tabTitle_rightClicked (GtkLabel *tabTitle, TabPtr tab);

struct DrnTab tab_template (gchar *title, GtkWidget *pageCon)
{
	GtkWidget *clsBtn, *tabTitle;
	struct DrnTab tabItem = {gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4), title, NULL, gtk_text_view_new (), NULL};

	tabTitle = label_template (title, 0.01);
	gtk_container_add (GTK_CONTAINER (tabItem.content), tabTitle);

	clsBtn = button_template ("x");
	g_signal_connect_swapped (clsBtn, "clicked", G_CALLBACK (tabClsBtn_clicked), pageCon);
	gtk_container_add (GTK_CONTAINER (tabItem.content), clsBtn);

	gtk_widget_show_all (tabItem.content);
	return tabItem;
}

GtkWidget *a_page ()
{
	return gtk_label_new ("+");
}

void tabClsBtn_clicked (GtkWidget *tab)
{
	gtk_notebook_detach_tab (GTK_NOTEBOOK (tabCon), tab);
}
