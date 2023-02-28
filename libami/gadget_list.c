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
#include "gadget_event.h"

struct gadget_list *
gadget_list_create(void)
{
	struct gadget_list *l;

	l = calloc(1, sizeof(struct gadget_list));
	if (l == NULL) {
		return (NULL);
	}

	TAILQ_INIT(&l->list);
	TAILQ_INIT(&l->event_list);

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

	/* XXX TODO: free the events */

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

/**
 * Pass a gevent into the list; check if it's destined for one of our
 * other gadgets, or up to the parent to handle/callback.
 */
int
gadget_list_queue_gevent(struct gadget_list *list, struct gadget_event *gevent)
{
	if (gevent->parent != NULL) {
		fprintf(stderr, "%s: event %p is already on list %p\n",
		    __func__,
		    gevent,
		    gevent->parent);
		return (0);
	}

	/* Add the event to our queue */
	TAILQ_INSERT_TAIL(&list->event_list, gevent, entry);
	gevent->parent = list;

	return (1);
}

/**
 * Run the list of gadget events that are queued.
 *
 * This is done separately from queuing entries so we don't
 * end up with a heavily nested callstack with stuff being
 * potentially deleted during said stack.
 *
 * Return how many events were handled.
 */
int
gadget_list_queue_run(struct gadget_list *list)
{
	struct gadget_event *e, *en;
	struct gadget_def *g, *gn;
	int count = 0, ret;

	/*
	 * Iterate the gadget list and see which gadgets want
	 * to run their own event loop.
	 *
	 * These may generate further events on this list, so do this
	 * first before iterating through our run queue.
	 *
	 * XXX TODO: this'll get spendy as it walks the WHOLE graph
	 * of all gadgets, and honestly we will benefit from having
	 * a second list of gadgets that have their own event
	 * queues.  Optimise that later.
	 */
	TAILQ_FOREACH_SAFE(g, &list->list, entry, gn) {
		ret = gadget_def_run_event_queue(g);
		if (ret >= 0) {
			count = count + ret;
		}
	}

	/*
	 * For each entry, call our callback.
	 * If the callback returns 1 then the event was handled.
	 * If the callback was 0 then it's up to us to free the event.
	 */
	TAILQ_FOREACH_SAFE(e, &list->event_list, entry, en) {
		ret = 0;
		if (list->gevent.cb != NULL) {
			ret = list->gevent.cb(list, list->gevent.cbdata, e);
		}

		/*
		 * If the caller didn't handle anything, we have to?
		 * Pass it to the parent?
		 * For now just free it; figure out whether/how to push
		 * stuff up to the parent list or free things later.
		 */
		if (ret == 0) {
			printf("%s: nothing handled event %p, freeing\n",
			     __func__, e);
			e->parent = NULL;
			TAILQ_REMOVE(&list->event_list, e, entry);
			gadget_event_free(e);
		}

		count++;
	}

	return (count);
}

void
gadget_list_set_gevent_callback(struct gadget_list *list,
     gadget_list_gevent_cb *cb, void *cbdata)
{
	list->gevent.cb = cb;
	list->gevent.cbdata = cbdata;
}
