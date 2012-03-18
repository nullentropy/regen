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

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/fcntl.h>
#include <errno.h>
#include <string.h>

#include "regen.h"
#include "server.h"
#include "channel.h"
#include "user.h"
#include "network.h"
#include "inbound.h"
#include "util.h"
#include "config.h"
#include "../gui/window.h"
#include "../gui/display.h"


int
server_connect(server *s)
{
	GThread *t = g_thread_create((GThreadFunc)_server_connect, s, TRUE, NULL);
	return 0;
}


int
_server_connect(server *s)
{
	struct sockaddr_in addr;
	struct hostent *he;
	
	if((he = gethostbyname(s->host)) == NULL)
	{
		display_error(window_get_active(), "can't resolve hostname");
		g_thread_exit(NULL);
	}

	memcpy(&addr.sin_addr.s_addr, he->h_addr, he->h_length);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(s->port);
	
	s->socket = socket(PF_INET, SOCK_STREAM, 0);
	
	if(connect(s->socket, (struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		window_printf(window_get_active(), "error: %s\n", strerror(errno));
		g_thread_exit(NULL);
	}
	
	s->connected  = TRUE;
	s->connecting = TRUE;
	s->tag = gdk_input_add(s->socket, GDK_INPUT_READ, cb_server_input, s);
	
	g_thread_exit(NULL);
}

int
server_disconnect(server *s)
{
	if(s->connected)
	{
		/* try to cleanly quit the server... */
		irc_quit(s, NULL); 

		/* ...then just trash the connection */	
		s->connected = FALSE;
		g_source_remove(s->tag);
		return close(s->socket);
	}
	return 0;
}

static void
cb_server_input(gpointer data, gint source, GdkInputCondition condition)
{
	server *s = data;
	memset(s->input_buffer, '\0', sizeof(s->input_buffer));
	
	if(recv(s->socket, s->input_buffer, sizeof(s->input_buffer), 0) == 0)
	{
		/* guess we got disconnected, let's disconnect the signal handler too */
		g_source_remove(s->tag);
		close(s->socket);
		s->connected = FALSE;
	}
	else
	{
		if(s->connecting)
		{
			irc_user(s, s->me->ident, s->me->host, s->host, s->me->realname);
			irc_nick(s, s->me->nick);
			s->connecting = FALSE;
		}
		parse_server_input(s);
	}
}

/* split up what the server sends to us into individual lines so we can process them */
static int
parse_server_input(server *s)
{
	char *buffer;

	buffer = strdup(s->input_buffer);
	
	char delims[] = "\r\n";
	char *result = NULL;
	
	result = (char *)strtok(buffer, delims);
	
	while(result != NULL)
	{
		process_server_input(s, result);
		result = (char *)strtok(NULL, delims);
	}

	free(result);
}

/* process a single line of server input */
static int
process_server_input(server *s, char *line)
{
	char *d_line = strdup(line),
	     *word[REGEN_MAX_ARGS],
	     *user_info[3],
	     *u_c;
	user *u_from,
	     *u_target;
	channel *c;
	gboolean u_s = FALSE;

	if(_debug == TRUE)
		printf("<- %s\n", line);

	split_words(d_line, word);
	
	if(line[0] == ':')
	{
		u_c = (char *)&word[0][1];
		if(is_user(u_c))
		{
			parse_user(u_c, user_info);
			u_from = server_find_user_by_nick(s, user_info[0]);
			if(u_from == NULL)
			{
				u_from = user_new(user_info[0]);
				if(user_info[1] != NULL)
				{
					server_add_user(s, u_from);
					user_set_ident(u_from, user_info[1]);
					user_set_host(u_from, user_info[2]);
				}
			}
		}
		else
		{
			u_s = TRUE;
		}
		
		if(!strcmp(word[1], "NOTICE"))
		{
			char *msg = get_msg(line);
			if(msg[0] == '\1' && msg[strlen(msg)-1] == '\1')
			{
				in_ctcp_reply(s, u_from, msg);
			}
			else
			{
				if(u_s == TRUE)
					in_server_notice(s, msg);
				else
					in_notice(s, u_from, word[2], get_msg(line));
			}
		}
		else if(!strcmp(word[1], "PRIVMSG"))
		{
			char *msg = get_msg(line);
			if(msg[0] == '\1' && msg[strlen(msg)-1] == '\1')
				in_ctcp(s, u_from, word[2], msg);
			else
			{
				if(is_channel(word[2]))
				{
					c = server_find_channel_by_name(s, word[2]);
					in_privmsg_channel(s, c, channel_find_user(c, u_from), msg);
				}
				else
					in_privmsg(s, u_from, msg);
			}
		}
		else if(!strcmp(word[1], "JOIN"))
		{
			/* for some fucking reason not all servers put a : before the channel name */
			char *msg = get_msg(line);
			if(msg != NULL)
			{
				c = server_find_channel_by_name(s, msg);
				if(c == NULL)
					c = channel_new(msg);
			}
			else
			{
				c = server_find_channel_by_name(s, word[2]);
				if(c == NULL)
					c = channel_new(word[2]);
			}
			in_join(s, u_from, c);
		}
		else if(!strcmp(word[1], "PART"))
		{
			if(word[2][0] == ':')
				c = server_find_channel_by_name(s, (char *)&word[2][1]);
			else
				c = server_find_channel_by_name(s, word[2]);
			in_part(s, channel_find_user(c, u_from), c, get_msg(line));
		}
		else if(!strcmp(word[1], "KICK"))
		{
			c = server_find_channel_by_name(s, word[2]);
			if(u_s == TRUE)
				in_kick_server(s, u_c, c, channel_find_user_by_nick(c, word[3]), get_msg(line));
			else
				in_kick(s, channel_find_user(c, u_from), c, channel_find_user_by_nick(c, word[3]), get_msg(line));
		}
		else if(!strcmp(word[1], "QUIT"))
		{
			in_quit(s, u_from, get_msg(line));
		}
		else if(!strcmp(word[1], "MODE"))
		{
			if(is_me(s, word[2]))
			{
				in_mode_me(s, get_msg(line));
			}
			else
			{
				c = server_find_channel_by_name(s, word[2]);
				if(u_s == TRUE)
					in_mode_server(s, u_c, c, word[3], get_words_from(line, 4));
				else
					in_mode(s, channel_find_user(c, u_from), c, word[3], get_words_from(line, 4));
			}
		}
		else if(!strcmp(word[1], "NICK"))
		{
			in_nick(s, u_from, get_msg(line));
		}
		else if(!strcmp(word[1], "TOPIC"))
		{
			in_topic(s, u_from, server_find_channel_by_name(s, word[2]), get_msg(line));
		}
		else if(isdigit(word[1][0]))
		{
			in_server_numeric(s, atoi(word[1]), word, line);
		}
		else
		{
			window_printf(window_get_active(), "%s\n", line);
		}	
	}
	else
	{
		if(!strcmp(word[0], "PING"))
			in_server_ping(s, get_msg(line));
		else if(!strcmp(word[0], "NOTICE"))
			in_server_notice(s, get_msg(line));	
		else
			window_printf(window_get_active(), "%s\n", line);
	}
}

int
server_sendf(server *s, char *format, ...)
{
	va_list args;
	char *buf;

	va_start(args, format);
	buf = g_strdup_vprintf(format, args);
	va_end(args);
	
	server_send(s, buf);
	g_free(buf);
}

int
server_send(server *s, char *what)
{
	int ret;
	const gchar *set;
	char *locale;
	char *push;
	gsize len;
	
	if(s == NULL || !s->connected)
	{
		display_error(window_get_active(), "not connected to server");
		return -1;
	}
	
	g_get_charset(&set);
	locale = g_convert_with_fallback(what, -1, set, "UTF-8", "?", NULL, &len, NULL);
	
	push = (char *)malloc(sizeof(char) * 513);

	strncpy(push, locale, 510);
	strcat(push, "\r\n");
	
	if(_debug == TRUE)
		printf("-> %s\n", locale);	

	ret = send(s->socket, push, strlen(push), 0);
	
	free(locale);
	free(push);

	return ret;
}
