#pragma once
#include <stdint.h>

#ifdef CONF_F0
#include "conf-f0.h"
#elif defined(CONF_L4)
#include "conf-l4.h"
#else
#error unsupported platform
#endif

#include "regs.h"
