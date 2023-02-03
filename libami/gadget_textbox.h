#ifndef	__LIBAMI_GADGET_TEXTBOX_H__
#define	__LIBAMI_GADGET_TEXTBOX_H__

// These are for the text box widget
#define TXT_HSPACE   48
#define TXT_TOPSPACE 4
#define TXT_MIDSPACE 3
#define TXT_BOTSPACE 4

#define	GADGET_DEF_TO_TEXTBOX(def)		((struct gadget_textbox *) def)
#define	GADGET_TEXTBOX_TO_DEF(g)		((struct gadget_def *) g)

struct gadget_textbox_line {
	struct gadget_textbox_line *next;
	char *text;
	int l, w, h;
};

struct gadget_textbox {
	struct gadget_def def;

	struct gadget_textbox_line *firstline, *lastline;
};

extern	struct gadget_textbox *gadget_textbox_create(Display *dpy,
	    struct DrawInfo *dri, GC gc, Window mainwin, int x, int y,
	    int width, int height);

extern	struct gadget_textbox_line * gadget_textbox_addline(
	    struct gadget_textbox *g, const char *text);

#endif	/* __LIBAMI_GADGET_TEXTBOX_H__ */
