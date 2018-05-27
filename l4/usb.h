#pragma once
#include <stdint.h>
#include <stddef.h>

struct usb_cntr {
	uint16_t fres : 1, pdwn : 1, lp_mode : 1, fsusp : 1, resume : 1,
		 l1resume : 1, : 1, l1reqm : 1, esofm : 1, sofm : 1,
		 resetm : 1, suspm : 1, wkupm : 1, errm : 1, pmaovrm : 1,
		 ctrm : 1;
	uint16_t : 16;
};

struct usb_istr {
	uint16_t ep_id : 4, dir : 1, l1req : 1, esof : 1, sof : 1, reset : 1,
		 susp : 1, wkup : 1, err : 1, pmaovr : 1, ctr : 1;
	uint16_t : 16;
};

struct usb_fnr {
	uint16_t fn : 11, lsof : 2, lck : 1, rxdm : 1, rxdp : 1;
	uint16_t : 16;
};

struct usb_daddr {
	uint16_t add : 7, ef : 1, : 8;
	uint16_t : 16;
};

struct usb_btable {
	uint16_t : 3, btable : 13;
	uint16_t : 16;
};

struct usb_lpmcsr {
	uint16_t lpmen : 1, lpmack : 1, : 1, remwake : 1, besl : 4, : 8;
	uint16_t : 16;
};

struct usb_bcdr {
	uint16_t bcden : 1, dcden : 1, pden : 1, sden : 1, dcdet : 1, pdet : 1,
		 sdet : 1, ps2det : 1, : 7, dppu : 1;
	uint16_t : 16;
};

enum {
	USB_STAT_TX_DISABLED,
	USB_STAT_TX_STALL,
	USB_STAT_TX_NAK,
	USB_STAT_TX_VALID,
};
enum {
	USB_STAT_RX_DISABLED,
	USB_STAT_RX_STALL,
	USB_STAT_RX_NAK,
	USB_STAT_RX_VALID,
};
enum {
	USB_EP_TYPE_BULK,
	USB_EP_TYPE_CONTROL,
	USB_EP_TYPE_ISO,
	USB_EP_TYPE_INTERRUPT,
};
struct usb_epnr {
	uint16_t ea : 4, stat_tx : 2, dtog_tx : 1, ctr_tx : 1, ep_kind : 1,
		 eptype : 2, setup : 1, stat_rx : 2, dtog_rx : 1, ctr_rx : 1;
	uint16_t : 16;
};

struct usb_addrn_tx {
	uint16_t : 1, addrn_tx : 15;
};

struct usb_countn_tx {
	uint16_t countn_tx : 10, : 6;
};

struct usb_addrn_rx {
	uint16_t : 1, addrn_rx : 15;
};

enum {
	USB_BL_SIZE_2_BYTES,
	USB_BL_SIZE_32_BYTES,
};
struct usb_countn_rx {
	uint16_t countn_rx : 10, num_block : 5, bl_size : 1;
};

struct usb_reg {
	struct usb_epnr ep0r;		// 0x00
	struct usb_epnr ep1r;		// 0x04
	struct usb_epnr ep2r;		// 0x08
	struct usb_epnr ep3r;		// 0x0C
	struct usb_epnr ep4r;		// 0x10
	struct usb_epnr ep5r;		// 0x14
	struct usb_epnr ep6r;		// 0x18
	struct usb_epnr ep7r;		// 0x1C

	uint32_t _padding0[8];		// 0x20

	struct usb_cntr cntr;		// 0x40
	struct usb_istr istr;		// 0x44
	struct usb_fnr fnr;		// 0x48
	struct usb_daddr daddr;		// 0x4C
	struct usb_btable btable;	// 0x50
	struct usb_lpmcsr lpmcsr;	// 0x54
	struct usb_bcdr	bcdr;		// 0x58
};

_Static_assert (offsetof(struct usb_reg, bcdr) == 0x58,
		"USB registers do not add up.");

struct usb_btable_reg {
	struct {
		struct usb_addrn_tx addrn_tx;
		struct usb_countn_tx countn_tx;
		struct usb_addrn_rx addrn_rx;
		struct usb_countn_rx countn_rx;
	} n[8];
};

