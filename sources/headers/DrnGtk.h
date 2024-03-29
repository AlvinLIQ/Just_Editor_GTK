#include "DrnTools.h"
#include <gtk/gtk.h>

//DrnTab
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

//Event Callback
/*
struct DrnArg
{
	gint value;
	void (*callbck) (GtkWidget*);
};
*/
void keydown_template (GtkWidget *tItem, GdkEventKey *args, gpointer sender)
{
	if (args->keyval == GDK_KEY_Return || args->keyval == GDK_KEY_KP_Enter && sender != NULL)
	{
		((void (*) (GtkWidget*))sender) (tItem);
		gtk_widget_destroy (gtk_widget_get_parent (gtk_widget_get_parent (tItem)));
	}
}

GtkWidget *button_template (const gchar *bls)
{
	GtkWidget *btn_tpl;
	btn_tpl = gtk_button_new_with_label (bls);
	gtk_button_set_relief (GTK_BUTTON (btn_tpl), false);

	return btn_tpl;
}

GtkWidget *label_template (const gchar *lst, gfloat x)
{
	GtkWidget *lbl_tpl;
	lbl_tpl = gtk_label_new (lst);
	gtk_label_set_xalign (GTK_LABEL (lbl_tpl), x);
	
	return lbl_tpl;
}

GtkWidget *text_view_template (bool editable)
{
	GtkWidget *txt_tpl;
	txt_tpl = gtk_text_view_new ();
	gtk_text_view_set_editable (GTK_TEXT_VIEW (txt_tpl), editable);
	gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (txt_tpl), editable);
	
	return txt_tpl;
}

void window_template (GtkWidget *window, gint width, gint height, bool sizeable, const gchar *title)
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
	gtk_grid_attach (GTK_GRID (titleBar), label_template (title, 0.01), 0, 0, 87, 1);
	gtk_grid_attach (GTK_GRID (titleBar), tBox, 87, 0, 13, 1);
//	gtk_container_add (GTK_CONTAINER (titleBar), tBox);

	if (title != NULL)
		gtk_window_set_title (GTK_WINDOW (window), title);
	
	gtk_window_set_titlebar (GTK_WINDOW (window), titleBar);
}

GtkWidget *indialog_template (const gchar *title, gchar *subTitle, gchar *btnLabel, void (*callbck) (GtkWidget*))
{
	GtkWidget *tDialog, *tBox, *tBtn, *tText; 
	tDialog = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	window_template (tDialog, 200, 300, false, title);

	tBox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 20);
	gtk_container_add (GTK_CONTAINER (tBox), gtk_label_new (subTitle));
	
	gtk_container_add (GTK_CONTAINER (tBox), tText = gtk_entry_new ());
	g_signal_connect (tText, "key-release-event", G_CALLBACK (keydown_template), callbck);
	
	tBtn = button_template (btnLabel);
	g_signal_connect_swapped (tBtn, "clicked", G_CALLBACK (callbck), tText);
	g_signal_connect_swapped (tBtn, "clicked", G_CALLBACK (gtk_widget_destroy), tDialog);

	gtk_container_add (GTK_CONTAINER (tBox), tBtn);
	gtk_container_add (GTK_CONTAINER (tDialog), tBox);
	
	gtk_widget_show_all (tDialog);
	return tDialog;
}

gchar *openFileDialog (GtkWidget *window)
{
	GtkWidget *chsr;
	chsr = gtk_file_chooser_dialog_new ("Open File", GTK_WINDOW (window),  
					GTK_FILE_CHOOSER_ACTION_OPEN,
					 "Open", GTK_RESPONSE_ACCEPT,
					 "Cancel", GTK_RESPONSE_CANCEL, NULL);
	uchar *filename = NULL;
	if (gtk_dialog_run (GTK_DIALOG (chsr)) == GTK_RESPONSE_ACCEPT)
	{

		GtkFileChooser *chooser = GTK_FILE_CHOOSER (chsr);
		filename = gtk_file_chooser_get_filename (chooser);

//		g_print (filename);

//		g_free (filename);
	}
	gtk_widget_destroy (chsr);
	return filename;
}

void gtk_text_view_scroll_to_end (GtkTextView *txtView)
{
	GtkTextIter ed;
	gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer (txtView), &ed);
	gtk_text_view_scroll_to_iter (txtView, &ed, 0, false, 0, 1);

}

struct bufferUpdateArgs
{
	GtkTextBuffer *r_Buf;
	GtkTextIter r_Iter;
	char *r_Str;
	int r_Len;
};

gpointer buffer_update (void *args)
{
	struct bufferUpdateArgs *bufArgs = (struct bufferUpdateArgs *)args;
	gtk_text_buffer_insert_interactive (bufArgs->r_Buf, &bufArgs->r_Iter, bufArgs->r_Str, -1, true);
	return 0;
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
		"background-color:		rgba (32, 32, 32, 0.8);"
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
		"button:hover, selection{"
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

