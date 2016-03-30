
enum ev_key {
	EV_KEY_UP = 1, /* kbd.c */
	EV_KEY_DOWN = 2,
	EV_KEY_LEFT = 4,
	EV_KEY_RIGHT = 8,
};

enum ev_type {
	EV_TYPE_PRESS,
	EV_TYPE_RELEASE,
};

struct app {
	/* draw your app on lcd */
	void (*select)(); /* null allowed */
	void (*remove)(); /* null allowed */
	void (*event)(enum ev_type, enum ev_key);
	struct app *previous; /* must be null-initialized */
};

/* no recursion allowed */
extern void app_push(struct app *m);
extern void app_pop();

extern void (*app_update)(); /* defined in main.c */

extern struct app app_exposure; /* app-exposure.c */
extern struct app app_info; /* app-info.c */
extern struct app app_conf; /* app-info.c */
extern struct app app_menu; /* app-menu.c */
extern struct app app_heater; /* app-heater.c */
extern struct app app_voltm; /* app-voltm.c */

