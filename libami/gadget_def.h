#ifndef	__LIBAMI_GADGET_H__
#define	__LIBAMI_GADGET_H__

struct gadget_def;

typedef int gadget_def_handle_event_cb(struct gadget_def *def, XEvent *event);
typedef void gadget_def_handle_destroy_cb(struct gadget_def *def);
typedef int gadget_def_handle_event_refresh_cb(struct gadget_def *def);

struct gadget_def {
	/* next in gadget list */
	struct gadget_def *next;

	/* parent gadget collection */
	struct gadget_list *list;

	/* our widget window */
	Window w;

	/* Main window / graphics context */
	Display *dpy;
	Window parentwin;
	struct DrawInfo *dri;
	GC gc;

	/* Location in the main window */
	int x, y;
	int width, height;
	int depth;

	gadget_def_handle_destroy_cb *destroy_cb;

	/* event methods */
	gadget_def_handle_event_cb *event_cb;
	gadget_def_handle_event_refresh_cb *event_refresh_cb;
};

/* Internal method for gadgets - init */
extern	int gadget_def_init(struct gadget_def *def, Display *dpy,
	    Window parentwin, GC gc, struct DrawInfo *dri, int x, int y,
	     int width, int height, int depth);

/* Destroy the gadget via the gadget_def handle */
extern	void gadget_def_destroy(struct gadget_def *def);

/* Pass the message to the gadget */
extern	int gadget_def_handle_event(struct gadget_def *def, XEvent *event);

extern	int gadget_def_handle_event_refresh(struct gadget_def *def);

#endif	/* __LIBAMI_GADGET_H__ */
