GtkWidget * create_switchbar();
GtkToolItem * switchbar_item_new(char *name, int state, int type);

GtkWidget * switchbar_get_icon(int state, int type);

void switchbar_set_active(GtkToolItem *item);
void switchbar_set_highlight(GtkToolItem *item, int state);
void cb_switchbar_clicked(GtkWidget *widget, gpointer data);
void cb_switchbar_close_clicked(GtkWidget *widget, gpointer data);
void switchbar_item_remove(GtkToolItem *item);
