#ifndef __PARTION__
#define __PARTION__
#include "defs.h"

error_t partion_load();

uint32_t partion_get_active();

uint32_t partion_get_start(uint32_t i);

uint32_t partion_get_totalsector(uint32_t i);

#endif // __PARTION__