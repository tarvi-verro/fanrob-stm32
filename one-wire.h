
extern void setup_onewire();
extern void ow_cmd(char *cmd, int len);

#ifdef CFG_ONEWIRE
struct onewire_configuration {
	enum pin power;
	enum pin tx;
	int tx_af;
	enum line extiln;
	volatile struct usart_reg *usart;
};
static const struct onewire_configuration cfg_ow;
#endif
