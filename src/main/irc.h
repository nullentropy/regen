void irc_join_channel(server *s, char *channel, char *key);
void irc_part_channel(server *s, char *channel, char *msg);
void irc_privmsg(server *s, char *to, char *msg);
void irc_notice(server *s, char *to, char *msg);
void irc_whois(server *s, char *who);
void irc_whowas(server *s, char *who, int count);
void irc_nick(server *s, char *nick);
void irc_user(server *s, char *username, char *hostname, char *servername, char *realname);
void irc_quit(server *s, char *msg);
void irc_oper(server *s, char *username, char *password);
void irc_mode(server *s, char *target, char *modes);
void irc_topic(server *s, char *channel, char *topic);
void irc_kick(server *s, char *channel, char *who, char *msg);
void irc_kill(server *s, char *who, char *msg);
void irc_pong(server *s, char *name);
void irc_away(server *s, char *msg);
void irc_rehash(server *s);
void irc_restart(server *s);
void irc_wallops(server *s, char *msg);
void irc_ison(server *s, char *nicks);
void irc_ctcp(server *s, char *target, char *ctcp, char *extra);
void irc_ctcp_reply(server *s, char *target, char *ctcp, char *extra);
