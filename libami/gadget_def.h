#ifndef	__LIBAMI_GADGET_H__
#define	__LIBAMI_GADGET_H__

struct gadget_def;

/* Default x11 event check */
typedef int gadget_def_handle_event_cb(struct gadget_def *def, XEvent *event);

/* Specific event callbacks */
typedef void gadget_def_handle_destroy_cb(struct gadget_def *def);
typedef int gadget_def_handle_event_refresh_cb(struct gadget_def *def);
typedef int gadget_def_handle_event_enter_cb(struct gadget_def *def);
typedef int gadget_def_handle_event_leave_cb(struct gadget_def *def);
typedef int gadget_def_handle_event_button_press_cb(struct gadget_def *def,
	    int button);
typedef int gadget_def_handle_event_button_release_cb(struct gadget_def *def,
	    int button);

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
	gadget_def_handle_event_enter_cb *event_enter_cb;
	gadget_def_handle_event_leave_cb *event_leave_cb;
	gadget_def_handle_event_button_press_cb *event_button_press_cb;
	gadget_def_handle_event_button_release_cb *event_button_release_cb;
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
extern	int gadget_def_handle_event_enter(struct gadget_def *def);
extern	int gadget_def_handle_event_leave(struct gadget_def *def);
extern	int gadget_def_handle_event_button_press(struct gadget_def *def,
	    int button);
extern	int gadget_def_handle_event_button_release(struct gadget_def *def,
	    int button);

#endif	/* __LIBAMI_GADGET_H__ */
