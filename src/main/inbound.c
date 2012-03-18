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
#include <errno.h>
#include <string.h>

#include "regen.h"
#include "server.h"
#include "inbound.h"
#include "channel.h"
#include "user.h"
#include "util.h"
#include "../gui/window.h"

void
in_server_notice(server *s, char *msg)
{
	window *w = window_find_by_name(s, "Status Window");
	window_printf(w, "[%s] %s\n", s->host, msg);
}

void
in_notice(server *s, user *from, char *target, char *msg)
{
	window *w = window_get_active();
	window_printf(w, "[%s] %s\n", from->nick, msg);
}

void
in_server_ping(server *s, char *stuff)
{
	irc_pong(s, stuff);
}

void
in_nick(server *s, user *u, char *newnick)
{
	if(is_me(s, u->nick))
		display_nick_me(window_get_active(), u->nick, newnick); /* FIXME need to output this to all the chans im in */
	else
		display_nick(window_get_active(), u->nick, newnick);
	
	user_set_nick(u, newnick);
	channel_nicklist_update(s, u);
}

void
in_privmsg_channel(server *s, channel *c, channel_user *from, char *msg)
{
	display_privmsg_channel(s, c, from, msg);
}

void
in_privmsg_channel_outside(server *s, channel *c, user *from, char *msg)
{
}

void
in_wall_channel(server *s, channel *c, channel_user *from, char *msg)
{
}

void
in_privmsg(server *s, user *from, char *msg)
{
	window *w = window_find_by_name(s, from->nick);
	if(w == NULL)
		w = window_new(s, from->nick, REGEN_WINDOW_STATE_HIGHLIGHTED, REGEN_WINDOW_TYPE_QUERY);
	display_privmsg(w, from, msg);
}

void
in_ctcp(server *s, user *u, char *target, char *msg)
{
	char *d = strdup(msg);
	char *word[REGEN_MAX_ARGS];
	char *ctcp;
	
	split_words(d, word);
	ctcp = ++word[0];
	ctcp[strlen(ctcp)-1] = '\0';
	
	if(!strcasecmp(ctcp, "version"))
	{
		irc_ctcp_reply(s, u->nick, "version", get_version());
	}
	
	window_printf(window_get_active(), "received ctcp %s from %s\n", ctcp, u->nick);
}

void
in_ctcp_reply(server *s, user *u, char *msg)
{
	char *rep = substr(msg, 1, strlen(msg) - 1);
	char *dupe = strdup(rep);
	char *word[REGEN_MAX_ARGS];
	char *ctcp;
	
	split_words(dupe, word);
	ctcp = word[0];
	
	window_printf(window_get_active(), "ctcp %s reply from %s: %s\n", ctcp, u->nick, get_words_from(rep, 1));
}

void
in_join(server *s, user *u, channel *c)
{
	window *w = window_find_by_name(s, c->name);

	if(is_me(s, u->nick))
	{
		server_add_channel(s, c);
		window_new(s, c->name, REGEN_WINDOW_STATE_NORMAL, REGEN_WINDOW_TYPE_CHANNEL);
		irc_mode(s, c->name, "");
	}
	else
	{
		channel_nicklist_add(c, channel_user_new(u));
		window_nicklist_update(w);
		window_printf(w, "··· join: %s (%s@%s)\n", u->nick, u->ident, u->host);
	}
}

void
in_part(server *s, channel_user *u, channel *c, char *msg)
{
	printf("k... seriously\n");
	window *w = window_find_by_name(s, c->name);
	if(is_me(s, u->u->nick))
	{
		window_destroy(w);
		server_remove_channel(s, c);
	}
	else
	{
		channel_nicklist_remove(c, u);
		window_nicklist_update(w);
		window_printf(w, "··· part: %s (%s@%s)\n", u->u->nick, u->u->ident, u->u->host);
	}
}

void
in_quit(server *s, user *u, char *msg)
{
	server_remove_user(s, u);
}

void
in_kick_server(server *s, char *server_name, channel *c, channel_user *target, char *msg)
{
	_in_kick(s, server_name, c, target, msg);
}

void
in_kick(server *s, channel_user *from, channel *c, channel_user *target, char *msg)
{
	_in_kick(s, from->u->nick, c, target, msg);
}

void
_in_kick(server *s, char *from, channel *c, channel_user *target, char *msg)
{
	window *w = window_find_by_name(s, c->name);
	
	channel_nicklist_remove(c, target);

	window_printf(w, "%s kicked by %s\n (%s)", target->u->nick, from, msg);
}

void
in_topic(server *s, channel_user *u, channel *c, char *topic)
{
	strncpy(c->topic, topic, sizeof(c->topic));
	regen_update_title();
}

void
in_mode(server *s, channel_user *from, channel *c, char *mode, char *target)
{
	_in_mode(s, from->u->nick, c, mode, target);
}

void
in_mode_server(server *s, char *server_name, channel *c, char *mode, char *target)
{
	_in_mode(s, server_name, c, mode, target);
}

void
_in_mode(server *s, char *from, channel *c, char *mode, char *target)
{
	window *w = window_find_by_name(s, c->name);
	int y = 0;
	char *targets[REGEN_MAX_ARGS],
	     *d_target,
	     *m;
	char giveortake;
	channel_user *u;
	
	if(target != NULL)
	{
		d_target = strdup(target);
		split_words(target, targets);
		for(m = mode;*m != '\0';m++)
		{
			if(*m == REGEN_MODE_GIVE || *m == REGEN_MODE_TAKE)
			{
				giveortake = *m;
			}
			else
			{
				if(*m == 'o' || *m == 'v')
				{
					u = channel_find_user_by_nick(c, targets[y]);
					if(giveortake == REGEN_MODE_GIVE)
					{
						if(*m == 'o')
							u->op = TRUE;
						else
							u->voice = TRUE;
					}
					else
					{
						if(*m == 'o')
							u->op = FALSE;
						else
							u->voice = FALSE;
					}
					y++;
				}
			}
					
		}
		window_printf(w, "··· mode %s [%s] by %s\n", mode, d_target, from);
		channel_nicklist_sort(c);
		window_nicklist_update(w);
	}
	else
	{
		window_printf(w, "··· mode %s by %s\n", mode, from);
	}
}

void
in_mode_me(server *s, char *modes)
{
	window *w = window_find_by_name(s, "Status Window");

	window_printf(w, "··· mode %s [%s]\n", s->me->nick, modes);
}

void
in_server_numeric(server *s, int numeric, char *word[], char *line)
{
	window *w;
	
	char *msg = get_msg(line);
	char *slabel = (char *)malloc(sizeof(char) * 255);
	sprintf(slabel, "[%s]", server_get_host(s));

	w = window_find_by_name(s, "Status Window");
	
	switch(numeric)
	{
		case 001:
			{
				char *heh[REGEN_MAX_ARGS];
				int i = split_words(msg, heh);
				user_set_nick(s->me, heh[i - 1]);
			}
			window_printf(w, "%s %s\n", slabel, msg);
			break;
		case 002:
			window_printf(w, "%s %s\n", slabel, msg);
			break;
		case 003:
			window_printf(w, "%s %s\n", slabel, msg);
			break;
		case 004:
			window_printf(w, "%s %s\n", slabel, get_words_from(line, 3)); /* the 004 numeric doesn't have a : separator */
			break;
		case 005:
			window_printf(w, "%s %s\n", slabel, get_words_from(line, 3)); /* neither does the 005 for that matter */
			break;
		case 250:
			window_printf(w, "%s %s\n", slabel, msg);
			break;
		case 251:
			window_printf(w, "%s %s\n", slabel, msg);
			break;
		case 252:
			window_printf(w, "%s %s\n", slabel, msg);
			break;
		case 254:
			window_printf(w, "%s %s\n", slabel, msg);
			break;
		case 255:
			window_printf(w, "%s %s\n", slabel, msg);
			break;
		case 265:
			window_printf(w, "%s %s\n", slabel, msg);
			break;
		case 266:
			window_printf(w, "%s %s\n", slabel, msg);
			break;
		case 324:
			{
				channel *c = server_find_channel_by_name(s, word[3]);
				strncpy(c->modes, strip_whitespace(get_words_from(line, 4)), sizeof(c->modes));
				regen_update_title();
			}
			break;
		case 332: /* topic */
			{
				channel *c = server_find_channel_by_name(s, word[3]);
				strncpy(c->topic, get_msg(line), sizeof(c->topic));
				regen_update_title();
			break;
			}
		case 353:
			{
				gboolean op = FALSE;
				gboolean vo = FALSE;
				user *u;
				channel_user *cu;
				channel *c = server_find_channel_by_name(s, word[4]);

				char *moop = get_msg(line);
				char *mooop[REGEN_MAX_ARGS];
				int x = split_words(moop, mooop);
				int i;
				for(i = 0;i < x;i++)
				{
					if(*mooop[i] == '@')
					{
						op = TRUE;
						mooop[i]++;
					}
					else if(*mooop[i] == '+')
					{
						vo = TRUE;
						mooop[i]++;
					}
					u = server_find_user_by_nick(s, mooop[i]);
					if(u == NULL)
					{
						u = user_new(mooop[i]);
						server_add_user(s, u);
					}
					cu = channel_user_new(u);
					cu->op = op;
					cu->voice = vo;
					channel_nicklist_add(c, cu);
					op = FALSE;
					vo = FALSE;
				}
			}
			break;
		case 366:
			window_nicklist_update(window_find_by_name(s, word[3]));
			break;
		case 422:
			window_printf(w, "%s %s\n", slabel, msg);
			break;
		case 482:
			display_error(window_get_active(), "you are not a channel operator");
			break;
		default:
			window_printf(w, "unscripted raw: %s\n", line);
	}
}
