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
#include "../main/server.h"
#include "../main/config.h"
#include "gui.h"
#include "window.h"
#include "switchbar.h"

static GtkItemFactoryEntry menu_items[] = {
  { "/_File",			NULL,		NULL,				0,	"<Branch>" },
  { "/File/_Connect",		NULL,		NULL,				0,	"<Item>", },
  { "/File/_Disconnect",	NULL,		NULL,				0,	"<Item>", },
  { "/File/_Quit",		"<CTRL>Q",	gtk_main_quit,			0,	"<StockItem>",	GTK_STOCK_QUIT },
  { "/_DCC",			NULL,		NULL,				0,	"<Branch>", },
  { "/DCC/Send",		NULL,		NULL,				0,	"<Item>", },
  { "/DCC/Chat",		NULL,		NULL,				0,	"<Item>", },
  { "/_Tools",			NULL,		NULL,				0,	"<Branch>" },
  { "/Tools/Options",		NULL,		NULL,				0,	"<Item>", },
  { "/_Help",			NULL,		NULL,				0,	"<Branch>" },
  { "/_Help/About",		NULL,		cb_toolbar_about_clicked,	0,	"<Item>" } 
};

static const gint nmenu_items = sizeof (menu_items) / sizeof (menu_items[0]);
static GtkWidget *notebook;
static GtkWidget *main_window;

void
create_main_window()
{
	GtkWidget *vbox, *vbox_status;

	GtkWidget *menubar;
	GtkWidget *toolbar;
	GtkWidget *switchbar;

	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(main_window), APP_NAME);
	gtk_widget_set_size_request(GTK_WIDGET(main_window), 650, 450); /* FIXME: have it remember size */
	gtk_window_set_resizable(GTK_WINDOW(main_window), TRUE);

	g_signal_connect(G_OBJECT(main_window), "delete_event", G_CALLBACK(cb_delete_event), NULL);
	g_signal_connect(G_OBJECT(main_window), "destroy", G_CALLBACK(cb_destroy), NULL);

	vbox        = gtk_vbox_new(FALSE, 0);
	vbox_status = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(main_window), vbox);
	
	notebook = gtk_notebook_new();
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), FALSE);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), FALSE);

	menubar   = create_menubar();
	toolbar   = create_toolbar();
	switchbar = create_switchbar();

	statusbar = gtk_statusbar_new();
	gtk_box_pack_start(GTK_BOX(vbox_status), statusbar, TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(vbox), menubar, 	FALSE, 	FALSE, 	0);
	gtk_box_pack_start(GTK_BOX(vbox), toolbar, 	FALSE, 	FALSE, 	0);
	gtk_box_pack_start(GTK_BOX(vbox), switchbar, 	FALSE, 	FALSE, 	0);
	gtk_box_pack_start(GTK_BOX(vbox), notebook,	TRUE, 	TRUE, 	0);
	gtk_box_pack_start(GTK_BOX(vbox), vbox_status,	FALSE, 	TRUE, 	0);

	gtk_widget_show(vbox);
	gtk_widget_show(vbox_status);
	gtk_widget_show(statusbar);
	gtk_widget_show(notebook);
	gtk_widget_show(main_window);

	window_new(NULL, "Status Window", REGEN_WINDOW_STATE_NORMAL, REGEN_WINDOW_TYPE_STATUS);
}

GtkWidget *
create_toolbar()
{
	GtkWidget *vbox;
	GtkWidget *toolbar;
	GtkWidget *button;
	GtkWidget *icon;
	GtkToolItem *item;
	
	vbox = gtk_vbox_new(FALSE, 0);
	
	toolbar = gtk_toolbar_new();
	gtk_toolbar_set_orientation(GTK_TOOLBAR(toolbar), GTK_ORIENTATION_HORIZONTAL);
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
	
	icon = gtk_image_new_from_file(get_regen_dir_plus("icons/connect.png")); 
	item = gtk_tool_button_new(icon, NULL);
	gtk_widget_show(GTK_WIDGET(item));
	gtk_widget_show(GTK_WIDGET(icon));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), item, -1);

	item = gtk_separator_tool_item_new();
	gtk_widget_show(GTK_WIDGET(item));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), item, -1);
	
	icon = gtk_image_new_from_file(get_regen_dir_plus("icons/dict.png")); 
	item = gtk_tool_button_new(icon, NULL);
	gtk_widget_show(GTK_WIDGET(item));
	gtk_widget_show(GTK_WIDGET(icon));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), item, -1);

	icon = gtk_image_new_from_file(get_regen_dir_plus("icons/options.png")); 
	item = gtk_tool_button_new(icon, NULL);
	gtk_widget_show(GTK_WIDGET(item));
	gtk_widget_show(GTK_WIDGET(icon));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), item, -1);
	
	item = gtk_separator_tool_item_new();
	gtk_widget_show(GTK_WIDGET(item));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), item, -1);
	
	icon = gtk_image_new_from_file(get_regen_dir_plus("icons/about.png")); 
	item = gtk_tool_button_new(icon, NULL);
	gtk_widget_show(GTK_WIDGET(item));
	gtk_widget_show(GTK_WIDGET(icon));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), item, -1);
	g_signal_connect(G_OBJECT(item), "clicked", G_CALLBACK(cb_toolbar_about_clicked), item);

	gtk_box_pack_start(GTK_BOX(vbox), toolbar, TRUE, TRUE, 0);

	gtk_widget_show(toolbar);
	gtk_widget_show(vbox);
	
	return vbox;
}
	
GtkWidget *
create_menubar()
{
	GtkWidget *vbox;
	GtkItemFactory *item_factory;
	GtkAccelGroup *accel_group;
	GtkWidget *menu_bar;
	
	vbox = gtk_vbox_new(FALSE, 0);
	
	/* Make an accelerator group (shortcut keys) */
	accel_group = gtk_accel_group_new ();

	/* Make an ItemFactory (that makes a menubar) */
	item_factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", accel_group);

	/* This function generates the menu items. Pass the item factory,
	the number of items in the array, the array itself, and any
	callback data for the the menu items. */
	gtk_item_factory_create_items (item_factory, nmenu_items, menu_items, NULL);

	/* Attach the new accelerator group to the window. */
	//gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);

	menu_bar = gtk_item_factory_get_widget(item_factory, "<main>");
	
	gtk_widget_show(menu_bar);
	gtk_widget_show(vbox);
	
	gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);

	return vbox;
}

gboolean 
cb_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	return FALSE;
}

void 
cb_destroy(GtkWidget *widget, gpointer data)
{
	irc_quit(server_get_active(), regen_get_quit_msg());
	gtk_main_quit();
}

void 
cb_toolbar_about_clicked(GtkWidget *widget, gpointer data)
{
	GtkWidget *about;

	about = gtk_about_dialog_new();
	
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about), APP_VERSION);
	gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(about), APP_NAME);
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about), "");
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about), "This code is in development, use at your own risk. ^_^");
	gtk_widget_show(about);
}

GtkNotebook *
regen_get_notebook()
{
	return GTK_NOTEBOOK(notebook);
}

void
regen_update_title(void)
{
	window *w = window_get_active();
	channel *c;
	if(w->type == REGEN_WINDOW_TYPE_CHANNEL)
	{
		c = server_find_channel_by_name(w->_server, w->name);
		if(c != NULL)
		{
			if(strlen(c->topic) > 1)
				regen_set_title("[%s(): %s] [%s(%s): %s]", w->_server->me->nick, w->_server->host, w->name, c->modes, c->topic);
			else
				regen_set_title("[%s(): %s] [%s(%s)]", w->_server->me->nick, w->_server->host, w->name, c->modes);
		}
	}
	else
		regen_set_title("[%s(): %s]", w->_server->me->nick, w->_server->host);
}

void
regen_set_title(char *fmt, ...)
{
	va_list args;
	char *buf;

	va_start(args, fmt);
	buf = g_strdup_vprintf(fmt, args);
	va_end(args);
	
	strncpy(title, "regen ", sizeof(title));
	strcat(title, buf);
	gtk_window_set_title(GTK_WINDOW(main_window), title);
	
	g_free(buf);
}
