
extern void assert(int cnd);

extern void setup_assert();

#ifdef CFG_ASSERT
struct assert_configuration {
	enum pin led;
};
static const struct assert_configuration cfg_assert;
#endif

