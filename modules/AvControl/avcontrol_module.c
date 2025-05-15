#include <stdio.h>
#include <string.h>
#include <err.h>
#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>

#include <X11/XF86keysym.h>

#include "libami.h"

static void
brightness_up(XEvent *e, void *callback)
{
	//printf("%s: called\n", __func__);
	system("backlight incr 5");
}

static void
brightness_down(XEvent *e, void *callback)
{
	//printf("%s: called\n", __func__);
	system("backlight decr 5");
}

static void
audio_mute(XEvent *e, void *callback)
{
	//printf("%s: called\n", __func__);
	system("mixer vol.mute=toggle >/dev/null");

}

static void
audio_raise_volume(XEvent *e, void *callback)
{
	//printf("%s: called\n", __func__);
	system("mixer vol=+0.05 >/dev/null");
}

static void
audio_lower_volume(XEvent *e, void *callback)
{
	//printf("%s: called\n", __func__);
	system("mixer vol=-0.05 >/dev/null");
}

static char *progname;
int
main(int argc, char *argv[])
{
  char *arg;

  arg = md_init(argc, argv);
  (void) arg;

  /* Setup hotkeys */
  cx_hotkey(XF86XK_MonBrightnessUp, 0, 0, IN_ANYTHING_MASK,
    brightness_up, NULL);
  cx_hotkey(XF86XK_MonBrightnessDown, 0, 0, IN_ANYTHING_MASK,
    brightness_down, NULL);

  cx_hotkey(XF86XK_AudioMute, 0, 0, IN_ANYTHING_MASK,
    audio_mute, NULL);
  cx_hotkey(XF86XK_AudioRaiseVolume, 0, 0, IN_ANYTHING_MASK,
    audio_raise_volume, NULL);
  cx_hotkey(XF86XK_AudioLowerVolume, 0, 0, IN_ANYTHING_MASK,
    audio_lower_volume, NULL);

  progname=argv[0];

  /* Run main loop */
  md_main_loop();

  return 0;
}
