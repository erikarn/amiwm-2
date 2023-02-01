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
#include "gadget_button.h"

#ifdef AMIGAOS
#include <pragmas/xlib_pragmas.h>
extern struct Library *XLibBase;
#endif

static void gadget_button_free(struct gadget_def *def);
static int gadget_button_handle_event(struct gadget_def *def, XEvent *event);
static int gadget_button_handle_event_refresh(struct gadget_def *def);

/*
 * This is the button code from the executecmd.c tool.
 */

struct gadget_button *
gadget_button_init(Display *dpy, struct DrawInfo *dri, GC gc, Window mainwin,
    int x, int y, int butw, int buth)
{
	struct gadget_button *b;

	b = calloc(1, sizeof(*b));
	if (b == NULL) {
		return (NULL);
	}

	if (gadget_def_init(GADGET_BUTTON_TO_DEF(b),
	    dpy, mainwin, gc, dri, x, y, butw, buth, 0) <= 0) {
		free(b);
		return (NULL);
	}

#if 0
	b->dpy = dpy;
	b->dri = dri;
	b->x = x;
	b->y = y;
	b->butw = butw;
	b->buth = buth;
	b->gc = gc;
#endif

	b->txt = strdup("");

	b->def.w = XCreateSimpleWindow(dpy, mainwin,
	    x, y,
	    butw, /* width */
	    buth, /* height */
	    0, /* depth */
	    b->def.dri->dri_Pens[SHADOWPEN],
	    b->def.dri->dri_Pens[BACKGROUNDPEN]);

	XSelectInput(b->def.dpy, b->def.w, ExposureMask | ButtonPressMask
	    | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask);

	b->def.destroy_cb = gadget_button_free;
	b->def.event_cb = gadget_button_handle_event;
	b->def.event_refresh_cb = gadget_button_handle_event_refresh;

	return (b);
}

void
gadget_button_set_text(struct gadget_button *b, const char *txt)
{
	if (b->txt != NULL)
		free(b->txt);
	b->txt = strdup(txt);
}

void
gadget_button_refresh(struct gadget_button *b)
{
	int fh = b->def.dri->dri_Ascent + b->def.dri->dri_Descent;
	int h = fh + (2 * BUT_VSPACE);
	int l=strlen(b->txt);
#ifdef USE_FONTSETS
	int tw = XmbTextEscapement(b->def.dri->dri_FontSet, b->txt, l);
#else
	int tw = XTextWidth(b->def.dri->dri_Font, b->txt, l);
#endif
	XSetForeground(b->def.dpy, b->def.gc, b->def.dri->dri_Pens[TEXTPEN]);
#ifdef USE_FONTSETS
	XmbDrawString(b->def.dpy, b->def.w, b->def.dri->dri_FontSet, b->def.gc,
	    (b->def.width-tw)>>1, b->def.dri->dri_Ascent+BUT_VSPACE, b->txt, l);
#else
	XDrawString(b->dpy, b->w, b->gc, (b->butw-tw)>>1,
	    b->dri->dri_Ascent+BUT_VSPACE, b->txt, l);
#endif
	XSetForeground(b->def.dpy, b->def.gc,
	    b->def.dri->dri_Pens[b->depressed ? SHADOWPEN:SHINEPEN]);

	XDrawLine(b->def.dpy, b->def.w, b->def.gc, 0, 0, b->def.width - 2, 0);
	XDrawLine(b->def.dpy, b->def.w, b->def.gc, 0, 0, 0, h-2);
	XSetForeground(b->def.dpy, b->def.gc,
	    b->def.dri->dri_Pens[b->depressed ? SHINEPEN:SHADOWPEN]);

	XDrawLine(b->def.dpy, b->def.w, b->def.gc, 1, h-1, b->def.width-1, h-1);
	XDrawLine(b->def.dpy, b->def.w, b->def.gc, b->def.width -1, 1, b->def.width -1, h-1);
	XSetForeground(b->def.dpy, b->def.gc, b->def.dri->dri_Pens[BACKGROUNDPEN]);
	XDrawPoint(b->def.dpy, b->def.w, b->def.gc, b->def.width - 1, 0);
	XDrawPoint(b->def.dpy, b->def.w, b->def.gc, 0, h-1);
}

void
gadget_button_set_depressed(struct gadget_button *b, int depressed)
{
	b->depressed = depressed;
}

void
gadget_button_toggle(struct gadget_button *b)
{
	int pen;

	pen = (b->depressed) ? FILLPEN : BACKGROUNDPEN;


	XSetWindowBackground(b->def.dpy, b->def.w, b->def.dri->dri_Pens[pen]);
	XClearWindow(b->def.dpy, b->def.w);
	gadget_button_refresh(b);
}

static void
gadget_button_free(struct gadget_def *def)
{
	struct gadget_button *b = GADGET_DEF_TO_BUTTON(def);

	XDestroyWindow(b->def.dpy, b->def.w);
	free(b->txt);
	free(b);
}

static int
gadget_button_handle_event(struct gadget_def *def, XEvent *event)
{
	return (0);
}

static int
gadget_button_handle_event_refresh(struct gadget_def *def)
{
	struct gadget_button *b = GADGET_DEF_TO_BUTTON(def);

	gadget_button_refresh(b);
	return (1);
}
