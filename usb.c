#ifndef CONF_F1
#warning "usb.c code has only been tested with STM32F103 blue pill."
#endif

#define CFG_USB
#include "usb.h"
#include "usb-c.h"
#include "conf.h"

#include "rcc.h"
#include "rcc-c.h"
#include "regs.h"
#include "exti.h"
#include "assert-c.h"
#include "usb-pck.h"
#include "gpio-abs.h"


#define cfg_usb_ep_length (sizeof(cfg_usb_ep) / sizeof(cfg_usb_ep[0]))

uint8_t addr;
int addr_received = 0;

int reset_count = 0;
int err_count = 0;

static void usb_reset();

/* Packet memory layout */
struct pm_layout {
	struct usb_buffer_desc_addr_off desc[cfg_usb_ep_length];
	uint16_t io[];
} *pm = (struct pm_layout *) usb_packet_memory;

struct {
	uint16_t rx;
	uint16_t tx;
} pm_io_offset[cfg_usb_ep_length];

static void ep_pm_layout_init()
{
	int o = 0;
	for (int i = 0; i < cfg_usb_ep_length; i++) {
		pm_io_offset[i].rx = o;
		o += cfg_usb_ep[i].rx_size;
		pm_io_offset[i].tx = o;
		o += cfg_usb_ep[i].tx_size;
	}
}


/* Helpers for accessing registers and packet memory. */
static inline struct usb_epnr epnr_invariant(struct usb_epnr e)
{
	e.stat_rx = e.stat_tx = e.dtog_rx = e.dtog_tx = 0;
	e.ctr_rx = e.ctr_tx = 1;
	return e;
}

static inline void dtog_tx_reset(volatile struct usb_epnr *epnr)
{
	struct usb_epnr e = { .u32 = epnr->u32 };
	struct usb_epnr i = epnr_invariant(e);
	i.dtog_tx = e.dtog_tx;
	epnr->u32 = i.u32;
}

static inline void dtog_rx_reset(volatile struct usb_epnr *epnr)
{
	struct usb_epnr e = { .u32 = epnr->u32 };
	struct usb_epnr i = epnr_invariant(e);
	i.dtog_rx = e.dtog_rx;
	epnr->u32 = i.u32;
}

static void ctr_tx_reset(volatile struct usb_epnr *epnr)
{
	struct usb_epnr i = epnr_invariant(*epnr);
	i.ctr_tx = 0;
	epnr->u32 = i.u32;
}

static void ctr_rx_reset(volatile struct usb_epnr *epnr)
{
	struct usb_epnr i = epnr_invariant(*epnr);
	i.ctr_rx = 0;
	epnr->u32 = i.u32;
}

static void stat_tx_set(volatile struct usb_epnr *epnr, enum usb_stat t)
{
	struct usb_epnr e = { .u32 = epnr->u32 };
	struct usb_epnr i = epnr_invariant(e);
	i.stat_tx = e.stat_tx ^ t;
	epnr->u32 = i.u32;
}

static void stat_rx_set(volatile struct usb_epnr *epnr, enum usb_stat t)
{
	struct usb_epnr e = { .u32 = epnr->u32 };
	struct usb_epnr i = epnr_invariant(e);
	i.stat_rx = e.stat_rx ^ t;
	epnr->u32 = i.u32;
}

static void *pm_memcpy(void *dest, const void *src, size_t n)
{
	void *pm = (void *) usb_packet_memory;
	if (dest >= pm && dest <= pm + 2*512) {
		uint32_t *d = (uint32_t *) dest;
		for (int i = 0; i < n/2; i++) {
			uint16_t y = ((uint16_t *) src)[i];
			d[i] = y;
		}
		if (n % 2) { // Copy over any leftover single byte
			uint8_t y = ((uint8_t *) src)[n-1];
			d[n/2] = y;
		}
	} else {
		uint32_t *s = (uint32_t *) src;
		for (int i = 0; i < n/2; i++) {
			uint16_t r = 0xFFFF & s[i];
			((uint16_t *)dest)[i] = r;
		}
		if (n % 2) {
			uint8_t r = 0xFF & s[n/2];
			((uint8_t *)dest)[n-1] = r;
		}
	}
	return dest;
}

/* Exported functions */
unsigned usb_get_max_packet_size(enum usb_endp ep)
{
	assert(ep < cfg_usb_ep_length);

	int a = cfg_usb_ep[ep].rx_size;
	int b = cfg_usb_ep[ep].tx_size;
	return a > b ? a : b;
}

void usb_set_address(uint8_t a)
{
	assert(a <= 127);
	addr = a;
}

void usb_respond(enum usb_endp ep, enum usb_response r)
{
	volatile struct usb_epnr *epnr = usbfs->epnr + ep;
	switch (r) {
	case USB_RESPONSE_ACK:
		pm->desc[ep].countn_tx.countn_tx = 0;
		stat_tx_set(epnr, USB_STAT_VALID);
		break;
	case USB_RESPONSE_NAK:
		stat_tx_set(epnr, USB_STAT_NAK);
		break;
	case USB_RESPONSE_STALL:
		stat_tx_set(epnr, USB_STAT_STALL);
		break;
	}
}

void usb_send(enum usb_endp ep, const void *b, size_t len)
{
	assert(len <= cfg_usb_ep[ep].tx_size);
	if (!len) {
		stat_tx_set(usbfs->epnr + ep, USB_STAT_STALL);
		return;
	}

	pm_memcpy(pm->io + pm_io_offset[ep].tx, b, len);
	pm->desc[ep].countn_tx.countn_tx = len;

	stat_tx_set(usbfs->epnr + ep, USB_STAT_VALID);
}

static void ep_sanity_check()
{
	// Check ep0r initial value.
	const struct usb_epnr ep0r_init = {
		.ea = 0,
		.ep_type = USB_EP_TYPE_CONTROL,
		.stat_tx = USB_STAT_NAK,
		.stat_rx = USB_STAT_VALID,
		.ep_kind = 0,
	};
	assert(cfg_usb_ep[0].epnr_init.u32 == ep0r_init.u32);

	// Check that the endpoint buffer sizes are aligned
	for (int i = 0; i < cfg_usb_ep_length; i++) {
		const struct usb_configuration_ep *e = cfg_usb_ep + i;
		assert((e->rx_size & 0x1) == 0);
		assert((e->tx_size & 0x1) == 0);
	}
}

void setup_usb() {
	ep_pm_layout_init();
	ep_sanity_check();

	nvic_iser[0] |= (1 << 19) | (1 << 20);

	rcc->apb2enr.iopaen = 1;
	rcc->apb1enr.usben = 1;

	usbfs->cntr.pdwn = 0;
	sleep_busy(1000); // wait 1μs

	usbfs->istr_u16 = 0;

	usbfs->cntr.resetm = 1;
	usbfs->cntr.errm = 1;
	usbfs->cntr.fres = 0;
}

/* Interrupt handling */
static void usb_reset() {
	reset_count++;
	usbfs->daddr.ef = 0;
	usbfs->daddr.add = 0;
	addr = 0;
	addr_received = 0;


	usbfs->btable.btable = 0;
	for (int i = 0; i < cfg_usb_ep_length; i++) {
		unsigned o = offsetof(struct pm_layout, io) / 2;
		pm->desc[i].addrn_tx.addrn_tx = pm_io_offset[i].tx + o;
		pm->desc[i].countn_tx.countn_tx = 0;

		pm->desc[i].addrn_rx.addrn_rx = pm_io_offset[i].rx + o;
		pm->desc[i].countn_rx.countn_rx = 0;

		unsigned y = cfg_usb_ep[i].rx_size;
		int u = y >= 32; // 0: 2 byte units, 1: 32 byte
		pm->desc[i].countn_rx.blsize = u;
		pm->desc[i].countn_rx.num_block = y/(2 + u*30); // divide y by 2 or 32

		usbfs->epnr[i].u32 = cfg_usb_ep[i].epnr_init.u32;
	}


	usbfs->cntr.ctrm = 1;
	usbfs->istr_u16 = 0;

	usbfs->daddr.ef = 1;
}

static union {
	struct pck_setup pck_setup;
	uint8_t htd[32];
} buffer;


void i_usblp_canrx0()
{
	if (usbfs->istr.reset) {
		usbfs->istr.reset = 0;
		usb_reset();
	}

	if (usbfs->istr.err) {
		err_count++;
		usbfs->istr.err = 0;
	}

	while (usbfs->istr.ctr) {
		enum usb_endp ep = usbfs->istr.ep_id;
		volatile struct usb_epnr *epnr = usbfs->epnr + ep;

		if (usbfs->istr.dir == USB_DIR_DEVICE_TO_HOST) {
			if (ep == USB_EP_CONTROL)
				assert(epnr->stat_rx == USB_STAT_NAK);

			ctr_tx_reset(epnr);
			if (ep == USB_EP_CONTROL && !addr_received) {
				usbfs->daddr.add = addr;
				addr_received = 1;
			}

			if (ep == USB_EP_CONTROL)
				stat_rx_set(epnr, USB_STAT_VALID);
			continue;
		} else {
			assert(!usbfs->istr.err);

			assert(epnr->stat_rx == USB_STAT_NAK);
			if (ep == USB_EP_CONTROL && epnr->setup) {
				struct pck_setup *p = &buffer.pck_setup;
				pm_memcpy(p, pm->io + pm_io_offset[ep].rx, sizeof(struct pck_setup));

				assert(p->bmRequestType.recipient == PCK_RECIPIENT_DEVICE);
				assert(p->bmRequestType.type == PCK_TYPE_STANDARD);

				ctr_rx_reset(epnr);
				handle_setup_requests(p);
			} else if (ep == USB_EP_CONTROL && epnr->ctr_rx) {
				ctr_rx_reset(epnr);
				stat_rx_set(epnr, USB_STAT_VALID);
			} else if (epnr->ctr_rx) {
				uint8_t *b = buffer.htd;
				int length = pm->desc[ep].countn_rx.countn_rx;
				pm_memcpy(b, pm->io + pm_io_offset[ep].rx, length);

				ctr_rx_reset(epnr);
				stat_rx_set(epnr, USB_STAT_VALID);

				usb_htd_handle(ep, b, length);
			}

		}
	}
}

__attribute__ ((weak)) void usb_htd_handle(enum usb_endp ep, void *dat, int len) {}

