#ifndef CONF_F1
#warning "usb.c code has only been tested with STM32F103 blue pill."
#endif

#include "conf.h"
#include "usb.h"
#include "rcc.h"
#include "rcc-c.h"
#include "regs.h"
#include "exti.h"
#include "assert-c.h"
#include "usb-pck.h"
#include "gpio-abs.h"
#include "usb-c.h"

uint8_t addr;
int addr_received = 0;

int reset_count = 0;
int err_count = 0;

static void usb_reset();

/* Packet memory layout */
struct usb_buffer_desc_addr_off *pm_buft =
	(struct usb_buffer_desc_addr_off *) usb_packet_memory;


static const int pm_rxbf_offset =  64;
static const int pm_txbf_offset = 128;

uint32_t *const pm_rxbf = usb_packet_memory + pm_rxbf_offset/2;
uint32_t *const pm_txbf = usb_packet_memory + pm_txbf_offset/2;

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
	assert(ep == USB_EP0);
	return pm_txbf_offset - pm_rxbf_offset;
}

void usb_set_address(uint8_t a)
{
	assert(a <= 127);
	addr = a;
}

void usb_respond(enum usb_endp ep, enum usb_response r)
{
	assert(ep == USB_EP0);
	switch (r) {
	case USB_RESPONSE_ACK:
		pm_buft[0].countn_tx.countn_tx = 0;
		stat_tx_set(&usbfs->ep0r, USB_STAT_VALID);
		break;
	case USB_RESPONSE_NAK:
		stat_tx_set(&usbfs->ep0r, USB_STAT_NAK);
		break;
	case USB_RESPONSE_STALL:
		stat_tx_set(&usbfs->ep0r, USB_STAT_STALL);
		break;
	}
}

void usb_send(enum usb_endp ep, const void *b, size_t len)
{
	assert(ep == USB_EP0);
	if (!len) {
		stat_tx_set(&usbfs->ep0r, USB_STAT_STALL);
		return;
	}

	pm_memcpy(pm_txbf, b, len);
	pm_buft->countn_tx.countn_tx = len;

	assert(pm_buft->countn_tx.countn_tx == len);
	assert(usbfs->ep0r.stat_tx == USB_STAT_NAK);
	assert(usbfs->ep0r.stat_rx == USB_STAT_NAK);

	stat_tx_set(&usbfs->ep0r, USB_STAT_VALID);
}

void setup_usb() {
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
	pm_buft[0].addrn_tx.addrn_tx = pm_txbf_offset;
	pm_buft[0].countn_tx.countn_tx = 0;

	pm_buft[0].addrn_rx.addrn_rx = pm_rxbf_offset;
	pm_buft[0].countn_rx.countn_rx = 0;

	unsigned y = usb_get_max_packet_size(USB_EP0);
	int u = y >= 32; // 0: 2 byte units, 1: 32 byte
	pm_buft[0].countn_rx.blsize = u;
	pm_buft[0].countn_rx.num_block = y/(2 + u*30); // divide y by 2 or 32

	struct usb_epnr ep0r = {
		.ea = 0,
		.ep_type = USB_EP_TYPE_CONTROL,
		.stat_tx = USB_STAT_NAK,
		.stat_rx = USB_STAT_VALID,
		.ep_kind = 0,
	};
	usbfs->ep0r.u32 = ep0r.u32;

	usbfs->cntr.ctrm = 1;
	usbfs->istr_u16 = 0;

	usbfs->daddr.ef = 1;
}

struct pck_setup pck_setup_buffer;

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
		assert(usbfs->istr.ep_id == 0);

		if (usbfs->istr.dir == USB_DIR_DEVICE_TO_HOST) {
			assert(usbfs->ep0r.stat_rx == USB_STAT_NAK);

			ctr_tx_reset(&usbfs->ep0r);
			if (!addr_received) {
				usbfs->daddr.add = addr;
				addr_received = 1;
			}
			stat_rx_set(&usbfs->ep0r, USB_STAT_VALID);
			continue;
		} else {
			assert(!usbfs->istr.err);

			assert(usbfs->ep0r.stat_rx == USB_STAT_NAK);
			if (usbfs->ep0r.setup) {
				struct pck_setup *p = &pck_setup_buffer;
				pm_memcpy(p, pm_rxbf, sizeof(struct pck_setup));

				assert(p->bmRequestType.recipient == PCK_RECIPIENT_DEVICE);
				assert(p->bmRequestType.type == PCK_TYPE_STANDARD);

				ctr_rx_reset(&usbfs->ep0r);
				handle_setup_requests(p);
			} else if (usbfs->ep0r.ctr_rx) {
				ctr_rx_reset(&usbfs->ep0r);
				stat_rx_set(&usbfs->ep0r, USB_STAT_VALID);
			}

		}
	}
}

