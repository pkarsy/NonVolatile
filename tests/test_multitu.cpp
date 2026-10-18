// Variables declared in different translation units must not overlap.

#include <stddef.h>
#include <stdio.h>

#include "avr/eeprom.h"

size_t addr_a();
size_t addr_b();

int main() {
    const size_t a = addr_a();
    const size_t b = addr_b();
    const size_t a_end = a + sizeof(uint16_t);  // exclusive
    const size_t b_end = b + sizeof(uint32_t);  // exclusive

    if (a > (size_t)E2END || b > (size_t)E2END ||
        a_end > (size_t)E2END + 1 || b_end > (size_t)E2END + 1) {
        printf("FAIL: address outside EEPROM (a=%lu b=%lu)\n",
               (unsigned long)a, (unsigned long)b);
        return 1;
    }

    if (!(a_end <= b || b_end <= a)) {
        printf("FAIL: overlapping variables (a=%lu..%lu, b=%lu..%lu)\n",
               (unsigned long)a, (unsigned long)(a_end - 1),
               (unsigned long)b, (unsigned long)(b_end - 1));
        return 1;
    }

    printf("test_multitu: OK (a=%lu, b=%lu)\n",
           (unsigned long)a, (unsigned long)b);
    return 0;
}
