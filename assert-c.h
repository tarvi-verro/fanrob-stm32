
extern void assert(int cnd);

extern void setup_assert();

/* User defined, called before entering endless assert busyloop. */
extern void preassert();

#ifdef CFG_ASSERT
struct assert_configuration {
	enum pin led;
};
static const struct assert_configuration cfg_assert;
#endif

