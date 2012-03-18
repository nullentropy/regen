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
#include <stdlib.h>

#include "regen.h"
#include "server.h"
#include "channel.h"
#include "commands.h"
#include "irc.h"
#include "util.h"
#include "../gui/window.h"
#include "../gui/display.h"

static struct command commands[] = {
	{ "server",	"/server <hostname> [port]",		cmd_server_connect },
	{ "msg",	"/msg <nick|channel> <message>",	cmd_msg },
	{ "disconnect",	"/disconnect [message]",		cmd_server_disconnect },
	{ "quit",	"/quit [message]",			cmd_server_disconnect },
	{ "exit",	"/exit heh",				cmd_exit },
	{ "join",	"/join <channel> [key]",		cmd_join },
	{ "part",	"/part [channel] [key]",		cmd_part },
	{ "whois",	"/whois <nick>",			cmd_whois },
	{ "nick",	"/nick <nick>",				cmd_nick },
	{ "quote",	"/quote <command>",			cmd_quote },
	{ "clear",	"/clear",				cmd_clear },
	{ "echo",	"/echo <stuff>",			cmd_echo },
	{ "debug",	"/debug <on|off>",			cmd_debug },
	{ "oper",	"/oper [username] <password>",		cmd_oper },
	{ "sv",		"/sv",					cmd_sv },
	{ "ctcp",	"/ctcp <nick> <type> [etc]",		cmd_ctcp },
	{ "query",	"/query <nick>",			cmd_query },
	{ "q",		"/q <nick>",				cmd_query },
	{ "op",		"/op [#channel] <nick>",		cmd_op },
	{ "deop",	"/deop [#channel] <nick>",		cmd_deop },
	{ "voice",	"/voice [#channel] <nick>",		cmd_voice },
	{ "devoice",	"/devoice [#channel] <nick>",		cmd_devoice },
	{ "mop",	"/mop [#channel]",			cmd_mop },
	{ "mdeop",	"/mdeop [#channel]",			cmd_mdeop },
	{ "mvoice",	"/mvoice [#channel]",			cmd_mvoice },
	{ "mdevoice",	"/mdevoice [#channel]",			cmd_mdevoice },
	{ "mv",		"/mv [#channel]",			cmd_mvoice },
	{ "mdv",	"/mdv [#channel]",			cmd_mdevoice },
	{ "mode",	"/mode <#channe|nick> <modestring>",	cmd_mode },
	{ "topic",	"/topic [#channel] [newtopic]",		cmd_topic },
	{ "kick",	"/kick [#channel] <nick> [msg]",	cmd_kick }
};
	
/* terribly inefficient but uh... meh */
static struct command *
find_command(char *cmd)
{
	int i;
	int x;
	struct command *c;
	
	x = sizeof(commands) / sizeof(struct command);
	
	for(i = 0;i < x;i++)
	{
		c = &commands[i];
		if(!strcmp(c->cmd, cmd))
			return c;
	}
	return NULL;
}

void
parse_command(char *input)
{
	struct command *c;
	char *word[REGEN_MAX_ARGS];
	char *args[REGEN_MAX_ARGS];
	char *line;
	char *err;
	int argc;
	int ret;
	
	line = get_words_from(input, 1);

	if(line == NULL)
	{
		argc = 0;
	}
	else
	{
		argc = split_words(strdup(line), args);
	}
	
	split_words(input, word);
	c = find_command((char *)&word[0][1]);
	
	if(c != NULL)
	{
		ret = c->callback(argc, args, line);
		if(ret < 0)
			display_syntax(window_get_active(), c->syntax);
	}
	else
	{
		err = (char *)malloc(sizeof(char) * (strlen(word[0]) + 30));
		sprintf(err, "unknown command (%s), try /help", word[0]);
		display_error(window_get_active(), err);
		free(err);
	}

	if(argc > 0)
		free(line);
}

static int
cmd_server_connect(int argc, char *args[], char *line)
{
	server *s;
	
	char *host;
	unsigned short port;

	if(argc < 1)
		return -1;
	
	host = args[0];
	
	if(argc < 2)
		port = 6667;
	else
		port = atoi(args[1]);

	s = server_get_active();

	if(s != NULL)
	{
		if(s->connected)
			server_disconnect(s);
	}
	else
	{
		s = server_new();
	}

	server_set_port(s, port);
	server_set_host(s, host);

	server_connect(s);
	window_get_active()->_server = s;

	return 0;
}

static int
cmd_server_disconnect(int argc, char *args[], char *line)
{
	server *s = server_get_active();
	
	if(argc < 2)
		irc_quit(s, "regen: Segmentation fault");
	else
		irc_quit(s, line);

	return 0;
}

static int
cmd_exit(int argc, char *args[], char *line)
{
	irc_quit(server_get_active(), "regen: Segmentation fault");
	gtk_main_quit();
}

static int
cmd_msg(int argc, char *args[], char *line)
{
	window *w;
	
	if(argc < 2)
		return -1;
	
	_cmd_privmsg(server_get_active(), args[0], get_words_from(line, 1));
	
	return 0;
}

static int
cmd_join(int argc, char *args[], char *line)
{
	server *s = server_get_active();
	window *w;
	
	if(argc < 1 || !is_channel(args[0]))
		return -1;

	w = window_find_by_name(s, args[0]);
	if(w != NULL)
		window_set_active(w);
	
	if(argc < 2)
		irc_join_channel(s, args[0], NULL);
	else
		irc_join_channel(s, args[0], args[1]);

	return 0;
}

static int
cmd_part(int argc, char *args[], char *line)
{
	window *w = window_get_active();
	
	if(argc < 1)
	{
		if(is_channel(w->name))
		{
			irc_part_channel(w->_server, w->name, NULL);
		}
		else
		{
			return -1;
		}
	}
	else if(argc < 2)
	{
		if(is_channel(args[0]))
		{
			irc_part_channel(w->_server, args[0], NULL);
		}
		else
		{
			return -1;
		}
	}
	else
	{
		if(is_channel(args[0]))
		{
			irc_part_channel(w->_server, args[0], get_words_from(line, 1));
		}
		else
		{
			irc_part_channel(w->_server, w->name, line);
		}
	}

	return 0;
}

static int
cmd_whois(int argc, char *args[], char *line)
{
	if(argc < 1)
		return -1;
	
	irc_whois(server_get_active(), args[0]);

	return 0;
}

static int
cmd_oper(int argc, char *args[], char *line)
{
	server *s = server_get_active();
	
	if(argc < 1)
		return -1;
	
	if(argc < 2)
		irc_oper(s, s->me->nick, args[0]);
	else
		irc_oper(s, args[0], args[1]);

	return 0;
}

static int
cmd_nick(int argc, char *args[], char *line)
{
	server *s = server_get_active();

	if(argc < 1)
		return -1;

	irc_nick(s, args[0]);

	return 0;
}

static int
cmd_quote(int argc, char *args[], char *line)
{
	server *s = server_get_active();

	if(argc < 1)
		return -1;

	server_send(s, line);

	return 0;
}

static int
cmd_ctcp(int argc, char *args[], char *line)
{
	irc_ctcp(server_get_active(), args[0], args[1], args[2]);

	return 0;
}

static int
cmd_clear(int argc, char *args[], char *line)
{
	window_clear_buffer(window_get_active());

	return 0;
}

static int
cmd_sv(int argc, char *args[], char *line)
{
	_cmd_privmsg(server_get_active(), window_get_active()->name, get_version());

	return 0;
}

static int
cmd_echo(int argc, char *args[], char *line)
{
	window *w = window_get_active();

	if(argc < 1)
		return -1;

	window_print(w, line);

	return 0;
}

static int
cmd_debug(int argc, char *args[], char *line)
{
	window *w;
	
	if(argc < 1)
		return -1;

	w = window_get_active();
	if(!strcasecmp(args[0], "on"))
	{
		_debug = TRUE;
		display_misc(w, "debug mode is now on");
	}
	else if(!strcasecmp(args[0], "off"))
	{
		_debug = FALSE;
		display_misc(w, "debug mode is now off");
	}
	else
	{
		return -1;
	}
	
	return 0;
}

static int
cmd_query(int argc, char *args[], char *line)
{
	window *w;

	if(argc < 1)
		return -1;
	
	w = window_find_by_name(server_get_active(), args[0]);
	
	if(w != NULL)
		window_set_active(w);
	else
		window_new(server_get_active(), args[0], REGEN_WINDOW_STATE_NORMAL, REGEN_WINDOW_TYPE_QUERY);

	return 0;
}
		
static int
cmd_op(int argc, char *args[], char *line)
{
	if(argc < 1)
		return -1;

	if(is_channel(args[0]) && argc < 2)
		return -1;

	if(is_channel(args[0]))
		_cmd_chanusermode(server_get_active(), args[0], REGEN_MODE_GIVE, "o", get_words_from(line, 1));
	else
		_cmd_chanusermode(server_get_active(), window_get_active()->name, REGEN_MODE_GIVE, "o", get_words_from(line, 0));

	return 0;
}

static int
cmd_deop(int argc, char *args[], char *line)
{
	if(argc < 1)
		return -1;

	if(is_channel(args[0]) && argc < 2)
		return -1;

	if(is_channel(args[0]))
		_cmd_chanusermode(server_get_active(), args[0], REGEN_MODE_TAKE, "o", get_words_from(line, 1));
	else
		_cmd_chanusermode(server_get_active(), window_get_active()->name, REGEN_MODE_TAKE, "o", get_words_from(line, 0));

	return 0;
}

static int
cmd_voice(int argc, char *args[], char *line)
{
	if(argc < 1)
		return -1;

	if(is_channel(args[0]) && argc < 2)
		return -1;

	if(is_channel(args[0]))
		_cmd_chanusermode(server_get_active(), args[0], REGEN_MODE_GIVE, "v", get_words_from(line, 1));
	else
		_cmd_chanusermode(server_get_active(), window_get_active()->name, REGEN_MODE_GIVE, "v", get_words_from(line, 0));

	return 0;
}

static int
cmd_devoice(int argc, char *args[], char *line)
{
	if(argc < 1)
		return -1;

	if(is_channel(args[0]) && argc < 2)
		return -1;

	if(is_channel(args[0]))
		_cmd_chanusermode(server_get_active(), args[0], REGEN_MODE_TAKE, "v", get_words_from(line, 1));
	else
		_cmd_chanusermode(server_get_active(), window_get_active()->name, REGEN_MODE_TAKE, "v", get_words_from(line, 0));

	return 0;
}

static int
cmd_mop(int argc, char *args[], char *line)
{
	channel *c;
	
	if(argc < 1 && !is_channel(window_get_active()->name))
		return -1;
	
	c = server_find_channel_by_name(server_get_active(), window_get_active()->name);

	_cmd_chanusermode(server_get_active(), c->name, REGEN_MODE_GIVE, "o", channel_nicklist_no_op_to_string(c));
}

static int
cmd_mdeop(int argc, char *args[], char *line)
{
	channel *c;
	if(argc < 1 && !is_channel(window_get_active()->name))
		return -1;

	c = server_find_channel_by_name(server_get_active(), window_get_active()->name);

	_cmd_chanusermode(server_get_active(), c->name, REGEN_MODE_TAKE, "o", channel_nicklist_op_to_string(c));
}

static int
cmd_mvoice(int argc, char *args[], char *line)
{
	channel *c;
	if(argc < 1 && !is_channel(window_get_active()->name))
		return -1;

	c = server_find_channel_by_name(server_get_active(), window_get_active()->name);

	_cmd_chanusermode(server_get_active(), c->name, REGEN_MODE_GIVE, "v", channel_nicklist_no_voice_to_string(c));
}

static int
cmd_mdevoice(int argc, char *args[], char *line)
{
	channel *c;
	if(argc < 1 && !is_channel(window_get_active()->name))
		return -1;

	c = server_find_channel_by_name(server_get_active(), window_get_active()->name);

	_cmd_chanusermode(server_get_active(), c->name, REGEN_MODE_TAKE, "v", channel_nicklist_voice_to_string(c));
}

static int
cmd_mode(int argc, char *args[], char *line)
{
	/* /mode #chan +o nick etc */
	if(argc < 1)
		return -1;

	irc_rawmode(server_get_active(), line);
}

static int
cmd_topic(int argc, char *args[], char *line)
{
	window *w = window_get_active();
	if(argc < 1)
	{
		if(!is_channel(w->name))
			return -1;

		irc_topic(w->_server, w->name, NULL);
	}
	else if(is_channel(args[0]))
	{
		if(argc < 2)
			irc_topic(w->_server, args[0], NULL);
		else
			irc_topic(w->_server, w->name, get_words_from(line, 1));
	}
	else
	{
		irc_topic(w->_server, w->name, line);
	}
	return 0;
}

static int
cmd_kick(int argc, char *args[], char *line)
{
	window *w = window_get_active();

	if(argc < 1)
		return -1;

	if(is_channel(args[0]))
	{
		if(argc < 2)
			return -1;
		else if(argc < 3)
			irc_kick(w->_server, args[0], args[1], NULL);
		else
			irc_kick(w->_server, args[0], args[1], get_words_from(line, 2));
	}
	else
	{
		if(argc < 2)
			irc_kick(w->_server, w->name, args[0], NULL);
		else
			irc_kick(w->_server, w->name, args[0], get_words_from(line, 1));
	}
	return 0;
}

int
_cmd_privmsg(server *s, char *target, char *message)
{
	window *w = window_find_by_name(s, target);
	char *buffer;

	buffer = strdup(message);
	
	char delims[] = "\r\n";
	char *result = NULL;
	
	result = (char *)strtok(buffer, delims);
	
	while(result != NULL)
	{
		irc_privmsg(s, target, result);
		if(is_channel(target))
			display_privmsg_channel_me(s, server_find_channel_by_name(s, target), result);
		result = (char *)strtok(NULL, delims);
	}

	free(result);
	free(buffer);
}

int
_cmd_chanusermode(server *s, char *channel, char giveortake, char *mode, char *users)
{
	char nicklist[239];
	char modelist[16];
	char modestring[256];
	char *nicks[REGEN_MAX_ARGS];
	int i = split_words(users, nicks) - 1, x, mpl = 4;

	for(;;)
	{
		modelist[0] = '\0';
		nicklist[0] = '\0';
		for(x = 0;x < mpl;x++)
		{
			if(i == -1)
			{
				snprintf(modestring, sizeof(modestring), "%c%s%s", giveortake, modelist, nicklist);
				irc_mode(s, channel, modestring);
				return 0;
			}
			
			strncat(modelist, mode, sizeof(modelist) - strlen(mode) - 1);
			strncat(nicklist, " ", sizeof(nicklist) - 2);
			strncat(nicklist, nicks[i--], sizeof(nicklist) - strlen(nicks[i]) - 1);
		}
		snprintf(modestring, sizeof(modestring), "%c%s%s", giveortake, modelist, nicklist);
		irc_mode(s, channel, modestring);
		
		if(i == -1)
			return 0;
	}
	return 0;
}

