#include <stdint.h>
#include <stddef.h>

enum {
	RTC_PM_24H,
	RTC_PM_PM,
};
struct rtc_tr {
	uint32_t su : 4, st : 3, : 1, mnu : 4, mnt : 3, : 1, hu : 4, ht : 2,
		 pm : 1, : 9;
};

struct rtc_dr {
	uint32_t du : 4, dt : 2, : 2, mu : 4, mt : 1, wdu : 3, yu : 4, yt : 4,
		 : 8;
};

enum {
	RTC_FMT_24H,
	RTC_FMT_12H,
};
struct rtc_cr {
	uint32_t wucksel : 3, tsedge : 1, refckon : 1, bypshad : 1, fmt : 1,
		 : 1, alrae : 1, : 1, wute : 1, tse : 1, alraie : 1, : 1,
		 wutie : 1, tsie : 1, add1h : 1, sub1h : 1, bkp : 1, cosel : 1,
		 pol : 1, osel : 2, coe : 1, : 8;
};

struct rtc_isr {
	uint32_t alrawf : 1, : 1, wutwf : 1, shpf : 1, inits : 1, rsf : 1,
		 initf : 1, init : 1, alraf : 1, : 1, wutf : 1, tsf : 1,
		 tsovf : 1, tamp1f : 1, tamp2f : 1, : 1, recalpf : 1, : 15;
};

struct rtc_prer {
	uint32_t prediv_s : 15, : 1, prediv_a : 7, : 9;
};

struct rtc_wutr {
	uint32_t wut : 16, : 16;
};

struct rtc_alrmar {
	uint32_t su : 4, st, : 3, msk1 : 1, mnu : 4, mnt : 3, msk2 : 1, hu : 4,
		 ht : 2, pm : 1, msk3 : 1, du : 4, dt : 2, wdsel : 1, msk4 : 1;
};

struct rtc_wpr {
	uint32_t key : 8, : 24;
};

struct rtc_ssr {
	uint32_t ss : 16, : 16;
};

struct rtc_shiftr {
	uint32_t subfs : 15, : 16, add1s : 1;
};

struct rtc_tstr {
	uint32_t su : 4, st : 3, : 1, mnu : 4, mnt : 3, : 1, hu : 4, ht : 2,
		 pm : 1, : 9;
};

struct rtc_tsdr {
	uint32_t du : 4, dt : 2, : 2, mu : 4, mt : 1, wdu : 3, : 16;
};

struct rtc_tsssr {
	uint32_t ss : 16, : 16;
};

struct rtc_calr {
	uint32_t calm : 9, : 4, calw16 : 1, calw8 : 1, calp : 1, : 16;
};

struct rtc_tafcr {
	uint32_t tamp1e : 1, tamp1trg : 1, tampie : 1, tamp2e : 1,
		 tamp2trg : 1, : 2, tampts : 1, tampfreq : 3, tampflt : 2,
		 tampprch : 2, tamppudis : 1, : 2, pc13value : 1, pc13mode : 1,
		 pc14value : 1, pc14mode : 1, pc15value : 1, pc15mode : 1, : 8;
};

struct rtc_alrmassr {
	uint32_t ss : 15, : 9, maskss : 4, : 4;
};

struct rtc_reg {
	struct rtc_tr tr;
	struct rtc_dr dr;
	struct rtc_cr cr;
	struct rtc_isr isr;
	struct rtc_prer prer;
	struct rtc_wutr wutr;
	struct rtc_alrmar alrmar;
	struct rtc_wpr wpr;
	struct rtc_ssr ssr;
	struct rtc_shiftr shiftr;
	struct rtc_tstr tstr;
	struct rtc_tsdr tsdr;
	struct rtc_tsssr tsssr;
	struct rtc_calr calr;
	struct rtc_tafcr tafcr;
	struct rtc_alrmassr alrmassr;
};

_Static_assert (offsetof(struct rtc_reg, alrmassr) == 0x44,
		"RTC registers don't add up.");
