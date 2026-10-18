// Overflow scenarios. The library must detect that the declaration
// does not fit and hang *before* touching the EEPROM. The mock exits
// with status 99 on any out-of-range access, so "124" (timeout) means
// the fatal path ran without an access.

#include "avr/eeprom.h"
#include "NonVolatile.h"

#if defined(NV_BOUNDS_SPACE)
// NvSpace itself must reject the oversized reservation.
NvSpace too_big(2000);
NonVolatile<uint16_t> after_space;
#elif defined(NV_BOUNDS_TAIL)
// The final variable would start in range but end past E2END.
NvAddress near_end(1022);
NonVolatile<uint32_t> after_address;
#elif defined(NV_BOUNDS_ADDRESS)
// An address outside the EEPROM must be rejected immediately.
NvAddress invalid_address(2000);
#elif defined(NV_BOUNDS_FILL)
// NvFill must not silently truncate a fill that does not fit.
NvAddress near_end(1022);
NvFill too_big_fill(16);
#else
#error "define one of the NV_BOUNDS_* scenarios"
#endif

int main() {
    return 0;
}
