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
#include <string.h>

#include "regen.h"
#include "server.h"
#include "irc.h"

void
irc_join_channel(server *s, char *channel, char *key)
{
	if(key == NULL)
		server_sendf(s, "JOIN %s", channel);
	else
		server_sendf(s, "JOIN %s :%s", channel, key);
}

void
irc_part_channel(server *s, char *channel, char *msg)
{
	if(msg == NULL)
		server_sendf(s, "PART %s", channel);
	else
		server_sendf(s, "PART %s :%s", channel, msg);
}

void
irc_privmsg(server *s, char *to, char *msg)
{
	server_sendf(s, "PRIVMSG %s :%s", to, msg);
}

void
irc_notice(server *s, char *to, char *msg)
{
	server_sendf(s, "NOTICE %s :%s", to, msg);
}

void
irc_whois(server *s, char *who)
{
	server_sendf(s, "WHOIS %s", who);
}

void
irc_whowas(server *s, char *who, int count)
{
	server_sendf(s, "WHOWAS %s %i", who, count);
}

void
irc_nick(server *s, char *nick)
{
	server_sendf(s, "NICK %s", nick);
}

void
irc_user(server *s, char *username, char *hostname, char *servername, char *realname)
{
	server_sendf(s, "USER %s %s %s :%s", username, hostname, servername, realname);
}

void
irc_quit(server *s, char *msg)
{
	server_sendf(s, "QUIT :%s", msg);
}

void
irc_oper(server *s, char *username, char *password)
{
	server_sendf(s, "OPER %s %s", username, password);
}

void
irc_mode(server *s, char *target, char *modes)
{
	server_sendf(s, "MODE %s %s", target, modes);
}

void
irc_rawmode(server *s, char *modestring)
{
	server_sendf(s, "MODE %s", modestring);
}

void
irc_topic(server *s, char *channel, char *topic)
{
	if(topic == NULL)
		server_sendf(s, "TOPIC %s", channel);
	else
		server_sendf(s, "TOPIC %s :%s", channel, topic);
}

void
irc_kick(server *s, char *channel, char *who, char *msg)
{
	if(msg == NULL)
		server_sendf(s, "KICK %s %s", channel, who);
	else
		server_sendf(s, "KICK %s %s :%s", channel, who, msg);
}

void
irc_kill(server *s, char *who, char *msg)
{
	server_sendf(s, "KILL %s :%s", who, msg);
}

void
irc_pong(server *s, char *name)
{
	server_sendf(s, "PONG %s", name);
}

void
irc_away(server *s, char *msg)
{
	if(msg == NULL)
		server_send(s, "AWAY");
	else
		server_sendf(s, "AWAY :%s", msg);
}

void
irc_rehash(server *s)
{
	server_send(s, "REHASH");
}

void
irc_restart(server *s)
{
	server_send(s, "RESTART");
}

void
irc_wallops(server *s, char *msg)
{
	server_sendf(s, "WALLOPS :%s", msg);
}

void
irc_ison(server *s, char *nicks)
{
	server_sendf(s, "ISON %s", nicks);
}

void
irc_ctcp(server *s, char *target, char *ctcp, char *extra)
{
	if(strlen(extra) < 1)
		server_sendf(s, "PRIVMSG %s :\1%s\1", target, g_ascii_strup(ctcp, -1));
	else
		server_sendf(s, "PRIVMSG %s :\1%s %s\1", target, g_ascii_strup(ctcp, -1), extra);
}

void
irc_ctcp_reply(server *s, char *target, char *ctcp, char *extra)
{
	if(strlen(extra) < 1)
		server_sendf(s, "NOTICE %s :\1%s\1", target, g_ascii_strup(ctcp, -1));
	else
		server_sendf(s, "NOTICE %s :\1%s %s\1", target, g_ascii_strup(ctcp, -1), extra);
}

