#include "app.h"
#include <stddef.h>
#include <stdbool.h>

extern void assert(bool);

static void app_select()
{
	assert(false);
}

static void app_remove()
{
}

static void app_event(enum ev_type typ, enum ev_key key)
{
}

struct app app_heater = {
	.select = app_select,
	.remove = app_remove,
	.event = app_event,
	.previous = NULL,
};

