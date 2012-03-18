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

#include "../main/regen.h"
#include "../main/config.h"
#include "gui.h"
#include "window.h"
#include "switchbar.h"

static GtkWidget *switchbar;


GtkWidget *
create_switchbar()
{
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *close;
	GtkWidget *icon;
	GtkToolItem *item;

	vbox = gtk_vbox_new(FALSE, 0);
	hbox = gtk_hbox_new(FALSE, 0);
	
	switchbar = gtk_toolbar_new();
	gtk_toolbar_set_orientation(GTK_TOOLBAR(switchbar), GTK_ORIENTATION_HORIZONTAL);
	gtk_toolbar_set_style(GTK_TOOLBAR(switchbar), GTK_TOOLBAR_BOTH_HORIZ);
	
	close = gtk_toolbar_new();
	gtk_toolbar_set_orientation(GTK_TOOLBAR(close), GTK_ORIENTATION_HORIZONTAL);
	gtk_toolbar_set_style(GTK_TOOLBAR(close), GTK_TOOLBAR_ICONS);
	gtk_toolbar_set_show_arrow(GTK_TOOLBAR(close), FALSE);
	item = gtk_separator_tool_item_new();
	gtk_widget_show(GTK_WIDGET(item));
	gtk_toolbar_insert(GTK_TOOLBAR(close), item, -1);
	icon = gtk_image_new_from_file(get_regen_dir_plus("icons/close.png"));
	item = gtk_tool_button_new(icon, NULL);
	gtk_widget_show(GTK_WIDGET(item));
	gtk_widget_show(GTK_WIDGET(icon));
	gtk_toolbar_insert(GTK_TOOLBAR(close), item, -1);
	g_signal_connect(G_OBJECT(item), "clicked", G_CALLBACK(cb_switchbar_close_clicked), item);
	
	gtk_box_pack_start(GTK_BOX(hbox), switchbar, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), close, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

	gtk_widget_show(switchbar);
	gtk_widget_show(close);
	gtk_widget_show(hbox);
	gtk_widget_show(vbox);

	return vbox;
}

GtkToolItem *
switchbar_item_new(char *name, int state, int type)
{
	GtkWidget *label;
	GtkToolItem *item;
	GSList *group = NULL;

	if(gtk_toolbar_get_n_items(GTK_TOOLBAR(switchbar)) > 0)
		group = gtk_radio_tool_button_get_group(GTK_RADIO_TOOL_BUTTON(gtk_toolbar_get_nth_item(GTK_TOOLBAR(switchbar), 0)));
	
	item = gtk_radio_tool_button_new(group);

	label = gtk_label_new(name);
	gtk_tool_button_set_label_widget(GTK_TOOL_BUTTON(item), label);
	gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(item), switchbar_get_icon(state, type));
	gtk_tool_item_set_is_important(GTK_TOOL_ITEM(item), TRUE);
	gtk_toolbar_insert(GTK_TOOLBAR(switchbar), item, -1);
	
	g_signal_connect(G_OBJECT(item), "toggled", G_CALLBACK(cb_switchbar_clicked), item);

	gtk_widget_show(label);
	gtk_widget_show(GTK_WIDGET(item));

	return item;
}

GtkWidget *
switchbar_get_icon(int state, int type)
{
	GtkWidget *icon;

	if(state == REGEN_WINDOW_STATE_HIGHLIGHTED)
	{
		if(type == REGEN_WINDOW_TYPE_CHANNEL)
			icon = gtk_image_new_from_file(get_regen_dir_plus("icons/channel-hl.png"));
		else if(type == REGEN_WINDOW_TYPE_QUERY)
			icon = gtk_image_new_from_file(get_regen_dir_plus("icons/query-hl.png"));
		else
			icon = gtk_image_new_from_file(get_regen_dir_plus("icons/window-hl.png"));
	}
	else
	{
		if(type == REGEN_WINDOW_TYPE_CHANNEL)
			icon = gtk_image_new_from_file(get_regen_dir_plus("icons/channel.png"));
		else if(type == REGEN_WINDOW_TYPE_QUERY)
			icon = gtk_image_new_from_file(get_regen_dir_plus("icons/query.png"));
		else
			icon = gtk_image_new_from_file(get_regen_dir_plus("icons/window.png"));
	}
	gtk_widget_show(icon);
	return icon;
}

void
switchbar_item_remove(GtkToolItem *item)
{
	gtk_container_remove(GTK_CONTAINER(switchbar), GTK_WIDGET(item));
}

void
switchbar_set_active(GtkToolItem *item)
{
	gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(item), TRUE);
}

/* FIXME if i can't get the fucking text highlighting (red) to work then we'll do icon highlighting */
void
switchbar_set_highlight(GtkToolItem *item, int state)
{
	window *w = window_find_by_switchbar_item(item); /* massively idiotic roundabout way of doing this but our item 
							    doesn't store useful info other than its name */
	gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(item), switchbar_get_icon(state, w->type));
	w->state = state;
}

void
cb_switchbar_clicked(GtkWidget *widget, gpointer data)
{
	if(gtk_toggle_tool_button_get_active(data))
		window_set_active(window_find_by_switchbar_item(data));
}

void
cb_switchbar_close_clicked(GtkWidget *widget, gpointer data)
{
	window *w = window_get_active();

	if(w->type == REGEN_WINDOW_TYPE_CHANNEL)
		irc_part_channel(w->_server, w->name, NULL);
	else
		window_destroy(w);
}	
