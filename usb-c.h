#include <stddef.h>
#include <stdint.h>

enum usb_endp { USB_EP0, USB_EP1, USB_EP2, USB_EP3, USB_EP4, USB_EP5, USB_EP6,
	USB_EP7 };

extern void setup_usb();
extern void usb_send(enum usb_endp ep, const void *b, size_t len);
extern void usb_set_address(uint8_t a);

enum usb_response {
	USB_RESPONSE_STALL = 1,
	USB_RESPONSE_NAK,
	USB_RESPONSE_ACK,
};
extern void usb_respond(enum usb_endp, enum usb_response);

unsigned usb_get_max_packet_size(enum usb_endp);

