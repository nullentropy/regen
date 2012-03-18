void parse_user(char *buf, char *info[]);
gboolean is_channel(char *c);
gboolean is_me(server *s, char *nick);
gboolean is_user(char *buf);
int split_words(char *line, char *word[]);
char *get_words_from(char *line, int word);
char *get_msg(char *line);
char *substr(char *string, int start, int end);
char *strip_whitespace(char *buffer);

char *get_version();

static G_CONST_RETURN gchar * get_widget_name(GtkWidget *w);
void modify_cursor_color (GtkWidget *input,  gchar *color_name);

