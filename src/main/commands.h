typedef int (*cmd_callback) (int argc, char *args[], char *line);

struct command
{
	char *cmd;
	char *syntax;
	cmd_callback callback;
};

void parse_command(char *input);
static struct command *find_command(char *cmd);

static int cmd_server_connect(int argc, char *args[], char *line);
static int cmd_server_disconnect(int argc, char *args[], char *line);
static int cmd_exit(int argc, char *args[], char *line);
static int cmd_msg(int argc, char *args[], char *line);
static int cmd_notice(int argc, char *args[], char *line);
static int cmd_join(int argc, char *args[], char *line);
static int cmd_part(int argc, char *args[], char *line);
static int cmd_kick(int argc, char *args[], char *line);
static int cmd_ban(int argc, char *args[], char *line);
static int cmd_whois(int argc, char *args[], char *line);
static int cmd_nick(int argc, char *args[], char *line);
static int cmd_quote(int argc, char *args[], char *line);
static int cmd_clear(int argc, char *args[], char *line);
static int cmd_echo(int argc, char *args[], char *line);
static int cmd_debug(int argc, char *args[], char *line);
static int cmd_oper(int argc, char *args[], char *line);
static int cmd_sv(int argc, char *args[], char *line);
static int cmd_ctcp(int argc, char *args[], char *line);
static int cmd_query(int argc, char *args[], char *line);
static int cmd_op(int argc, char *args[], char *line);
static int cmd_deop(int argc, char *args[], char *line);
static int cmd_voice(int argc, char *args[], char *line);
static int cmd_devoice(int argc, char *args[], char *line);
static int cmd_mode(int argc, char *args[], char *line);
static int cmd_mop(int argc, char *args[], char *line);
static int cmd_mdeop(int argc, char *args[], char *line);
static int cmd_mvoice(int argc, char *args[], char *line);
static int cmd_mdevoice(int argc, char *args[], char *line);
static int cmd_topic(int argc, char *args[], char *line);

int _cmd_privmsg(server *s, char *target, char *message);
int _cmd_chanusermode(server *s, char *channel, char giveortake, char *mode, char *users);
