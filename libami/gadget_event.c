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

#include <sys/queue.h>

#include "gadget_def.h"
#include "gadget_list.h"
#include "gadget_event.h"

struct gadget_event *
gadget_event_create(uint32_t id, uint32_t sub_id)
{
	struct gadget_event *ev;

	ev = calloc(1, sizeof(struct gadget_event));
	if (ev == NULL) {
		return (NULL);
	}

	ev->id = id;
	ev->sub_id = sub_id;
	printf("%s: allocating event %p\n", __func__, ev);

	return (ev);
}

void
gadget_event_free(struct gadget_event *event)
{

	if (event->parent != NULL) {
		TAILQ_REMOVE(&event->parent->event_list, event, entry);
		event->parent = NULL;
	}
	printf("%s: freeing event %p\n", __func__, event);
	free(event);
}
