int server_connect(server *s);
int _server_connect(server *s);
int server_disconnect(server *s);
static void cb_server_input(gpointer data, gint source, GdkInputCondition condition);
static int parse_server_input(server *s);
static int process_server_input(server *s, char *line);
int server_sendf(server *s, char *format, ...);
int server_send(server *s, char *what);
