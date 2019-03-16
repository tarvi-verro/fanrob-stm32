#include "usb-c.h"
#include "usb-pck.h"
#include "assert-c.h"

static void setup_send(struct pck_setup *r, void *pck, size_t s)
{
	usb_send(USB_EP_CONTROL, pck, s < r->wLength ? s : r->wLength);
}

static void req_get_descriptor(struct pck_setup *r)
{
	enum str_indx {
		STR_LANGID,
		STR_MANUF,
		STR_PRODUCT,
		STR_SERIAL
	};
	struct pck_device_descriptor desc = {
		.bLength = sizeof(struct pck_device_descriptor),
		.bDescriptorType = PCK_DESCRIPTOR_DEVICE,
		.bcdUSB = 0x00,
		.bDeviceClass = 0xFF,
		.bDeviceSubClass = 0xFF,
		.bDeviceProtocol = 0xFF,
		.bMaxPacketSize = usb_get_max_packet_size(USB_EP_CONTROL),
		.idVendor = 0xDEAF,
		.idProduct = 0xCAFE,
		.bcdDevice = 0xFACE,
		.iManufacturer = STR_MANUF,
		.iProduct = STR_PRODUCT,
		.iSerialNumber = STR_SERIAL,
		.bNumConfigurations = 1,
	};

	struct {
		struct pck_configuration_descriptor cfg;
		struct pck_interface_descriptor face;
	} __attribute__ ((__packed__)) configdesc = {
		.cfg = {
			.bLength = sizeof(struct pck_configuration_descriptor),
			.bDescriptorType = PCK_DESCRIPTOR_CONFIGURATION,
			.wTotalLength = sizeof(configdesc),
			.bNumInterfaces = 1,
			.bConfigurationValue = 1,
			.iConfiguration = 0,
			.bmAttributes = {
				.self_powered = 1,
				.reserved_set_to_1 = 1,
			},
			.bMaxPower = 100 / 2,
		},
		.face = {
			.bLength = sizeof(struct pck_interface_descriptor),
			.bDescriptorType = PCK_DESCRIPTOR_INTERFACE,
			.bInterfaceNumber = 0,
			.bAlternateSetting = 0,
			.bNumEndpoints = 0,
			.bInterfaceClass = 0,
			.bInterfaceSubClass = 0,
			.bInterfaceProtocol = 0,
			.iInterface = 0,
		}
	};
	_Static_assert(sizeof(configdesc) == 18, "configdesc mismatch");

	struct {
		struct pck_string_descriptor d;
		short arr[2];
	} langid = {
		{ sizeof(langid), PCK_DESCRIPTOR_STRING },
		{ 0x09, 0x04 }
	};

	struct {
		struct pck_string_descriptor d;
		uint16_t arr[23];
	} vendor = {
		{ sizeof(vendor), PCK_DESCRIPTOR_STRING },
		u"Omniscient Vendor Group"
	};

	struct {
		struct pck_string_descriptor d;
		uint16_t arr[21];
	} product = {
		{ sizeof(product), PCK_DESCRIPTOR_STRING },
		u"Uneven Hissing Device"
	};

	struct {
		struct pck_string_descriptor d;
		uint16_t arr[14];
	} serial = {
		{ sizeof(serial), PCK_DESCRIPTOR_STRING },
		u"Vague Quotient"
	};
	switch (r->wValue >> 8) {
	case PCK_DESCRIPTOR_DEVICE:
		setup_send(r, &desc, sizeof(desc));
		break;
	case PCK_DESCRIPTOR_CONFIGURATION:
		setup_send(r, &configdesc, sizeof(configdesc));
		break;
	case PCK_DESCRIPTOR_STRING:
		switch (r->wValue & 0xff) {
		case STR_LANGID:
			setup_send(r, &langid, sizeof(langid));
			break;
		case STR_MANUF:
			setup_send(r, &vendor, sizeof(vendor));
			break;
		case STR_PRODUCT:
			setup_send(r, &product, sizeof(product));
			break;
		case STR_SERIAL:
			setup_send(r, &serial, sizeof(serial));
			break;
		}
		break;
	default:
		usb_send(USB_EP_CONTROL, NULL, 0);
	}
}

void handle_setup_requests(struct pck_setup *r)
{
	switch (r->bRequest) {
	case PCK_REQUEST_GET_DESCRIPTOR:
		req_get_descriptor(r);
		break;
	case PCK_REQUEST_SET_ADDRESS:
		usb_set_address(r->wValue);
		usb_respond(USB_EP_CONTROL, USB_RESPONSE_ACK);
		break;
	case PCK_REQUEST_SET_CONFIGURATION:
		usb_respond(USB_EP_CONTROL, USB_RESPONSE_ACK);
		break;
	default:
		usb_respond(USB_EP_CONTROL, USB_RESPONSE_STALL);
	}
}

