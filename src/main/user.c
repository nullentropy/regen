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

user *
user_new(char *nick)
{
	user *u = (user *)malloc(sizeof(user));

	strncpy(u->nick, nick, sizeof(u->nick));

	return u;
}

channel_user *
channel_user_new(user *u)
{
	channel_user *cu = (channel_user *)malloc(sizeof(channel_user));
	cu->u = u;
	return cu;
}

void
user_set_nick(user *u, char *nick)
{
	strncpy(u->nick, nick, sizeof(u->nick));
}

void
user_set_ident(user *u, char *ident)
{
	strncpy(u->ident, ident, sizeof(u->ident));
}

void
user_set_host(user *u, char *host)
{
	strncpy(u->host, host, sizeof(u->host));
}

void
user_set_realname(user *u, char *c)
{
	strncpy(u->realname, c, sizeof(u->realname));
}

char *
user_nicklist_format(channel_user *u)
{
	char *r = (char *)malloc(sizeof(char) * (strlen(u->u->nick) + 2));
	*r = '\0';
	if(u->op == TRUE)
		strcpy(r, "@");
	else if(u->voice == TRUE)
		strcpy(r, "+");
	strcat(r, u->u->nick);
	return r;
}
