#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#include <errno.h>
#ifdef USE_FONTSETS
#include <locale.h>
#include <wchar.h>
#endif

#include <sys/queue.h>

#include "gadget_def.h"
#include "gadget_list.h"

struct gadget_list *
gadget_list_create(void)
{
	struct gadget_list *l;

	l = calloc(1, sizeof(struct gadget_list));
	if (l == NULL) {
		return (NULL);
	}

	TAILQ_INIT(&l->list);

	return l;
}

void
gadget_list_destroy(struct gadget_list *list)
{
	struct gadget_def *g;

	while (! TAILQ_EMPTY(&list->list)) {
		g = TAILQ_FIRST(&list->list);
		TAILQ_REMOVE(&list->list, g, entry);
		gadget_def_destroy(g);
	}

	free(list);
}

void
gadget_list_add(struct gadget_list *list, struct gadget_def *g)
{
	TAILQ_INSERT_TAIL(&list->list, g, entry);
	g->list = list;
}

void
gadget_list_remove(struct gadget_list *list, struct gadget_def *g)
{

	TAILQ_REMOVE(&list->list, g, entry);
	g->list = NULL;
}

int
gadget_list_handle_event(struct gadget_list *list, XEvent *event)
{
	struct gadget_def *g, *gn;
	int ret;

	if (list->event_cb) {
		ret = list->event_cb(list, event);
		/* Error or handled; exit now */
		if (ret != 0) {
			return (ret);
		}
	}

	/* Iterate over gadgets until we find one that consumes the event */
	TAILQ_FOREACH_SAFE(g, &list->list, entry, gn) {
		switch (event->type) {
		case Expose:
			if (! event->xexpose.count) {
				if (event->xexpose.window == g->w) {
					ret = gadget_def_handle_event_refresh(g);
					if (ret > 0) {
						return (ret);
					}
				}
			}
			break;
		case EnterNotify:
			if (event->xcrossing.window == g->w) {
				ret = gadget_def_handle_event_enter(g);
				if (ret > 0) {
					return (ret);
				}
			}
			break;
		case LeaveNotify:
			if (event->xcrossing.window == g->w) {
				ret = gadget_def_handle_event_leave(g);
				if (ret > 0) {
					return (ret);
				}
			}
			break;
		case ButtonPress:
			if (event->xbutton.window == g->w) {
				ret = gadget_def_handle_event_button_press(g,
				    event->xbutton.button);
				if (ret > 0) {
					return (ret);
				}
			}
			break;
		case ButtonRelease:
			if (event->xbutton.window == g->w) {
				ret =
				    gadget_def_handle_event_button_release(g,
				    event->xbutton.button);
				if (ret > 0) {
					return (ret);
				}
			}
			break;
		}

		/*
		 * Got to here - no specific event handler did anything;
		 * let's instead try the generic X11 handler.
		 */
		ret = gadget_def_handle_event(g, event);
		if (ret > 0) {
			return (ret);
		}
	}

	return 0; /* nothing handled it */
}

