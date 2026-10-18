// Second translation unit used by test_multitu. It uses a different
// template instantiation than tu_a on purpose: with a per-translation-
// unit cursor both variables would be placed at the same address.

#include "avr/eeprom.h"
#include "NonVolatile.h"

NonVolatile<uint32_t> vb;

size_t addr_b() {
    return vb.start();
}
