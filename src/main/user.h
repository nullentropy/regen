user * user_new(char *nick);
channel_user * channel_user_new(user *u);
gboolean user_has_op(user *u);
gboolean user_has_voice(user *u);

char * user_nicklist_format(channel_user *u);


void this_is_pissing_me_off(void);
