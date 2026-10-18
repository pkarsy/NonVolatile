// Compile-time variables declared in different translation units must
// keep their declared addresses and must not use the runtime cursor.

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "avr/eeprom.h"
#include "NonVolatile.h"

size_t static_addr_a();
void static_set_a(uint16_t value);
uint16_t static_get_a();

size_t static_addr_b();
void static_set_b(uint32_t value);
uint32_t static_get_b();

int main() {
    if (static_addr_a() != 300) {
        printf("FAIL: static_a address %lu, expected 300\n",
               (unsigned long)static_addr_a());
        return 1;
    }
    if (static_addr_b() != 400) {
        printf("FAIL: static_b address %lu, expected 400\n",
               (unsigned long)static_addr_b());
        return 2;
    }

    static_set_a(0x1234);
    static_set_b(0x89ABCDEFUL);

    if (static_get_a() != 0x1234) return 3;
    if (static_get_b() != 0x89ABCDEFUL) return 4;

    if (nvtest::mem()[300] != 0x34 || nvtest::mem()[301] != 0x12) return 5;
    if (nvtest::mem()[400] != 0xEF || nvtest::mem()[403] != 0x89) return 6;

    if ((size_t)(uintptr_t)nv_index() != 0) return 7;

    printf("test_static_multitu: OK (a=%lu, b=%lu)\n",
           (unsigned long)static_addr_a(),
           (unsigned long)static_addr_b());
    return 0;
}
