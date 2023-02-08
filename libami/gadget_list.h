#ifndef	__GADGET_LIST_H__

struct gadget_def;
struct gadget_event;

/* X11 events into the list */
typedef int gadget_list_event_cb(struct gadget_list *list, XEvent *event);

/* Gadget generated events into this list */
typedef int gadget_list_gevent_cb(struct gadget_list *list, void *cbdata,
	     struct gadget_event *event);

struct gadget_list {
	/* Yeah, single link list for now */
	TAILQ_HEAD(gadget_def_l, gadget_def) list;

	gadget_list_event_cb *event_cb;

	/* gadget generated events */
	struct {
		gadget_list_gevent_cb *cb;
		void *cbdata;
	 } gevent;
};

/* Create list */
extern	struct gadget_list * gadget_list_create(void);

/* Destroy list and list members */
extern	void gadget_list_destroy(struct gadget_list *list);

extern	void gadget_list_add(struct gadget_list *list, struct gadget_def *g);
extern	void gadget_list_remove(struct gadget_list *list, struct gadget_def *g);

/* Pass an X event into the list, route it to the right widget */
extern	int gadget_list_handle_event(struct gadget_list *list, XEvent *event);

/*
 * Pass a gevent event into the list; route it to a matching gadget or
 * up to the parent to handle/callback.
 */
extern	int gadget_list_handle_gevent(struct gadget_list *list,
	     struct gadget_event *gevent);

#endif	/* __GADGET_LIST_H__ */
