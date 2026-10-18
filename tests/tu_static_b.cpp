// Second translation unit for the compile-time multi-TU test.

#include <stdint.h>

#include "avr/eeprom.h"
#include "NonVolatile.h"

NonVolatileAt<uint32_t, 400> static_b;

size_t static_addr_b() {
    return static_b.start();
}

void static_set_b(uint32_t value) {
    static_b = value;
}

uint32_t static_get_b() {
    return static_b;
}
