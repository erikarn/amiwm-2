#include "libami.h"

static int (*eventfunc[LASTEvent])(XEvent*);

static unsigned long eventmask=0;

/**
 * Dispatch an event.
 *
 * If an event is handled by the event function then consume it,
 * else call cx_send_event() to route it back towards amiwm.
 */
static void eb_dispatch(XEvent *e, unsigned long mask)
{
  if(e->type<0 || e->type>=LASTEvent || (!eventfunc[e->type]) ||
     (!eventfunc[e->type](e)))
    cx_send_event(mask, e);
}

/**
 * Register an event with the amiwm broker.
 *
 * This registers the callback for the given event type, and
 * will add to the current event mask.  cx_broker() will
 * register the broker with libami and thus amiwm, and amiwm
 * will route the events here appropriately.
 */
void cx_event_broker(int type, unsigned long mask, int (*callback)(XEvent*))
{
  if(type>=0 && type<LASTEvent) {
    eventfunc[type]=callback;
    cx_broker(eventmask|=mask, eb_dispatch);
  }
}

