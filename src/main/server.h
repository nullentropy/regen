server * server_new();
server * server_find(char *name);
void     server_add(server *s);
server * server_get_active();

void server_set_port(server *s, int port);
void server_set_host(server *s, char *host);

int    server_get_port(server *s);
char * server_get_host(server *s);

void server_add_channel(server *s, channel *c);
void server_add_user(server *s, user *u);
channel * server_find_channel_by_name(server *s, char *name);
user * server_find_user_by_nick(server *s, char *nick);

