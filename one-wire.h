
extern void setup_onewire();
extern void ow_cmd(char *cmd, int len);

/**
 * ow_measure_temp() - get the temperature from a sensor
 * dev:		device id
 *
 * Return: temperature in millidegrees.
 */
extern int ow_measure_temp(const uint8_t dev[8]);

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
