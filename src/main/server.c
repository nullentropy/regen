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

#include "regen.h"
#include "config.h"
#include "server.h"
#include "channel.h"
#include "user.h"
#include "../gui/window.h"

static GList *server_list = NULL;

server *
server_new()
{
	server *s = (server *)malloc(sizeof(server));

	/* intialize some defaults */
	s->connected = FALSE;
	s->channel_list = NULL;
	s->user_list = NULL;
	s->me = user_new(config_get(REGEN_CONFIG_NICK));
	server_add_user(s, s->me);
	user_set_ident(s->me, config_get(REGEN_CONFIG_IDENT));
	user_set_host(s->me, "asuka"); /* completely useless what we send here */
	user_set_realname(s->me, config_get(REGEN_CONFIG_REALNAME));
	
	/* add the new server to the list */
	server_add(s);

	return s;
}

void
server_add(server *s)
{
	server_list = g_list_append(server_list, s);
}

server *
server_get_active()
{
	return window_get_active()->_server;
}

void
server_set_port(server *s, int port)
{
	s->port = port;
}

void
server_set_host(server *s, char *host)
{
	strcpy(s->host, host);
}

int 
server_get_port(server *s)
{
	return s->port;
}

char *
server_get_host(server *s)
{
	return s->host;
}

void
server_add_channel(server *s, channel *c)
{
	s->channel_list = g_list_append(s->channel_list, c);
}

void
server_remove_channel(server *s, channel *c)
{
	s->channel_list = g_list_remove(s->channel_list, c);
}

void
server_add_user(server *s, user *u)
{
	s->user_list = g_list_append(s->user_list, u);
}

user *
server_find_user_by_nick(server *s, char *nick)
{
	GList *ul = g_list_first(s->user_list);
	user *u;
	
	while(ul != NULL)
	{
		u = ul->data;
		if(!strcmp(nick, u->nick))
			return u;
		ul = g_list_next(ul);
	}
	return NULL;
}

void
server_print_users(server *s)
{
	GList *ul = g_list_first(s->user_list);
	user *u;
	printf("user list:\n");
	while(ul != NULL)
	{
		u = ul->data;
		printf("%s\n", u->nick);
		ul = g_list_next(ul);
	}
	printf("\n");
}

channel *
server_find_channel_by_name(server *s, char *name)
{
	GList *cl;
	channel *c;

	cl = g_list_first(s->channel_list);
	while(cl != NULL)
	{
		c = (channel *)cl->data;
		if(!strcmp(c->name, name))
			return c;
		cl = g_list_next(cl);
	}
	return NULL;
}

int
server_remove_user(server *s, user *u)
{
	GList *cl;
	channel *c;

	cl = g_list_first(s->channel_list);
	while(cl != NULL)
	{
		c = cl->data;
		if(channel_find_user(c, u) != NULL)
		{
			channel_nicklist_remove(c, u);
			window_nicklist_update(window_find_by_name(s, c->name));
		}
		cl = g_list_next(cl);
	}
}

