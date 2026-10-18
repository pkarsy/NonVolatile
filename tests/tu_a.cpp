// One of two translation units used by test_multitu.

#include "avr/eeprom.h"
#include "NonVolatile.h"

NonVolatile<uint16_t> va;

size_t addr_a() {
    return va.start();
}
