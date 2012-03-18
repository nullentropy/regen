window * window_new(server *s, char *name, int state, int type);
void     window_add(window *w);
window * window_find_by_name(server *s, char *name);
window * window_find_by_pos(int pos);
window * window_find_by_switchbar_item(GtkToolItem *item);
window * window_next(void);

window      * window_get_active();
server      * window_get_server(window *w);
GtkTextView * window_get_textview(window *w);
char        * window_get_name(window *w);

void window_set_server(window *w, server *s);
void window_set_type(window *w, int type);
void window_set_name(window *w, char *name);
void window_set_active(window *w);

void window_printf(window *w, char *format, ...);
void window_print(window *w, char *what);
void window_print_error(window *w, char *what);

gboolean window_is_active(window *w);

void window_scroll_to_bottom(window *w);

GtkWidget * textview_new(window *w);
GtkWidget * inputbar_new(window *w);
static GtkWidget * _window_create_nicklist();
void window_nicklist_update(window *w);

static void window_cb_text_entry(GtkWidget *widget, GtkWidget *entry);
gboolean window_cb_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
void window_input_buffer_insert(window *w, char *ptr);

