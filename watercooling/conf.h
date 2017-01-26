#include "regs.h"

#ifdef CONF_L4
#include "conf-l4.h"
#elif defined(CONF_L0)
#include "conf-l0.h"
#else
#error target not configured
#endif
