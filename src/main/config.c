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
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include "regen.h"
#include "config.h"
#include "util.h"

static GList *_config = NULL;

static struct config config_defaults[] = {
	{ "nick",		"regen" },
	{ "nick.alt",		"regen-" },
	{ "realname",		"regen testing" },
	{ "ident",		"regen" },
	{ "background-color",	"#40475b" }
};

static struct config theme_defaults[] = {
};

char *
get_regen_dir()
{
	size_t s;
	const char *home;
	char *path;
	
	home = g_get_home_dir();

	s = sizeof(char) * (strlen(home) + 9);
	path = (char *)malloc(s);
	
	strncpy(path, home, s - (sizeof(char)));
	strncat(path, "/.regen/", s - (sizeof(char) * (strlen(home) - 1)));
	
	return path;
}

char *
get_regen_dir_plus(char *plus)
{
	size_t s;
	char *regendir = get_regen_dir();
	char *path;

	s = sizeof(char) * (strlen(regendir) + strlen(plus) + 1);
	path = (char *)malloc(s);
	
	snprintf(path, s, "%s%s", regendir, plus);

	return path;
}

int
config_load()
{
	config_init();
	
	size_t size = sizeof(char) * 256;
	char *f = get_regen_dir_plus("config");
	char *line = (char *)malloc(size);
	FILE *file = fopen(f, "r");
	char *result = NULL;
	char delims[] = "=";
	char *name, *value, *heh;

	if(file == NULL)
	{
		g_warning("could not load config file");
		return -1;
	}

	while(!feof(file))
	{
		if(getline(&line, &size, file) < 0)
			return -1;
		
		result = (char *)strtok(line, delims);
		if(result == NULL)
			continue;
		heh = strdup(result);
		name = strip_whitespace(heh);
	
		result = (char *)strtok(NULL, delims);
		if(result == NULL)
			continue;
		heh = strdup(result);
		value = strip_whitespace(heh);

		config_set(name, value);
	}
	free(name);
	free(value);
	free(result);
}

static void
config_init()
{
	int x;
	for(x = 0;x < (sizeof(config_defaults) / sizeof(struct config));x++)
		_config = g_list_append(_config, &config_defaults[x]);
}

char *
config_get(char *name)
{
	GList *l = g_list_first(_config);
	while(l != NULL)
	{
		if(!strcasecmp(((struct config *)l->data)->name, name))
			return ((struct config *)l->data)->value;
		l = g_list_next(l);
	}
	return NULL;
}

int
config_set(char *name, char *value)
{
	GList *l = g_list_first(_config);
	struct config *c = (struct config *)malloc(sizeof(struct config));
	while(l != NULL)
	{
		if(!strcasecmp(((struct config *)l->data)->name, name))
		{
			strncpy(((struct config*)l->data)->value, value, sizeof(((struct config *)l->data)->value));
			return 0;
		}
		l = g_list_next(l);
	}
	strncpy(c->name, name, sizeof(c->name));
	strncpy(c->value, value, sizeof(c->value));
	_config = g_list_append(_config, c);
}
