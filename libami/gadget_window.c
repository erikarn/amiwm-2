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
#include "gadget_list.h"
#include "gadget_window.h"

#ifdef AMIGAOS
#include <pragmas/xlib_pragmas.h>
extern struct Library *XLibBase;
#endif

static void gadget_window_free(struct gadget_def *def);
static int gadget_window_handle_event(struct gadget_def *def, XEvent *event);

/*
 * Create a window gadget.
 *
 * This gets a little hairy because unlike the other gadgets, the graphics
 * context used here is actually created by the window itself.
 *
 * Subsequent gadgets created for inside this window should use the windows
 * GC.
 *
 * So for now this'll be a bit dirty until I get enough pieces working to
 * better understand what to do here.
 */
struct gadget_window *
gadget_window_init(Display *dpy, struct DrawInfo *dri, Window parent_win,
    int width, int height)
{
	struct gadget_window *win;
	GC gc;

	win = calloc(1, sizeof(*win));
	if (win == NULL) {
		return (NULL);
	}

	if (gadget_def_init(GADGET_WINDOW_TO_DEF(win),
	    dpy, parent_win, NULL, dri, 0, 0, width, height, 0) <= 0) {
		free(win);
		return (NULL);
	}

	win->title_label = strdup("default_title");
	win->icon_label = strdup("default_label");

	win->def.w = XCreateSimpleWindow(dpy, parent_win,
	    0, 0,
	    width, height,
	    1, /* depth */
	    win->def.dri->dri_Pens[SHADOWPEN],
	    win->def.dri->dri_Pens[BACKGROUNDPEN]);

	gc = XCreateGC(dpy, win->def.w, 0, NULL);

	XSetBackground(dpy, gc, dri->dri_Pens[BACKGROUNDPEN]);
#ifndef	USE_FONTSETS
	XSetFont(dpy, gc, dri->dri_Font->fid);
#endif

	/* XXX ew, reach into the gadget and store the GC there. */
	win->def.gc = gc;

	win->def.destroy_cb = gadget_window_free;
	win->def.event_cb = gadget_window_handle_event;

	win->glist = gadget_list_create();

	return (win);
}

static void
gadget_window_free(struct gadget_def *def)
{
	struct gadget_window *win = GADGET_DEF_TO_WINDOW(def);

	/* First up, destroy anything under it */
	gadget_list_destroy(win->glist);
	win->glist = NULL;

	/* Now we can destroy the window */
	XDestroyWindow(win->def.dpy, win->def.w);

	/* Now we can destroy the GC */
	XFreeGC(win->def.dpy, win->def.gc);
	win->def.gc = NULL;

	/* Now free our memory */
	if (win->title_label)
		free(win->title_label);
	if (win->icon_label)
		free(win->icon_label);
	free(win);
}

static int
gadget_window_handle_event(struct gadget_def *def, XEvent *event)
{
	struct gadget_window *win = GADGET_DEF_TO_WINDOW(def);

	return (gadget_list_handle_event(win->glist, event));
}

static void
gadget_window_update_wm_properties(struct gadget_window *win)
{
	win->size_hints.flags = PResizeInc;
	win->txtprop1.value = (unsigned char *) win->title_label;
	win->txtprop2.value = (unsigned char *) win->icon_label;
	win->txtprop1.encoding = XA_STRING;
	win->txtprop2.encoding = XA_STRING;
	win->txtprop1.format = 8; /* XXX ? */
	win->txtprop2.format = 8; /* XXX ? */
	win->txtprop1.nitems = strlen((char *) win->txtprop1.value);
	win->txtprop2.nitems = strlen((char *) win->txtprop2.value);

	/* Note: no argc/argv available here */
	XSetWMProperties(win->def.dpy, win->def.w,
	    &win->txtprop1, &win->txtprop2,
	    NULL, 0, &win->size_hints, NULL, NULL);
}

static void
gadget_window_update_window_setup(struct gadget_window *win)
{
	XMapSubwindows(win->def.dpy, win->def.w);
	XMapRaised(win->def.dpy, win->def.w);
}

void
gadget_window_update(struct gadget_window *win)
{
	gadget_window_update_wm_properties(win);
	gadget_window_update_window_setup(win);
}

int
gadget_window_set_title_label(struct gadget_window *win, const char *title)
{
	if (win->title_label != NULL) {
		free(win->title_label);
	}
	win->title_label = strdup(title);
	if (win->title_label == NULL) {
		return (0);
	}
	return (1);
}

int
gadget_window_set_icon_label(struct gadget_window *win, const char *label)
{
	if (win->icon_label != NULL) {
		free(win->icon_label);
	}
	win->icon_label = strdup(label);
	if (win->icon_label == NULL) {
		return (0);
	}
	return (1);
}
