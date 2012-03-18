/* Regen (an irc client)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <string.h>

#include "../main/regen.h"
#include "../main/server.h"
#include "../main/irc.h"
#include "../main/config.h"
#include "gui.h"
#include "window.h"
#include "switchbar.h"
#include "display.h"

static GList *window_list = NULL;

window *
window_new(server *s, char *name, int state, int type)
{
	GtkWidget *event_box_main,
		  *event_box_text,
		  *textview,
		  *treeview,
		  *inputbar,
		  *vbox,
		  *hbox;
	GtkNotebook *notebook;
	GdkColor background;
	window *w = (window *)malloc(sizeof(window));
	
	w->type = type;
	w->state = state;
	strcpy(w->name, name);
	w->_server = s;
	
	event_box_main = gtk_event_box_new();
	event_box_text = gtk_event_box_new();
	
	vbox = gtk_vbox_new(FALSE, 1);
	hbox = gtk_hbox_new(FALSE, 1);
	
	notebook = regen_get_notebook();
	inputbar = inputbar_new(w);
	textview = textview_new(w);
	
	gdk_color_parse("white", &background);
	gtk_widget_modify_bg(event_box_main, GTK_STATE_NORMAL, &background);
	gdk_color_parse("white", &background); /*#40475b", &background);*/
	gtk_widget_modify_bg(event_box_text, GTK_STATE_NORMAL, &background);
	
	gtk_container_add(GTK_CONTAINER(event_box_main), vbox);
	gtk_container_add(GTK_CONTAINER(event_box_text), hbox);
	
	gtk_box_pack_start(GTK_BOX(hbox), textview, TRUE, TRUE, 0);

	if(type == REGEN_WINDOW_TYPE_CHANNEL)
	{
		treeview = _window_create_nicklist();
		gtk_box_pack_start(GTK_BOX(hbox), treeview, FALSE, FALSE, 0);
		w->treeview = GTK_TREE_VIEW(treeview);
	}
	
	gtk_box_pack_start(GTK_BOX(vbox), event_box_text, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), inputbar, FALSE, FALSE, 0);
	
	gtk_widget_show(vbox);
	gtk_widget_show(hbox);
	gtk_widget_show(event_box_main);
	gtk_widget_show(event_box_text);
	
	gtk_notebook_append_page(notebook, event_box_main, NULL);

	window_list = g_list_append(window_list, w);
	w->nb_pos = gtk_notebook_get_n_pages(notebook) - 1;
	w->switchbar_item = switchbar_item_new(w->name, state, type);
	switchbar_set_active(w->switchbar_item);

	w->input_buffer = NULL; /* init the input buffer linked list */
	w->input_buffer_current = NULL; /* this too */
	
	gtk_notebook_set_current_page(notebook, w->nb_pos);
	gtk_widget_grab_focus(GTK_WIDGET(w->inputbar));

	return w;
}

void
window_destroy(window *w)
{
	int pos;
	GtkNotebook *notebook = regen_get_notebook();
	window *win;
	
	/* er... no, bad user! */	
	if(w->type == REGEN_WINDOW_TYPE_STATUS)
		return;
	
	pos = w->nb_pos;
	gtk_notebook_remove_page(notebook, pos);
	switchbar_item_remove(w->switchbar_item);

	/* shift all the remaining ones down one */
	win = window_find_by_pos(++pos);
	while(win != NULL)
	{
		win->nb_pos--;
		win = window_find_by_pos(++pos);
	}
	
	window_list = g_list_remove(window_list, w);
	
	free(w);

	switchbar_set_active(window_get_active()->switchbar_item);
}

window *
window_get_active()
{
	return window_find_by_pos(gtk_notebook_get_current_page(regen_get_notebook()));
}

gboolean
window_is_active(window *w)
{
	if(w == window_get_active())
		return TRUE;
	return FALSE;
}

window *
window_find_by_pos(int pos)
{
	window *w;
	GList  *wl = g_list_first(window_list);
	
	while(wl != NULL)
	{
		w = (window *)wl->data;
		if(w->nb_pos == pos)
			return w;
		wl = g_list_next(wl);
	}
	return NULL;
}

window *
window_find_by_switchbar_item(GtkToolItem *item)
{
	window *w;
	GList *wl = g_list_first(window_list);
	while(wl != NULL)
	{
		w = (window *)wl->data;
		if(w->switchbar_item == item)
			return w;
		wl = g_list_next(wl);
	}
	return NULL;
}

window *
window_find_by_name(server *s, char *name)
{
	window *w;
	GList *wl = g_list_first(window_list);
	while(wl != NULL)
	{
		w = (window *)wl->data;
		if(!strcmp(w->name, name) && w->_server == s)
			return w;
		wl = g_list_next(wl);
	}
	return NULL;
}

window *
window_next(void)
{
	window *w = window_find_by_pos(window_get_active()->nb_pos + 1);
	if(w != NULL)
		return w;
	else
		return window_find_by_pos(0);
}

void
window_set_active(window *w)
{
	GtkNotebook *notebook;
	channel *c;
	
	if(w == NULL)
		return;
	
	notebook = regen_get_notebook();
	gtk_notebook_set_current_page(notebook, w->nb_pos);
	switchbar_set_active(w->switchbar_item);
	if(w->state == REGEN_WINDOW_STATE_HIGHLIGHTED)
		switchbar_set_highlight(w->switchbar_item, REGEN_WINDOW_STATE_NORMAL);
	regen_update_title();
}

void
window_scroll_to_bottom(window *w)
{
	GtkTextBuffer *buffer;
	GtkTextMark *mark;
	GtkTextIter iter_start;
	GtkTextIter iter_end;

	buffer = gtk_text_view_get_buffer(w->textview);
	gtk_text_buffer_get_bounds(buffer, &iter_start, &iter_end);

	mark = gtk_text_buffer_create_mark(buffer, NULL, &iter_end, FALSE);
	gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(w->textview), mark);
	gtk_text_buffer_delete_mark(buffer, mark);
}

void
window_clear_buffer(window *w)
{
	GtkTextBuffer *buffer;
	GtkTextIter iter_start;
	GtkTextIter iter_end;

	buffer = gtk_text_view_get_buffer(w->textview);
	gtk_text_buffer_get_bounds(buffer, &iter_start, &iter_end);
	gtk_text_buffer_delete(buffer, &iter_start, &iter_end);
}
	
GtkWidget *
textview_new(window *w)
{
	GtkWidget *vbox;
	GtkWidget *scrolled;
	GtkTextBuffer *buffer;
	GdkColor background;
	GdkColor foreground;
	PangoFontDescription *font;
	GtkWidget *textview;
	
	vbox = gtk_vbox_new(FALSE, 0);
	
	scrolled = gtk_scrolled_window_new(NULL, NULL);

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled), GTK_SHADOW_NONE);

	textview = gtk_text_view_new();

	gtk_widget_show(scrolled);
	gtk_widget_show(textview);

	/* deal with changing the colors */
	gdk_color_parse(config_get("background-color"), &background);
	gdk_color_parse("white", &foreground);
	gtk_widget_modify_base(textview, GTK_STATE_NORMAL, &background);
	gtk_widget_modify_text(textview, GTK_STATE_NORMAL, &foreground);

	/* FIXME this'll have to be determined by themes */
	font = pango_font_description_from_string ("Verdana 8");
	gtk_widget_modify_font(textview, font);
	
	/* create the color tags */
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
	gtk_text_buffer_create_tag(buffer, "c00", "foreground", "#FFFFFF", NULL);
	gtk_text_buffer_create_tag(buffer, "c01", "foreground", "#000000", NULL);
	gtk_text_buffer_create_tag(buffer, "c02", "foreground", "#000088", NULL);
	gtk_text_buffer_create_tag(buffer, "c03", "foreground", "#008800", NULL);
	gtk_text_buffer_create_tag(buffer, "c04", "foreground", "#880000", NULL);
	gtk_text_buffer_create_tag(buffer, "c05", "foreground", "#888800", NULL);
	gtk_text_buffer_create_tag(buffer, "c06", "foreground", "#880088", NULL);
	gtk_text_buffer_create_tag(buffer, "c07", "foreground", "#FF0000", NULL);
	gtk_text_buffer_create_tag(buffer, "c08", "foreground", "#FFFF00", NULL);
	gtk_text_buffer_create_tag(buffer, "c09", "foreground", "#00FF00", NULL);
	gtk_text_buffer_create_tag(buffer, "c10", "foreground", "#00FFFF", NULL);
	gtk_text_buffer_create_tag(buffer, "c11", "foreground", "#008888", NULL);
	gtk_text_buffer_create_tag(buffer, "c12", "foreground", "#0000FF", NULL);
	gtk_text_buffer_create_tag(buffer, "c13", "foreground", "#FF00FF", NULL);
	gtk_text_buffer_create_tag(buffer, "c14", "foreground", "#888888", NULL);
	gtk_text_buffer_create_tag(buffer, "c15", "foreground", "#CCCCCC", NULL);
	gtk_text_buffer_create_tag(buffer, "c16", "foreground", "#6A8093", NULL);
	gtk_text_buffer_create_tag(buffer, "c17", "foreground", "#8496A6", NULL);
	gtk_text_buffer_create_tag(buffer, "bold", "weight", PANGO_WEIGHT_BOLD, NULL);

	/* setup some policies regarding the text area */
	GTK_WIDGET_UNSET_FLAGS(textview, GTK_CAN_FOCUS);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview), GTK_WRAP_WORD_CHAR);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(textview), FALSE);
	gtk_text_view_set_left_margin(GTK_TEXT_VIEW(textview), 2);
	
	gtk_container_add(GTK_CONTAINER(scrolled), textview);
	gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);

	gtk_widget_show(vbox);
	
	w->textview = GTK_TEXT_VIEW(textview);
	return vbox;
}

GtkWidget *
inputbar_new(window *w)
{
	GtkWidget *vbox;
	GtkWidget *inputbar;
	GdkColor background;
	GdkColor foreground;
	PangoFontDescription *font;
	
	vbox     = gtk_vbox_new(FALSE, 0);
	inputbar = gtk_entry_new();
	
	gdk_color_parse("#40475b", &background);
	gdk_color_parse("white", &foreground);
	gtk_widget_modify_base(inputbar, GTK_STATE_NORMAL, &background);
	gtk_widget_modify_text(inputbar, GTK_STATE_NORMAL, &foreground);

	font = pango_font_description_from_string ("Verdana 8");
	gtk_widget_modify_font(inputbar, font);
	
	gtk_box_pack_start(GTK_BOX(vbox), inputbar, TRUE, TRUE, 0);
	
	gtk_entry_set_has_frame(GTK_ENTRY(inputbar), FALSE);
	
	g_signal_connect(G_OBJECT(inputbar), "activate", G_CALLBACK(window_cb_text_entry), (gpointer)inputbar);
	g_signal_connect(G_OBJECT(inputbar), "key-press-event", G_CALLBACK(window_cb_key_press), (gpointer)inputbar);
	
	gtk_widget_show(vbox);
	gtk_widget_show(inputbar);

	modify_cursor_color(inputbar, "white");
	
	w->inputbar = GTK_ENTRY(inputbar);
	return vbox;
}

void
window_printf(window *w, char *format, ...)
{
	va_list args;
	char *buf;

	va_start(args, format);
	buf = g_strdup_vprintf(format, args);
	va_end(args);

	window_print(w, buf);
	g_free(buf);
}

void
window_print(window *w, char *what)
{
	GtkTextBuffer *buffer;
	GtkTextIter iter_start;
	GtkTextIter iter_end;
	GtkTextTag *tag;
	
	char *ptr = what;
	char *pptr = what;
	int was_bold = FALSE;
	char color[4] = "c00\0";
	int x = 0;
	int i = 0;

	if(w == NULL)
	{
		g_warning("window_print() was passed a NULL window pointer\n");
		return;
	}
	if(!g_utf8_validate(what, -1, NULL))
	{
		g_warning("window_print() was passed an invalid utf-8 string: %s", what);
		return;
	}
	
	buffer = gtk_text_view_get_buffer(w->textview);
	gtk_text_buffer_get_bounds(buffer, &iter_start, &iter_end);

	for(i = 0;i < strlen(what);i++)
	{
		if(ptr[0] == '\3')
		{
			if(was_bold)
				gtk_text_buffer_insert_with_tags_by_name(buffer, &iter_end, pptr, x, (char *)&color, "bold", NULL);
			else
				gtk_text_buffer_insert_with_tags_by_name(buffer, &iter_end, pptr, x, (char *)&color, NULL);
			pptr = ptr;
			x = 0;
			if(isdigit(ptr[1]))
			{
				if(isdigit(ptr[2]))
				{
					sprintf((char *)&color, "c%c%c\0", ptr[1], ptr[2]);
					pptr += 3;
					x -= 3;
				}
				else
				{
					sprintf((char *)&color, "c0%c\0", ptr[1]);
					pptr += 2;
					x -=2;
				}
			}
		}
		if(ptr[0] == '\2')
		{
			if(was_bold)
			{
				gtk_text_buffer_insert_with_tags_by_name(buffer, &iter_end, pptr, x, (char *)&color, "bold", NULL);
				was_bold = FALSE;
			}
			else
			{
				gtk_text_buffer_insert_with_tags_by_name(buffer, &iter_end, pptr, x, (char *)&color, NULL);
				was_bold = TRUE;
			}
			pptr = ptr + 1;
			x = -1;
		}
		x++;
		ptr++;
	}
	if(was_bold)
		gtk_text_buffer_insert_with_tags_by_name(buffer, &iter_end, pptr, x, (char *)&color, "bold", NULL);
	else
		gtk_text_buffer_insert_with_tags_by_name(buffer, &iter_end, pptr, x, (char *)&color, NULL); 

	if(!window_is_active(w) && w->state == REGEN_WINDOW_STATE_NORMAL)
		switchbar_set_highlight(w->switchbar_item, REGEN_WINDOW_STATE_HIGHLIGHTED);
	
	window_scroll_to_bottom(w);
}



static void
window_cb_text_entry(GtkWidget *widget, GtkWidget *entry)
{
	window *w = window_get_active();
	char *input_text;
	
	input_text = (char *)gtk_entry_get_text(GTK_ENTRY(entry));
	
	if(strlen(input_text) < 1)
		return;

	window_input_buffer_insert(w, input_text);
	
	if(input_text[0] == '/')
		parse_command(input_text);
	else
	{
		if(w->type == REGEN_WINDOW_TYPE_CHANNEL || w->type == REGEN_WINDOW_TYPE_QUERY)
		{
			_cmd_privmsg(w->_server, w->name, input_text);
		}
		else
		{
			display_error(w, "cannot output to this window");
		}
	}

	gtk_entry_set_text(GTK_ENTRY(entry), "");
}

gboolean
window_cb_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	window *w = window_get_active();
	if(event->state & GDK_CONTROL_MASK)
	{
		if(event->keyval == GDK_1)
			window_set_active(window_find_by_pos(0));
		else if(event->keyval == GDK_2)
			window_set_active(window_find_by_pos(1));
		else if(event->keyval == GDK_3)
			window_set_active(window_find_by_pos(2));
		else if(event->keyval == GDK_4)
			window_set_active(window_find_by_pos(3));
		else if(event->keyval == GDK_5)
			window_set_active(window_find_by_pos(4));
		else if(event->keyval == GDK_6)
			window_set_active(window_find_by_pos(5));
		else if(event->keyval == GDK_7)
			window_set_active(window_find_by_pos(6));
		else if(event->keyval == GDK_8)
			window_set_active(window_find_by_pos(7));
		else if(event->keyval == GDK_9)
			window_set_active(window_find_by_pos(8));
		else if(event->keyval == GDK_Tab)
			window_set_active(window_next());
	}
			
	if(event->keyval == GDK_Up && w->input_buffer != NULL)
	{
		if(w->input_buffer_current == NULL)
			w->input_buffer_current = g_list_first(w->input_buffer);
		
		else if(w->input_buffer_current == g_list_last(w->input_buffer))
			return FALSE;
		
		else
			w->input_buffer_current = g_list_next(w->input_buffer_current);

		
		gtk_entry_set_text(GTK_ENTRY(user_data), w->input_buffer_current->data);
		gtk_entry_set_position(GTK_ENTRY(user_data), -1);
	}
	else if(event->keyval == GDK_Down && w->input_buffer != NULL)
	{
		if(w->input_buffer_current == NULL)
		{
			return FALSE;
		}
		else if(w->input_buffer_current == g_list_first(w->input_buffer))
		{
			gtk_entry_set_text(GTK_ENTRY(user_data), "");
			w->input_buffer_current = NULL;
			return FALSE;
		}
		else
		{
			w->input_buffer_current = g_list_previous(w->input_buffer_current);
		}

		gtk_entry_set_text(GTK_ENTRY(user_data), w->input_buffer_current->data);
		gtk_entry_set_position(GTK_ENTRY(user_data), -1);
	}
	else if(event->keyval == GDK_Tab)
	{
		/* stupid tab ! */
		return TRUE;
	}
	return FALSE;
}

void
window_input_buffer_insert(window *w, char *ptr)
{
	char *buf;
	int len;
	
	buf = strdup(ptr);
	w->input_buffer = g_list_prepend(w->input_buffer, buf);

	len = g_list_length(w->input_buffer);
	if(len > REGEN_MAX_INPUT)
		w->input_buffer = g_list_remove_link(w->input_buffer, g_list_nth(w->input_buffer, len - 1));

	w->input_buffer_current = NULL;
}

void
window_nicklist_update(window *w)
{
	GtkListStore *store = gtk_list_store_new(NUM_COLUMNS, G_TYPE_STRING);
	GtkTreeIter iter;
	GList *list;
	channel *c = server_find_channel_by_name(w->_server, w->name);
	channel_user *u;
	
	list = g_list_first(c->nick_list);
	while(list != NULL)
	{
		u = list->data;
		gtk_list_store_insert_with_values(store, &iter, -1, COLUMN_FIXED, user_nicklist_format(u), -1);
		list = g_list_next(list);
	}
	gtk_tree_view_set_model(w->treeview, GTK_TREE_MODEL(store));
}

static GtkTreeModel *
create_model()
{
	GtkListStore *store;

	store = gtk_list_store_new(NUM_COLUMNS, G_TYPE_STRING);

	return GTK_TREE_MODEL(store);
}

static GtkWidget *
_window_create_nicklist()
{
	GtkWidget *treeview;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GdkColor background;
	GdkColor foreground;

	treeview = gtk_tree_view_new_with_model(create_model());
	GTK_WIDGET_UNSET_FLAGS(treeview, GTK_CAN_FOCUS);
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(treeview), FALSE);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview), FALSE);
	
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(NULL, renderer, "text", COLUMN_FIXED, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

	gdk_color_parse("#40475b", &background);
	gdk_color_parse("white", &foreground);
	gtk_widget_modify_base(treeview, GTK_STATE_NORMAL, &background);
	gtk_widget_modify_text(treeview, GTK_STATE_NORMAL, &foreground);
	
	gtk_widget_set_size_request(treeview, 125, -1);
	gtk_widget_show(treeview);

	return treeview;
}

