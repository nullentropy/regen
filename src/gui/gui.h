void create_main_window();

GtkWidget * create_menubar();
GtkWidget * create_toolbar();
GtkWidget * create_statusbar();

void create_textview(GtkWidget *box);
void create_inputbar(GtkWidget *box);

gboolean cb_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);
void cb_destroy(GtkWidget *widget, gpointer data);
void cb_toolbar_about_clicked();

GtkNotebook * regen_get_notebook();
void regen_update_title(void);
void regen_set_title(char *fmt, ...);
