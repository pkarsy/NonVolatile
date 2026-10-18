// NvCounter value integrity across simulated power-on resets, and
// wear leveling across the sliding window.

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <new>

#include "avr/eeprom.h"
#include "NonVolatile.h"

template <typename C>
static void reboot(C* obj, uint8_t* at) {
    nv_index() = at;
    new (obj) C();
}

int main() {
    typedef NvCounter<uint32_t,5> Counter;

    nvtest::reset();
    uint8_t* base = nv_index();

    alignas(Counter) static unsigned char storage[sizeof(Counter)];
    Counter* counter = reinterpret_cast<Counter*>(storage);
    new (counter) Counter();
    *counter = 0;

    unsigned long failures = 0;
    const unsigned long increments = 200000;

    for (unsigned long i = 0; i < increments; i++) {
        // Rebuild the object from EEPROM, as a reset would.
        if (i != 0 && i % 997 == 0) {
            reboot(counter, base);
        }

        const uint32_t before = *counter;
        (*counter)++;

        if ((uint32_t)*counter != before + 1) {
            if (failures < 5) {
                printf("FAIL at %lu: %lu -> %lu\n",
                       i, (unsigned long)before,
                       (unsigned long)(uint32_t)*counter);
            }
            failures++;
        }
    }

    if ((uint32_t)*counter != increments) {
        printf("FAIL: final value %lu, expected %lu\n",
               (unsigned long)(uint32_t)*counter, increments);
        failures++;
    }
    if (counter->selected() >= 5) {
        printf("FAIL: select %u out of range\n", counter->selected());
        failures++;
    }
    if (counter->end() != counter->start() + sizeof(uint32_t) + 5 - 1) {
        printf("FAIL: wrong end address\n");
        failures++;
    }

    if (failures != 0) {
        printf("test_counter: %lu failure(s)\n", failures);
        return 1;
    }

    printf("test_counter: OK (final=%lu, select=%u)\n",
           (unsigned long)(uint32_t)*counter, counter->selected());
    return 0;
}
