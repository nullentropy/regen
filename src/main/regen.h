#define APP_NAME "regen"
#define APP_VERSION "0.0.6"
#define APP_VERSION_NAME "birth"
#define APP_WEBSITE ""
#define REGEN_MAX_ARGS 32
#define REGEN_MAX_INPUT 64
#define REGEN_WINDOW_TYPE_STATUS 0
#define REGEN_WINDOW_TYPE_CHANNEL 1
#define REGEN_WINDOW_TYPE_QUERY 2
#define REGEN_WINDOW_STATE_NORMAL 0
#define REGEN_WINDOW_STATE_HIGHLIGHTED 1
#define REGEN_MODE_GIVE '+'
#define REGEN_MODE_TAKE '-'
#define REGEN_CONFIG_NICK "nick"
#define REGEN_CONFIG_NICKALT "nick.alt"
#define REGEN_CONFIG_IDENT "ident"
#define REGEN_CONFIG_REALNAME "realname"
#define REGEN_CONFIG_SERVER "server" 


gboolean _debug;

char title[512];
GtkWidget *statusbar;

typedef struct
{
	char		name[255];
	char		topic[512];
	GList		*nick_list;
	char		modes[216];
	
	/* what's the max length for a chan name? hrm... */
	/* what's the best way of dealing with modes? should we have flags or just individual settings? */
	
} channel;

typedef struct
{
	char	nick[52];
	char	ident[128];
	char	host[256];
	char	realname[256];
} user;

typedef struct
{
	user *u;

	gboolean op;
	gboolean voice;
} channel_user;

typedef struct 
{
	unsigned short	port;
	char 		host[255];
	char		input_buffer[2050];
	char		network[255];
	
	int		socket;
	int		tag;

	GList		*channel_list;
	GList		*user_list;
	
	user		*me;

	int		connected:1;
	int		connecting:1;
} server; 

typedef struct
{
	GtkToolItem	*switchbar_item;
	GtkTextView	*textview;
	GtkEntry	*inputbar;
	GtkWidget	*nick_list;	/* only used when the window type is REGEN_WINDOW_TYPE_CHANNEL */
	GtkTreeView	*treeview;	/* " */
	GList		*input_buffer;
	GList		*input_buffer_current;
	server		*_server;
	int		type;
	int		state;
	int		nb_pos;		/* position in the notebook, so we know how to activate/send events to the window */
	char		name[255];
} window;


enum
{
	COLUMN_FIXED,
	NUM_COLUMNS
};
