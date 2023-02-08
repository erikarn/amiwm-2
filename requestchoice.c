#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "drawinfo.h"
#include "libami.h"

#ifdef AMIGAOS
#include <pragmas/xlib_pragmas.h>
extern struct Library *XLibBase;
#endif

// These are used for button spacing, not the button itself
#define BUT_BUTSPACE (2*(2+5))
#define BUT_INTSPACE 12
#define BUT_EXTSPACE 4

#include "gadget_def.h"
#include "gadget_list.h"
#include "gadget_window.h"
#include "gadget_button.h"
#include "gadget_textbox.h"

struct choice {
	struct choice *next;
	const char *text;
	int l, w;
	struct gadget_button *b;
} *firstchoice=NULL, *lastchoice=NULL;

struct line {
	struct line *next;
	const char *text;
	int l, w, h;
} *firstline=NULL, *lastline=NULL;

Display *dpy;
Window root;

char *progname;

int totw=0, maxw=0, toth=0, nchoices=0;
int depressed=0;
struct choice *selected=NULL;

struct DrawInfo dri;

struct RDArgs *ra=NULL;


static void
selection(int n)
{
	printf("%d\n", n);
	XCloseDisplay(dpy);
	FreeArgs(ra);
	exit(0);
}

static void
*myalloc(size_t s)
{
	void *p=calloc(s,1);
	if(p)
		return p;
	fprintf(stderr, "%s: out of memory!\n", progname);
	FreeArgs(ra);
	exit(1);
}

/*
 * Add a choice to the list of choices, but don't create
 * the button just yet.  This'll be done once all of them
 * are known and the math to create the window sizing/layout
 * can be done.
 */
static void
addchoice(const char *txt)
{
	struct choice *c=myalloc(sizeof(struct choice));
	if(lastchoice)
		lastchoice->next=c;
	else
		firstchoice=c;
	lastchoice=c;

	c->l = strlen(txt);
	c->text = txt;

#ifdef USE_FONTSETS
	totw+=(c->w=XmbTextEscapement(dri.dri_FontSet, c->text, c->l))+BUT_BUTSPACE;
#else
	totw+=(c->w=XTextWidth(dri.dri_Font, c->text, c->l))+BUT_BUTSPACE;
#endif
	nchoices++;
}

/*
 * Add a line to the text box that we can draw.
 */
static void
addline(const char *txt)
{
  struct line *l=myalloc(sizeof(struct line));
  if(lastline)
    lastline->next=l;
  else
    firstline=l;
  lastline=l;
  l->l=strlen(l->text=txt);

  /* These are used to find the size, which we want
   * to use in order to determine how big our window
   * should be.  It's a bit of a chicken/egg problem
   * for now whilst this is figured out - it's also
   * done in gadget_textbox_addline().
   */
#ifdef USE_FONTSETS
  l->w=XmbTextEscapement(dri.dri_FontSet, l->text, l->l);
#else
  l->w=XTextWidth(dri.dri_Font, l->text, l->l);
#endif
  toth+=l->h=dri.dri_Ascent+dri.dri_Descent;
  if(l->w>maxw)
    maxw=l->w;
}

static void
split(char *str, const char *delim, void (*func)(const char *))
{
  char *p;
  if((p=strtok(str, delim)))
    do {
      (*func)(p);
    } while((p=strtok(NULL, delim)));
}

struct choice *getchoice(Window w)
{
  struct choice *c;
  for(c=firstchoice; c; c=c->next)
    if(w == c->b->def.w)
      return c;
  return NULL;
}

void toggle(struct choice *c)
{
  gadget_button_toggle(c->b);
}

void abortchoice()
{
  if(depressed) {
    depressed=0;
    gadget_button_set_depressed(selected->b, 0);
    toggle(selected);
  }
  selected=NULL;
}

void endchoice()
{
  struct choice *c=selected, *c2=firstchoice;
  int n;

  abortchoice();
  if(c==lastchoice)
    selection(0);
  for(n=1; c2; n++, c2=c2->next)
    if(c2==c)
      selection(n);
  selection(0);
}

int main(int argc, char *argv[])
{
  XWindowAttributes attr;
#if 0
  static XSizeHints size_hints;
  static XTextProperty txtprop1, txtprop2;
#endif
  int x, y, extra=0, n=0;
  struct choice *c;
  struct line *l;
  Argtype array[3], *atp;
  struct gadget_list *glist = NULL;
  struct gadget_window *win = NULL;
  struct gadget_textbox *g_textbox = NULL;
  Pixmap stipple;

  progname=argv[0];
  initargs(argc, argv);
  if(!(ra=ReadArgs((STRPTR)"TITLE/A,BODY/A,GADGETS/M", (LONG *)array, NULL))) {
    PrintFault(IoErr(), (UBYTE *)progname);
    exit(1);
  }
  if(!(dpy = XOpenDisplay(NULL))) {
    fprintf(stderr, "%s: cannot connect to X server %s\n", progname,
	    XDisplayName(NULL));
    FreeArgs(ra);
    exit(1);
  }
  root = RootWindow(dpy, DefaultScreen(dpy));
  XGetWindowAttributes(dpy, root, &attr);
  init_dri(&dri, dpy, root, attr.colormap, False);

  glist = gadget_list_create();

  split(array[1].ptr, "\n", addline);
  if((atp=array[2].ptr) != NULL)
    for(; atp->ptr; atp++)
      split(atp->ptr, "|\n", addchoice);

  totw+=BUT_EXTSPACE+BUT_EXTSPACE+BUT_INTSPACE*(nchoices-1);
  toth+=2*(dri.dri_Ascent+dri.dri_Descent)+TXT_TOPSPACE+
    TXT_MIDSPACE+TXT_BOTSPACE+BUT_VSPACE;
  maxw+=TXT_HSPACE+BUT_EXTSPACE+BUT_EXTSPACE;

  if(maxw>totw) {
    extra=maxw-totw;
    totw=maxw;
  }

  /* Create top-level application window */
  win = gadget_window_init(dpy, &dri, root, totw, toth);
  /* XXX errors */

#if 0
  /*
   * XXX TODO: yes, definitely, positively want to make
   * some way to do this as a window background pattern,
   * just need to create a method for it and keep the Pixmap
   * in gadget_window for safe keeping.
   *
   * Note that creating the pixmap requires the destination X11
   * Window and GC, which is peeking under the hood a bit much
   * for what i want to do here.
   */
  stipple = XCreatePixmap(win->def.dpy, win->def.w, 2, 2, attr.depth);
  // Note - this isn't drawing in the stipple; it's drawing in the gc */
  XSetForeground(win->def.dpy, win->def.gc, win->def.dri->dri_Pens[BACKGROUNDPEN]);
  XFillRectangle(win->def.dpy, stipple, win->def.gc, 0, 0, 2, 2);
  // Note - this isn't drawing in the stipple; it's drawing in the gc */
  XSetForeground(win->def.dpy, win->def.gc, win->def.dri->dri_Pens[SHINEPEN]);
  XDrawPoint(win->def.dpy, stipple, win->def.gc, 0, 1);
  XDrawPoint(win->def.dpy, stipple, win->def.gc, 1, 0);

  /* Set the background pixmap for the window itself */
  gadget_window_set_background_pixmap(win, stipple);
  /* stipple is no longer ours */
  stipple = 0;
#endif

  /* Add it to our top level gadget list for event routing */
  gadget_list_add(glist, GADGET_WINDOW_TO_DEF(win));

  /* Create the top-level textbox */
  g_textbox = gadget_textbox_create(dpy, &dri, win->def.gc, win->def.w,
    BUT_EXTSPACE,
    TXT_TOPSPACE,
    totw - BUT_EXTSPACE - BUT_EXTSPACE,
    toth-TXT_TOPSPACE- TXT_MIDSPACE-TXT_BOTSPACE-BUT_VSPACE- (dri.dri_Ascent+dri.dri_Descent));
   /* add it to the window glist */
    gadget_list_add(win->glist, GADGET_TEXTBOX_TO_DEF(g_textbox));

  /* Lay out + create buttons */
  x=BUT_EXTSPACE;
  y=toth-TXT_BOTSPACE-(dri.dri_Ascent+dri.dri_Descent)-BUT_VSPACE;
  for(c=firstchoice; c; c=c->next) {
    c->b = gadget_button_init(dpy, &dri, win->def.gc, win->def.w,
        x + (nchoices == 1 ? (extra >> 1) : n++*extra/(nchoices-1)),
        y,
        c->w + BUT_BUTSPACE,
        // Note: the original code didn't need a +2 here, but
        // when using the ported button gadget it's needed or
        // the bottom of the button isn't shown. Figure out why!
        dri.dri_Ascent+dri.dri_Descent + BUT_VSPACE + 2);
    gadget_button_set_text(c->b, c->text);
    x+=c->w+BUT_BUTSPACE+BUT_INTSPACE;
    /* Add it to the window glist that we're in */
    gadget_list_add(win->glist, GADGET_BUTTON_TO_DEF(c->b));
  }

  /* Lay out + create text box contents */
  for (l = firstline; l; l = l->next) {
    gadget_textbox_addline(g_textbox, l->text);
  }

  /* XXX TODO: add gadget_window set title and icon name calls here */
  gadget_window_set_title_label(win, array[0].ptr);
  gadget_window_set_icon_label(win, "RequestChoice");

#if 0
  XMapSubwindows(dpy, mainwin);
  XMapRaised(dpy, mainwin);
#endif
  /*
   * This does XMapSubWindows, XMapRaised; need to understand those
   * calls better and figure out how to implicitly do it or have
   * a better named function.
   *
   * It also updates the window manager properties (window title,
   * minimised icon title.)
   */
  gadget_window_update(win);

  for(;;) {
    XEvent event;
    XNextEvent(dpy, &event);

    /* Pass event to our top-level glist, which has our main window */
    /* It will pass events to the windows glist and thus gadgets there */
    if (gadget_list_handle_event(glist, &event) == 1) {
	printf("event handled!\n");
        continue;
    }
#if 0
    switch(event.type) {
    case Expose:
      break;
    case LeaveNotify:
      if(depressed && event.xcrossing.window==selected->b->def.w) {
	depressed=0;
	gadget_button_set_depressed(selected->b, 0);
	toggle(selected);
      }
      break;
    case EnterNotify:
      if((!depressed) && selected && event.xcrossing.window==selected->b->def.w) {
	gadget_button_set_depressed(selected->b, 1);
	depressed=1;
	toggle(selected);
      }
      break;
    case ButtonPress:
      if(event.xbutton.button==Button1 &&
	 (c=getchoice(event.xbutton.window))) {
	abortchoice();
	depressed=1;
	gadget_button_set_depressed(c->b, 1);
	toggle(selected=c);
      }
      break;
    case ButtonRelease:
      if(event.xbutton.button==Button1 && selected) {
	if(depressed)
	  endchoice();
	else
	  abortchoice();
      }
      break;
    }
#endif

  }
  gadget_list_destroy(glist);
  FreeArgs(ra);
}
