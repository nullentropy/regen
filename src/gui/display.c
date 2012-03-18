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
#include <time.h>
#include <string.h>

#include "../main/regen.h"
#include "../main/server.h"
#include "../main/config.h"
#include "../main/channel.h"
#include "gui.h"
#include "window.h"

/* for the themes, we're going to need to have some kind of table that stores:
 * - the name of the line
 * - the template to use for that line
 * 
 * then we're going to need a function that reads that, and replaces the symbols in the template with the actual 
 * information we've been passed 
 */

char *
display_thingie()
{
	return  "···";
}
	
char *
display_timestamp()
{
	char ret[11];
	char *tem = "[%H:%M:%S]";

	time_t t = time(0);
	strftime(ret, sizeof(ret), tem, localtime(&t));

	return strdup((char *)&ret);
}

void
display_privmsg_channel_me(server *s, channel *c, char *msg)
{
	channel_user *u = channel_find_user_by_nick(c, s->me->nick);
	window *w = window_find_by_name(s, c->name);

	if(u->op == TRUE)
		window_printf(w, "%s 17(00@%s17)00 %s\n", display_timestamp(), s->me->nick, msg);
	else if(u->voice == TRUE)
		window_printf(w, "%s 17(00+%s17)00 %s\n", display_timestamp(), s->me->nick, msg);
	else 
		window_printf(w, "%s 17(00%s17)00 %s\n", display_timestamp(), s->me->nick, msg);

}

void
display_privmsg_channel(server *s, channel *c, channel_user *u, char *msg)
{
	window *w = window_find_by_name(s, c->name);
	
	if(u->op == TRUE)
		window_printf(w, "%s 17[00@%s17]00 %s\n", display_timestamp(), u->u->nick, msg);
	else if(u->voice == TRUE)
		window_printf(w, "%s 17[00+%s17]00 %s\n", display_timestamp(), u->u->nick, msg);
	else
		window_printf(w, "%s 17[00%s17]00 %s\n", display_timestamp(), u->u->nick, msg);
}

void
display_privmsg(server *s, user *u, char *msg)
{
}

void
display_ctcp(window *w, char *nick, char *ident, char *host, char *ctcp, char *msg)
{
}

void
display_ctcp_reply(window *w, char *nick, char *ident, char *host, char *ctcp, char *msg)
{
}



void
display_error(window *w, char *message)
{
	window_printf(w, "error: %s\n", message);
}

void
display_syntax(window *w, char *buf)
{
	window_printf(w, "syntax: %s\n", buf);
}

void
display_misc(window *w, char *buf)
{
	window_printf(w, "regen: %s\n", buf);
}

void
display_nick_me(window *w, char *oldnick, char *newnick)
{
	window_printf(w, "--- %s is now %s\n", oldnick, newnick);
}

void
display_nick(window *w, char *oldnick, char *newnick)
{
	window_printf(w, "--- %s is now %s\n", oldnick, newnick);
}

