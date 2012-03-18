struct config 
{
	char name[32];
	char value[256];
};

char *get_regen_dir();
char *get_regen_dir_plus(char *plus);

int config_load();
static void config_init();
char * config_get(char *name);
