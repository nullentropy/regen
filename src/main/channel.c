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
#include "channel.h"
#include "server.h"
#include "user.h"

channel *
channel_new(char *name)
{
	channel *chan = (channel *)malloc(sizeof(channel));
	
	strncpy(chan->name, name, sizeof(chan->name));
	strncpy(chan->topic, "", sizeof(chan->topic));
	chan->nick_list = NULL;

	return chan;
}

void
channel_nicklist_add(channel *c, channel_user *u)
{
	c->nick_list = g_list_insert_sorted(c->nick_list, u, (GCompareFunc)channel_nicklist_sort_func);
}

void
channel_nicklist_remove(channel *c, channel_user *u)
{
	c->nick_list = g_list_remove(c->nick_list, u);
}

void
channel_nicklist_sort(channel *c)
{
	c->nick_list = g_list_sort(c->nick_list, (GCompareFunc)channel_nicklist_sort_func);
}

gint
channel_nicklist_sort_func(channel_user *a, channel_user *b)
{
	if(a->op == TRUE)
	{
		if(b->op == TRUE)
			return strcmp(a->u->nick, b->u->nick);
		else if(b->voice == TRUE)
			return -1;
		else
			return -1;
	}
	else if(a->voice == TRUE)
	{
		if(b->op == TRUE)
			return 1;
		else if(b->voice == TRUE)
			return strcmp(a->u->nick, b->u->nick);
		else
			return -1;
	}
	else
	{
		if(b->op == TRUE)
			return 1;
		else if(b->voice == TRUE)
			return 1;
		else
			return strcmp(a->u->nick, b->u->nick);
	}
}

channel_user *
channel_find_user_by_nick(channel *c, char *nick)
{
	GList *list = g_list_first(c->nick_list);
	channel_user *u;
	
	while(list != NULL)
	{
		u = list->data;
		if(!strcmp(u->u->nick, nick))
			return u;
		list = g_list_next(list);
	}
	return NULL;
}

channel_user *
channel_find_user(channel *c, user *u)
{
	GList *ul = g_list_first(c->nick_list);
	channel_user *cu;
	
	while(ul != NULL)
	{
		cu = ul->data;
		if(cu->u == u)
			return cu;
		ul = g_list_next(ul);
	}
	return NULL;
}

char *
channel_nicklist_no_op_to_string(channel *c)
{
	GList *ul = g_list_first(c->nick_list);
	channel_user *cu;
	char *string = (char *)malloc(sizeof(char) * 5012);
	*string = '\0';
	while(ul != NULL)
	{
		cu = ul->data;
		if(!is_me(server_get_active(), cu->u->nick) && cu->op == FALSE)
		{
			strcat(string, cu->u->nick);
			strcat(string, " ");
		}
		ul = g_list_next(ul);
	}
	strcat(string, "\0");
	return string;
}

char *
channel_nicklist_op_to_string(channel *c)
{
	GList *ul = g_list_first(c->nick_list);
	channel_user *cu;
	char *string = (char *)malloc(sizeof(char) * 5012);
	*string = '\0';
	while(ul != NULL)
	{
		cu = ul->data;
		if(!is_me(server_get_active(), cu->u->nick) && cu->op == TRUE)
		{
			strcat(string, cu->u->nick);
			strcat(string, " ");
		}
		ul = g_list_next(ul);
	}
	strcat(string, "\0");
	return string;
}

char *
channel_nicklist_no_voice_to_string(channel *c)
{
	GList *ul = g_list_first(c->nick_list);
	channel_user *cu;
	char *string = (char *)malloc(sizeof(char) * 5012);
	*string = '\0';
	while(ul != NULL)
	{
		cu = ul->data;
		if(!is_me(server_get_active(), cu->u->nick) && cu->voice == FALSE)
		{
			strcat(string, cu->u->nick);
			strcat(string, " ");
		}
		ul = g_list_next(ul);
	}
	strcat(string, "\0");
	return string;
}

char *
channel_nicklist_voice_to_string(channel *c)
{
	GList *ul = g_list_first(c->nick_list);
	channel_user *cu;
	char *string = (char *)malloc(sizeof(char) * 5012);
	*string = '\0';
	while(ul != NULL)
	{
		cu = ul->data;
		if(!is_me(server_get_active(), cu->u->nick) && cu->voice == TRUE)
		{
			strcat(string, cu->u->nick);
			strcat(string, " ");
		}
		ul = g_list_next(ul);
	}
	strcat(string, "\0");
	return string;
}

void
channel_nicklist_update(server *s, user *u)
{
	GList *cl = g_list_first(s->channel_list);
	channel *c;
	
	while(cl != NULL)
	{
		c = cl->data;
		if(channel_find_user(c, u) != NULL)
		{
			channel_nicklist_sort(c);
			window_nicklist_update(window_find_by_name(s, c->name));
		}
		cl = g_list_next(cl);
	}
}
