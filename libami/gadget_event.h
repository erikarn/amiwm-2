#ifndef	__LIBAMI_GADGET_EVENT_H__
#define	__LIBAMI_GADGET_EVENT_H__

/*
 * These are events generated by the gadgets themselves.
 *
 * I haven't yet figured out how or where to put the gadget
 * events.  Right now there's a top-level gadget list to
 * contain the list of application windows, and pushing
 * xorg events into gadgets themselves flows from that
 * top level glist into the top level windows, and then
 * any child windows / child gadgets.  Each window has
 * a gadget list of gadgets and it'll send the event to
 * them as well.
 *
 * However, when gadgets / windows generate their OWN
 * events, then what? which gadget list should get those
 * events?  Should events somehow flow up from gadgets to
 * their parent glists, and callbacks trigger to handle
 * those events and can either consume them or route them
 * back up another layer?
 *
 * That way by default the top level glist callback will
 * get all events for all windows, but if a callback
 * is registered for a glist, it'll get all the events and
 * can either claim/consume them in a window specific
 * event loop, or punt them up to the default handler that
 * will route them all the way up.
 *
 * Hm, maybe that'll work.  I'll need to clean up a bunch
 * of code first though.  Oh well, now's a good time to
 * do that!
 *
 * ===
 *
 * So gadget_def has a link to the gadget_list it sits on.
 * Thus gadgets can generate events back up to the owning
 * list.  The owning gadget_list code currently doesn't have
 * a callback / owner pointer for these, but it's trivial
 * to add that so a gadget_window can register for those
 * events, and then the top-level gadget_list (containing
 * the application windows) can register its own callback/
 * callback data.
 */
struct gadget_event;
struct gadget_list;

struct gadget_event {
	/* Placeholders for now */
	uint32_t id;
	uint32_t sub_id;

	/* Gadget list queue entry */
	TAILQ_ENTRY(gadget_event) entry;
	struct gadget_list *parent;
};

extern	struct gadget_event * gadget_event_create(uint32_t id, uint32_t subid);
extern	void gadget_event_free(struct gadget_event *event);

#endif
