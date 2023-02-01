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

#include "drawinfo.h"

#include "gadget_def.h"

int
gadget_def_init(struct gadget_def *def, Display *dpy, Window parentwin,
    GC gc, struct DrawInfo *dri, int x, int y, int width, int height,
    int depth)
{
	bzero(def, sizeof(*def));

	def->dpy = dpy;
	def->parentwin = parentwin;
	def->gc = gc;
	def->dri = dri;
	def->x = x;
	def->y = y;
	def->width = width;
	def->height = height;
	def->depth = depth;

	return (1);
}

void
gadget_def_destroy(struct gadget_def *def)
{
	/* TODO: If it's still on a list, warn or free? */
	if (def->list != NULL) {
		fprintf(stderr, "%s: freeing gadget that's still on a list?\n",
		    __func__);
	}

	if (def->destroy_cb) {
		def->destroy_cb(def);
	}

	free(def);
}

int
gadget_def_handle_event(struct gadget_def *def, XEvent *event)
{
	if (def->event_cb) {
		return def->event_cb(def, event);
	}
	return -1;
}
