
extern void camsig_set(int state);
extern void setup_camsig();

#ifdef CFG_CAMSIG
struct camsig_configuration {
	enum pin cam;
};
static const struct camsig_configuration cfg_camsig;
#endif
