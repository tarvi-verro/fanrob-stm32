#pragma once
#include <stdint.h>
#include <stddef.h>

struct usb_cntr {
	uint32_t fres : 1, pdwn : 1, lp_mode : 1, fsusp : 1, resume : 1,
		 : 3, esofm : 1, sofm : 1, resetm : 1, suspm : 1, wkupm : 1,
		 errm : 1, pmaovrm : 1, ctrm : 1;
};

struct usb_istr {
	uint32_t ep_id : 4, dir : 1, : 3, esof : 1, sof : 1, reset : 1,
		 susp : 1, wkup : 1, err : 1, pmaovr : 1, ctr : 1;
};

struct usb_fnr {
	uint32_t fn : 11, lsof : 2, lck : 1, rxdm : 1, rxdp : 1;
};

struct usb_daddr {
	uint32_t add : 7, ef : 1, : 8;
};

struct usb_btable {
	/* the three least significant bits are HW-forced to 0 */
	uint32_t btable : 16;
};

enum usb_stat {
	USB_STAT_DISABLED = 0,
	USB_STAT_STALL = 1,
	USB_STAT_NAK = 2,
	USB_STAT_VALID = 3,
};
enum usb_ep_type {
	USB_EP_TYPE_BULK,
	USB_EP_TYPE_CONTROL,
	USB_EP_TYPE_ISO,
	USB_EP_TYPE_INTERR,
};
struct usb_epnr {
	union {
		struct {
			uint32_t ea : 4;
			enum usb_stat stat_tx : 2;
			uint32_t dtog_tx : 1, ctr_tx : 1, ep_kind : 1;
			enum usb_ep_type ep_type : 2;
			uint32_t setup : 1;
			enum usb_stat stat_rx : 2;
			uint32_t dtog_rx : 1, ctr_rx : 1;
		};
		uint32_t u32;
	};
};

struct usb_reg {
	union {
		struct {
			struct usb_epnr ep0r;
			struct usb_epnr ep1r;
			struct usb_epnr ep2r;
			struct usb_epnr ep3r;
			struct usb_epnr ep4r;
			struct usb_epnr ep5r;
			struct usb_epnr ep6r;
			struct usb_epnr ep7r; // 0x1C
		};
		struct usb_epnr epnr[8];
	};
	uint32_t _reserved[8];
	struct usb_cntr cntr; // 0x40
	union { struct usb_istr istr; uint16_t istr_u16; };
	struct usb_fnr fnr;
	struct usb_daddr daddr;
	struct usb_btable btable;
};
_Static_assert(offsetof(struct usb_reg, btable) == 0x50,
	"USB registers misaligned.");

struct usb_addrn_tx {
	/* lowest bit must always be 0 */
	uint32_t addrn_tx : 16, : 16;
};

struct usb_countn_tx {
	uint32_t countn_tx : 10, : 6, : 16;
};

struct usb_addrn_rx {
	/* lowest bit must always be 0 */
	uint32_t addrn_rx : 16, : 16;
};

struct usb_countn_rx {
	uint32_t countn_rx : 10, num_block : 5, blsize : 1, : 16;
};

struct usb_buffer_desc_addr_off {
	struct usb_addrn_tx addrn_tx;
	struct usb_countn_tx countn_tx;
	struct usb_addrn_rx addrn_rx;
	struct usb_countn_rx countn_rx;
};
_Static_assert(sizeof(struct usb_buffer_desc_addr_off) == 16,
	"USB address offset buffer descriptor table misaligned.");

