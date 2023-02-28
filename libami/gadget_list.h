#ifndef	__GADGET_LIST_H__

struct gadget_def;
struct gadget_event;

/* X11 events into the list */
typedef int gadget_list_event_cb(struct gadget_list *list, XEvent *event);

/* Gadget generated events into this list */
typedef int gadget_list_gevent_cb(struct gadget_list *list, void *cbdata,
	     struct gadget_event *event);

struct gadget_list {
	/* List of child gadgets */
	TAILQ_HEAD(gadget_def_l, gadget_def) list;

	/* List of pending gadget events from child gadgets */
	TAILQ_HEAD(gadget_event_l, gadget_event) event_list;

	/* optional x11 processing callback, before trying the list of gadgets */
	gadget_list_event_cb *event_cb;

	/* gadget generated event handler */
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
 * queue it up to the parent to handle/callback.
 */
extern	int gadget_list_queue_gevent(struct gadget_list *list,
	     struct gadget_event *gevent);

/*
 * Run the queue for this gadget list (and any child gadget lists?)
 */
extern	int gadget_list_queue_run(struct gadget_list *list);

/*
 * Set the gadget handler callback.
 */
extern	void gadget_list_set_gevent_callback(struct gadget_list *list,
	     gadget_list_gevent_cb *cb, void *cbdata);

#endif	/* __GADGET_LIST_H__ */
