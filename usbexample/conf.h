#pragma once

#ifdef CFG_ASSERT
static const struct assert_configuration cfg_assert = {
	.led = PC13,
};
#endif

#ifdef CFG_USB
static const struct usb_configuration_ep cfg_usb_ep[] = {
	{
		.rx_size = 64,
		.tx_size = 64,
		.epnr_init = {
			.ea = 0,
			.ep_type = USB_EP_TYPE_CONTROL,
			.stat_tx = USB_STAT_NAK,
			.stat_rx = USB_STAT_VALID,
		},
	},
};
#endif
