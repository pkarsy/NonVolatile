// This file must NOT compile: the declaration ends one byte past E2END.
// It is used by the Makefile's static-bounds-fail check.

#include <stdint.h>

#include "avr/eeprom.h"
#include "NonVolatile.h"

NonVolatileAt<uint16_t, E2END> out_of_range; // 1023..1024

int main() {
    return 0;
}
