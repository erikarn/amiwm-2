#ifndef	__LIBAMI__GADGET_BUTTON_H__
#define	__LIBAMI__GADGET_BUTTON_H__

#define BOT_SPACE 4
#define TEXT_SIDE 8
#define BUT_SIDE 12
#define TOP_SPACE 4
#define INT_SPACE 7
#define BUT_VSPACE 2
#define BUT_HSPACE 8

#define	GADGET_DEF_TO_BUTTON(def)	((struct gadget_button *) def)
#define	GADGET_BUTTON_TO_DEF(button)	((struct gadget_def *) button)

struct gadget_button {
	/* Must be first */
	struct gadget_def def;

#if 0
	Display *dpy;
	struct DrawInfo *dri;
	Window w;
	GC gc;
	int x;
	int y;
	int buth;
	int butw;
#endif

	char *txt;
	int depressed;
};

extern	struct gadget_button * gadget_button_init(Display *dpy,
	    struct DrawInfo *dri, GC gc, Window mainwin,
	    int x, int y, int butw, int buth);
extern	void gadget_button_set_text(struct gadget_button *b, const char *txt);

/* Old, pre gadget list / event routing direct methods */
extern	void gadget_button_refresh(struct gadget_button *b);
extern	void gadget_button_set_depressed(struct gadget_button *b,
	    int depressed);
extern	void gadget_button_toggle(struct gadget_button *b);

#endif	/* __LIBAMI__GADGET_BUTTON_H__ */
