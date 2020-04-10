
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <X11/extensions/XI2.h>
#include <X11/extensions/XInput2.h>
#include <stdio.h>
#include <stdint.h>

int main()
{
	Display *dpy = XOpenDisplay(NULL);
	Window rootwin = DefaultRootWindow(dpy);

	/* become immune to server grabs */
	XTestGrabControl(dpy, /* impervious = */ True);

	int xiOpcode, xiEvent, xiError;
	if (!XQueryExtension(dpy, "XInputExtension", &xiOpcode, &xiEvent, &xiError)) {
		fprintf(stderr, "XInputExtension no supported by X-Server -> quit\n");
		return 1;
	}

	/* select XI2 raw events */
	uint32_t xi2_event_mask =
	      XI_RawKeyPressMask
	    | XI_RawKeyReleaseMask
	    | XI_RawButtonPressMask
	    | XI_RawButtonReleaseMask
	    | XI_RawMotionMask
	;
	XIEventMask xi2_event_mask_obj;
	xi2_event_mask_obj.deviceid = XIAllMasterDevices;
	xi2_event_mask_obj.mask = (unsigned char*)(&xi2_event_mask);
	xi2_event_mask_obj.mask_len = sizeof(xi2_event_mask);

	XISelectEvents(
	    dpy,
	    rootwin,
	    &xi2_event_mask_obj,
	    1
	);

	while(1) {
		XEvent ev;
		XNextEvent(dpy, &ev);

		switch(ev.type) {
		    case GenericEvent: {
			XGenericEventCookie *cookie = &ev.xcookie;
			if (XGetEventData(dpy, cookie)) {
			    if (cookie->extension == xiOpcode) {
				switch(cookie->evtype) {
				case XI_RawKeyPress: {
				    XIRawEvent* myEv = (XIRawEvent*)cookie->data;
				    int keycode = myEv->detail;
				    printf("RawKeyPress: keycode = %d\n", keycode);
				} break;
				case XI_RawKeyRelease: {
				    XIRawEvent* myEv = (XIRawEvent*)cookie->data;
				    int keycode = myEv->detail;
				    printf("RawKeyRelease: keycode = %d\n", keycode);
				} break;
				case XI_RawButtonPress: {
				    XIRawEvent* myEv = (XIRawEvent*)cookie->data;
				    int buttonNum = myEv->detail;
				    printf("RawButtonPress: buttonNum = %d\n", buttonNum);
				} break;
				case XI_RawButtonRelease: {
				    XIRawEvent* myEv = (XIRawEvent*)cookie->data;
				    int buttonNum = myEv->detail;
				    printf("RawButtonRelease: buttonNum = %d\n", buttonNum);
				} break;
				case XI_RawMotion: {
				    XIRawEvent* myEv = (XIRawEvent*)cookie->data;
				    printf("RawMotion:\n");
				} break;
				}
				fflush(stdout);
			    }
			}
		    }
		}
	}
}


