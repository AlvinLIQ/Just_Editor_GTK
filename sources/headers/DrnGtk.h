#include "DrnTools.h"
#include <gtk/gtk.h>

struct DrnTab;
#define TabPtr struct DrnTab*
#define DRN_TAB (x) (TabPtr)x

GtkWidget *selectedTab;

struct DrnTab
{
	GtkWidget *content;
	gchar *title;
	gchar *path;
	GtkWidget *editor;
	TabPtr nxtPtr;
};

struct DrnEdit
{
	GtkWidget *box;
	gchar *text;
	guint st_pos;
	guint ed_pos;
};

void AddTab (gchar *title, gchar *path, GtkWidget *content, TabPtr after)
{
	TabPtr nPtr;
	struct DrnTab nTab = {content, title, path, gtk_text_view_new (), NULL};
	while ((nPtr = after->nxtPtr) != NULL);
	nPtr = &nTab;
//	nPtr = (TabPtr)malloc (sizeof (struct DrnTab));
}

void RemoveTabAfter (TabPtr tItem)
{
	TabPtr tmp = tItem->nxtPtr;
	tItem->nxtPtr = tItem->nxtPtr->nxtPtr;
	free (tmp);
}

GtkWidget *button_template (gchar *bls)
{
	GtkWidget *btn_tpl;
	btn_tpl = gtk_button_new_with_label (bls);
	gtk_button_set_relief (GTK_BUTTON (btn_tpl), false);

	return btn_tpl;
}

GtkWidget *label_template (gchar *lst, gfloat x)
{
	GtkWidget *lbl_tpl;
	lbl_tpl = gtk_label_new (lst);
	gtk_label_set_xalign (GTK_LABEL (lbl_tpl), x);
	
	return lbl_tpl;
}

void window_template (GtkWidget *window, gint width, gint height, bool sizeable, gchar *title)
{
	gtk_window_set_default_size (GTK_WINDOW (window), width, height);
	gtk_window_set_resizable (GTK_WINDOW (window), sizeable);

	GtkWidget *titleBar, *tBox, *tBtn;

	tBox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_halign (tBox, GTK_ALIGN_END);

	tBtn = button_template ("-");
	g_signal_connect_swapped (tBtn, "clicked", G_CALLBACK (gtk_window_iconify), GTK_WINDOW (window));
	gtk_container_add (GTK_CONTAINER (tBox), tBtn);

	tBtn = button_template ("x");
	g_signal_connect_swapped (tBtn, "clicked", G_CALLBACK (gtk_widget_destroy), window);
	gtk_container_add (GTK_CONTAINER (tBox), tBtn);

	titleBar = gtk_grid_new ();
	gtk_grid_set_column_homogeneous (GTK_GRID (titleBar), true);
	gtk_grid_attach (GTK_GRID (titleBar), label_template ("Just Editor", 0.01), 0, 0, 87, 1);
	gtk_grid_attach (GTK_GRID (titleBar), tBox, 87, 0, 13, 1);
//	gtk_container_add (GTK_CONTAINER (titleBar), tBox);

	if (title != NULL)
		gtk_window_set_title (GTK_WINDOW (window), title);
	
	gtk_window_set_titlebar (GTK_WINDOW (window), titleBar);
}

void append_css (gchar *css_s)
{
	GtkCssProvider *cprdr;
	cprdr = gtk_css_provider_new ();
	gtk_css_provider_load_from_data (cprdr, css_s, -1 ,NULL);
	gtk_style_context_add_provider_for_screen (gdk_screen_get_default (), GTK_STYLE_PROVIDER(cprdr), GTK_STYLE_PROVIDER_PRIORITY_USER);
	g_object_unref (cprdr);
}

void css_default ()
{
	append_css (
//		".titlebar label:first-child{"
//		"font-size:			14pt;"
//		"}"
		"window, .titlebar{"
		"background-image:		url(\"resources/Ashe.png\");"
		"background-size:		cover;"
		"background-repeat:		no-repeat;"
		"}"
		"notebook grid:first-child{"
		"background-color:		rgba (32, 57, 57, 0.6);"
		"padding:			4px 8px 4px 8px;"
		"}"
		"notebook header, notebook tab, notebook, header.top, button, scrollbar:hover, scrollbar, entry{"
		"border:			0 none;"
		"box-shadow:			none;"
		"}"
		"notebook header, header box{"
		"padding:			0px;"
		"}"
		"notebook tab{"
		"background-color:		rgba (32, 32, 32, 0.8);"
		"margin:			0px;"
		"padding:			3px 0px 3px 3px;"
		"}"
		"notebook tab:first-child{"
		"padding:			3px;"
		"}"
		"button{"
		"background-image:		none;"
		"background-color:		rgba (32, 57, 57, 0.5);"
		"}"
		"button:hover{"
		"background-color:		rgba (70, 70, 70, 0.5);"
		"}"
		"button:active{"
		"background-color:		rgba (48, 48, 48, 0.5);"
		"}"
		"notebook tab:first-child, header button, scrollbar:hover, scrollbar{"
		"background-color:		rgba (0, 0, 0, 0);"
		"}"
		"notebook tab:hover{"
		"background-color:		rgba (68, 68, 68, 0.5);"
		"}"
		"label{"
		"text-shadow:			none;"
		"color:				#AAA;"
		"}"
		"text, entry{"
		"background-color:		rgba (0, 0, 0, 0);"
		"color:				#AAA;"
		"}"
		"textview, entry{"
		"background-color:		rgba (32, 47, 47, 0.7);"
		"font-size:			12pt;"
		"}"
		".titlebar, notebook stack, header.top{"
		"background-color:		rgba (37, 51, 51, 0.3);"
		"}"
		".titlebar button{"
		"padding:			4px 16px;"
		"background-color:		rgba (0, 0, 0, 0);"
		"}"
		"window, button, .titlebar, notebook tab, entry{"
		"border-radius:			0px;"
		"}"
		"header button{"
		"padding:			1px 4px 2px 4px;"
		"margin:			4px 0px;"
		"}"
		".titlebar box button:last-child:hover{"
		"background-color:		rgba (255, 0, 0, 0.5);"
		"}"
		".titlebar box button:last-child:active{"
		"background-color:		rgba (238, 68, 68, 0.5);"
		"}");
}

