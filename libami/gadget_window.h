#ifndef	__LIBAMI__GADGET_WINDOW_H__
#define	__LIBAMI__GADGET_WINDOW_H__

#define	GADGET_DEF_TO_WINDOW(def)	((struct gadget_window *) def)
#define	GADGET_WINDOW_TO_DEF(win)	((struct gadget_def *) win)

struct gadget_list;

struct gadget_window {
	/* Must be first */
	struct gadget_def def;

	char *title_label;
	char *icon_label;

	XSizeHints size_hints;
	XTextProperty txtprop1, txtprop2;
	Pixmap bg_pixmap;

	struct gadget_list *glist;
};

extern	struct gadget_window * gadget_window_init(Display *dpy,
	    struct DrawInfo *dri, Window parent_win,
	    int width, int height);

extern	int gadget_window_set_title_label(struct gadget_window *win, const char *txt);
extern	int gadget_window_set_icon_label(struct gadget_window *win, const char *txt);

/* Update the window - for now, setting wm properties, raise window, subwindow map, etc */
extern	void gadget_window_update(struct gadget_window *win);

extern	int gadget_window_set_background_pixmap(struct gadget_window *win,
	     Pixmap p);

#endif	/* __LIBAMI__GADGET_WINDOW_H__ */
