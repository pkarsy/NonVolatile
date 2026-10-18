// First translation unit for the compile-time multi-TU test.

#include <stdint.h>

#include "avr/eeprom.h"
#include "NonVolatile.h"

NonVolatileAt<uint16_t, 300> static_a;

size_t static_addr_a() {
    return static_a.start();
}

void static_set_a(uint16_t value) {
    static_a = value;
}

uint16_t static_get_a() {
    return static_a;
}
