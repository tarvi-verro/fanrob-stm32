#include <stdint.h>

enum pck_setup_txdir {
	PCK_TXDIR_HOST_TO_DEV,
	PCK_TXDIR_DEV_TO_HOST,
};
enum pck_setup_type {
	PCK_TYPE_STANDARD,
	PCK_TYPE_CLASS,
	PCK_TYPE_VENDOR,
	PCK_TYPE_RESERVED,
};
enum pck_setup_recipient {
	PCK_RECIPIENT_DEVICE,
	PCK_RECIPIENT_INTERFACE,
	PCK_RECIPIENT_ENDPOINT,
	PCK_RECIPIENT_OTHER,
};
enum pck_setup_request {
	PCK_REQUEST_GET_STATUS,
	PCK_REQUEST_CLEAR_FEATURE,
	PCK_REQUEST_SET_FEATURE = 0x03,
	PCK_REQUEST_SET_ADDRESS = 0x05,
	PCK_REQUEST_GET_DESCRIPTOR,
	PCK_REQUEST_SET_DESCRIPTOR,
	PCK_REQUEST_GET_CONFIGURATION,
	PCK_REQUEST_SET_CONFIGURATION,
};
struct pck_setup {
	struct {
		enum pck_setup_recipient recipient : 5;
		enum pck_setup_type type : 2;
		enum pck_setup_txdir tx_dir : 1;
	} bmRequestType;

	enum pck_setup_request bRequest : 8;

	uint16_t wValue : 16;
	uint16_t wIndex : 16;
	uint16_t wLength : 16;
};
_Static_assert(sizeof(struct pck_setup) == 8,
		"Size of the setup packet turned out wrong.");

enum pck_descriptor_type {
	PCK_DESCRIPTOR_DEVICE = 0x01,
	PCK_DESCRIPTOR_CONFIGURATION = 0x02,
	PCK_DESCRIPTOR_STRING = 0x03,
	PCK_DESCRIPTOR_INTERFACE = 0x04,
	PCK_DESCRIPTOR_ENDPOINT = 0x05,
} __attribute__ ((__packed__));
struct pck_device_descriptor {
	uint8_t bLength;
	enum pck_descriptor_type bDescriptorType;
	uint16_t bcdUSB;
	uint8_t bDeviceClass;
	uint8_t bDeviceSubClass;
	uint8_t bDeviceProtocol;
	uint8_t bMaxPacketSize;
	uint16_t idVendor;
	uint16_t idProduct;
	uint16_t bcdDevice;
	uint8_t iManufacturer;
	uint8_t iProduct;
	uint8_t iSerialNumber;
	uint8_t bNumConfigurations;
};
_Static_assert(sizeof(struct pck_device_descriptor) == 18,
		"Device descriptor structure is of the wrong size");

struct pck_configuration_descriptor {
	uint8_t bLength;
	enum pck_descriptor_type bDescriptorType;
	uint16_t wTotalLength;
	uint8_t bNumInterfaces;
	uint8_t bConfigurationValue;
	uint8_t iConfiguration;
	struct {
		uint8_t : 5, remote_wakeup : 1, self_powered : 1,
			reserved_set_to_1 : 1;
	} bmAttributes;
	uint8_t bMaxPower;
} __attribute__ ((__packed__));
_Static_assert(sizeof(struct pck_configuration_descriptor) == 9,
		"configuration descriptor mismatch");

struct pck_interface_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bInterfaceNumber;
	uint8_t bAlternateSetting;
	uint8_t bNumEndpoints;
	uint8_t bInterfaceClass;
	uint8_t bInterfaceSubClass;
	uint8_t bInterfaceProtocol;
	uint8_t iInterface;
};
_Static_assert(sizeof(struct pck_interface_descriptor) == 9,
		"interface descriptor mismatch");

struct pck_endpoint_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	struct {
		uint8_t endpoint_number : 4, : 3, direction : 1;
	} bEndpointAddress;
	struct {
		uint8_t transfer_type : 2, synchronisation_type : 2,
			usage_type : 2, : 2;
	} bmAttributes;
	uint16_t wMaxPacketSize;
	uint8_t bInterval;
};

struct pck_string_langid {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t wLANGID[];
};

struct pck_string_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	short bString[];
};

extern void handle_setup_requests();
