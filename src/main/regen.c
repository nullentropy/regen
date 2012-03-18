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
#include "server.h"
#include "config.h"
#include "../gui/window.h"
#include "../gui/gui.h"

int
main(int argc, char *argv[])
{
	g_thread_init(NULL);
	gdk_threads_init();
	
	gtk_init(&argc, &argv);

	config_load();
	
	create_main_window();

	_debug = FALSE;

	window_printf(window_get_active(), "[%s-%s (%s)] : [%s] : [placeholderforquotes]\n", APP_NAME, APP_VERSION, APP_VERSION_NAME, APP_WEBSITE);

	gtk_main();

	return 0;
}
