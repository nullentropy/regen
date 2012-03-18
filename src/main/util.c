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
#include "util.h"

/* the info array absolutely needs to have a size of at least 3 */
void
parse_user(char *buf, char *info[])
{
	char *ptr = buf;
	info[0] = buf;
	
	while(1)
	{
		switch(*ptr)
		{
			case 33:
				*ptr = '\0';
				info[1] = ++ptr;
				break;
			case 64:
				*ptr = '\0';
				info[2] = ++ptr;
				break;
			case 0:
				return;
			default:
				ptr++;
				break;
		}
	}
}

gboolean
is_channel(char *c)
{
	if(c[0] == '#')
		return TRUE;
	return FALSE;
}

gboolean
is_me(server *s, char *nick)
{
	if(!strcmp(s->me->nick, nick))
		return TRUE;
	return FALSE;
}

gboolean
is_user(char *buf)
{
	int x;
	for(x = 0;x < strlen(buf);x++)
		if(buf[x] == '!')
			return TRUE;
	return FALSE;
}

int
split_words(char *line, char *word[])
{
	char *ptr = line;
	int words = 0, space = TRUE, x;

	while(1)
	{
		switch(*ptr)
		{
			case ' ':
				space = TRUE;
				*ptr = '\0'; /* null terminate instead of a space, that way each pointer has one word */ 
				ptr++;
				break;
			case 0:
				for(x = words;x < REGEN_MAX_ARGS;x++)
					word[x] = "\0";
				return words;
				break;
			default:
				if(space)
				{
					/* we had a space before so this a new word */
					space = FALSE;
					word[words++] = ptr;
				}
				ptr++;
		}
	}
}

/* this is pretty much the same function but all we're looking for is a pointer to the start of the nth word */
char *
get_words_from(char *line, int word)
{
	char *ptr = line;
	int words = 0;
	int space = TRUE;
	
	while(1)
	{
		switch(*ptr)
		{
			case ' ':
				space = TRUE;
				ptr++;
				break;
			case 0:
				return NULL;
			default:
				if(space)
				{
					space = FALSE;
					if(words == word)
						return strdup(ptr);
					ptr++;
					words++;
				}
				else
					ptr++;
		}
	}
}

char *
get_msg(char *line)
{
	char *ptr = line;
	char *ret;
	int x;

	for(x = 1;x < strlen(line); x++) /* we start at 1 because usually the first char is : and that's not the 
					    one we're looking for */
	{
		if(line[x] == ':')
			return strdup((char *)&line[++x]);
	}
	return NULL;
}

char *
substr(char *string, int start, int end)
{
	char *_string = strdup(string);
	char *ret = (char *)malloc(sizeof(char) * ((end - start) + 1));
	char *i = _string + end;

	if(start > (strlen(_string) - 1) || end >= strlen(_string) || start > end)
		return NULL;
	
	*i = '\0';
	_string += start;
	
	strcpy(ret, _string);
	return ret;	
}

char *
strip_whitespace(char *buffer)
{
	char *r = strdup(buffer), *s, *x;
	
	while(isspace(*r))
		r++;

	for(s = r;*s != '\0';s++)
		if(!isspace(*s))
			x = s + sizeof(char);
	
	*x = '\0';
	
	return r;
}

char *
get_version()
{
	char *heh = (char *)malloc(sizeof(char) * 512); /* bleh */
	sprintf(heh, "[%s-%s (%s)] : [%s]", APP_NAME, APP_VERSION, APP_VERSION_NAME, APP_WEBSITE);
	return heh;
}

/* this is from tea */
void 
modify_cursor_color(GtkWidget *input,  gchar *color_name)
{
	static const char cursor_color_rc[] = 	"style \"svs-cc\"\n"
						"{\n"
						"GtkEntry::cursor-color=\"#%04x%04x%04x\"\n"
						"}\n"
						"widget \"*.%s\" style : application \"svs-cc\"\n";

	GdkColor color;
	gdk_color_parse(color_name, &color);

	const gchar *name;
	gchar *rc_temp;

	name = gtk_widget_get_name(input);
	
	rc_temp = g_strdup_printf(cursor_color_rc, color.red, color.green, color.blue, name);
	gtk_rc_parse_string(rc_temp);
	gtk_widget_reset_rc_styles(input);

	g_free(rc_temp);
}

char *
regen_get_quit_msg()
{
	return "regen: segfault";
}

