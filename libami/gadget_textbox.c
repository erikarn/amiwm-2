#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/queue.h>

#include "drawinfo.h"
#include "libami.h"

#ifdef AMIGAOS
#include <pragmas/xlib_pragmas.h>
extern struct Library *XLibBase;
#endif

#include "gadget_def.h"
#include "gadget_textbox.h"

static void gadget_textbox_destroy(struct gadget_def *def);
static int gadget_textbox_handle_event_refresh(struct gadget_def *def);

struct gadget_textbox *
gadget_textbox_create(Display *dpy, struct DrawInfo *dri, GC gc,
    Window mainwin, int x, int y, int width, int height)
{
	struct gadget_textbox *g;

	g = calloc(1, sizeof(*g));
	if (g == NULL) {
		return (NULL);
	}

	if (gadget_def_init(GADGET_TEXTBOX_TO_DEF(g),
	    dpy, mainwin, gc, dri, x, y, width, height, 0) <= 0) {
		free(g);
		return (NULL);
	}

	g->def.w = XCreateSimpleWindow(g->def.dpy, mainwin,
	    x, y,
	    width, height,
	    0, /* depth */
	    g->def.dri->dri_Pens[SHADOWPEN],
	    g->def.dri->dri_Pens[BACKGROUNDPEN]);

	XSelectInput(g->def.dpy, g->def.w, ExposureMask);

	g->def.destroy_cb = gadget_textbox_destroy;
	g->def.event_refresh_cb = gadget_textbox_handle_event_refresh;

	return (g);
}

struct gadget_textbox_line *
gadget_textbox_addline(struct gadget_textbox *g, const char *text)
{
	struct gadget_textbox_line *l;

	l = calloc(1, sizeof(*l));
	if (l == NULL) {
		return (NULL);
	}
	if(g->lastline)
		g->lastline->next = l;
	else
		g->firstline = l;
	g->lastline = l;
	l->text = strdup(text);
	l->l = strlen(text);
#ifdef USE_FONTSETS
	l->w = XmbTextEscapement(g->def.dri->dri_FontSet, l->text, l->l);
#else
	l->w = XTextWidth(g->def.dri->dri_Font, l->text, l->l);
#endif
	l->h = g->def.dri->dri_Ascent + g->def.dri->dri_Descent;

	return (l);
}

static void
gadget_textbox_refresh(struct gadget_textbox *g)
{
  struct gadget_textbox_line *l;

  // This is OBVIOUSLY the wrong value for x here, but let's get it going
  int x = TXT_HSPACE / 2;
  int y = ((g->def.dri->dri_Ascent + g->def.dri->dri_Descent)>>1) + g->def.dri->dri_Ascent;


  /* Draw the bounding box */
  XSetForeground(g->def.dpy, g->def.gc, g->def.dri->dri_Pens[SHADOWPEN]);
  XDrawLine(g->def.dpy, g->def.w, g->def.gc, 0, 0, g->def.width-2, 0);
  XDrawLine(g->def.dpy, g->def.w, g->def.gc, 0, 0, 0, g->def.height-2);

  XSetForeground(g->def.dpy, g->def.gc, g->def.dri->dri_Pens[SHINEPEN]);
  XDrawLine(g->def.dpy, g->def.w, g->def.gc, 0, g->def.height-1, g->def.width-1, g->def.height-1);
  XDrawLine(g->def.dpy, g->def.w, g->def.gc, g->def.width-1, 0, g->def.width-1, g->def.height-1);

  /* Draw text lines */
  XSetForeground(g->def.dpy, g->def.gc, g->def.dri->dri_Pens[TEXTPEN]);
  for(l = g->firstline; l; l=l->next) {
#ifdef USE_FONTSETS
    XmbDrawString(g->def.dpy, g->def.w, g->def.dri->dri_FontSet, g->def.gc,
      x, y, l->text, l->l);
#else
    XDrawString(g->def.dpy, g->def.w, g->def.gc, x, y, l->text, l->l);
#endif
    y+=g->def.dri->dri_Ascent + g->def.dri->dri_Descent;
  }
}

static int
gadget_textbox_handle_event_refresh(struct gadget_def *def)
{
	struct gadget_textbox *g = GADGET_DEF_TO_TEXTBOX(def);

	gadget_textbox_refresh(g);
	return (1);
}

static void
gadget_textbox_destroy(struct gadget_def *def)
{
	struct gadget_textbox *g = GADGET_DEF_TO_TEXTBOX(def);
	struct gadget_textbox_line *l, *n;

	/* Free text list */
	l = g->firstline;
	while (l != NULL) {
		free(l->text);
		n = l->next;
		free(l);
		l = n;
	}

	/* Free window */
	XDestroyWindow(g->def.dpy, g->def.w);

	/* Free struct */
	free(g);
}
