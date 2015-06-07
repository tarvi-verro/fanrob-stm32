#include "app.h"
#include <stddef.h>
#include <stdbool.h>

extern void assert(bool);

static struct app *first = NULL;
struct app *app_top = NULL;

void app_push(struct app *m)
{
	assert(m->previous == NULL);
	if (!first) {
		first = m;
	} else {
		m->previous = app_top;
		if (app_top->remove)
			app_top->remove();
	}
	app_top = m;
	if (m->select)
		m->select();
}

void app_pop()
{
	assert(app_top != NULL && app_top != first);
	if (app_top->remove)
		app_top->remove();
	struct app *p = app_top->previous;
	app_top->previous = NULL;
	app_top = p;
	p->select();
}

