
extern void setup_pump();
void pump_cmd(char *cmd, int len);

#ifdef CFG_PUMP
struct pump_configuration {
	enum pin gaten;
	int gaten_af;
	enum tim_preset_ch fast_ch;
};
static const struct pump_configuration cfg_pump;
#endif

