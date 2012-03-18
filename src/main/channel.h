channel * channel_new(char *name);
void channel_nicklist_add(channel *c, channel_user *u);
gint channel_nicklist_sort_func(channel_user *a, channel_user *b);
void channel_nicklist_sort(channel *c);
char * channel_nicklist_op_to_string(channel *c);
char * channel_nicklist_no_op_to_string(channel *c);
char * channel_nicklist_no_voice_to_string(channel *c);
char * channel_nicklist_voice_to_string(channel *c);
channel_user * channel_find_user_by_nick(channel *c, char *nick);
channel_user * channel_find_user(channel *c, user *u);


