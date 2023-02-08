#ifndef	__GADGET_LIST_H__

struct gadget_def;

typedef int gadget_list_event_cb(struct gadget_list *list, XEvent *event);

struct gadget_list {
	/* Yeah, single link list for now */
	TAILQ_HEAD(gadget_def_l, gadget_def) list;

	gadget_list_event_cb *event_cb;
};

/* Create list */
extern	struct gadget_list * gadget_list_create(void);

/* Destroy list and list members */
extern	void gadget_list_destroy(struct gadget_list *list);

extern	void gadget_list_add(struct gadget_list *list, struct gadget_def *g);
extern	void gadget_list_remove(struct gadget_list *list, struct gadget_def *g);

/* Pass an X event into the list, route it to the right widget */
extern	int gadget_list_handle_event(struct gadget_list *list, XEvent *event);

#endif	/* __GADGET_LIST_H__ */
