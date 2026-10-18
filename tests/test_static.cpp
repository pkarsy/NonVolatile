// Compile-time address API: scalars, arrays, layout helpers, operators,
// start()/end(), and RAM savings.

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <new>

#include "avr/eeprom.h"
#include "NonVolatile.h"

NonVolatileAt<uint16_t, 0> a;      // 0..1
NonVolatileAt<uint32_t, 2> b;      // 2..5
NonVolatileAt<bool, 6> flag;       // 6
NvCounterAt<uint32_t, 5, 7> nvc;   // 7..15 (1 select byte + 4 + 5)
NvArrayAt<int, 20, 4> arr;         // 20..20+4*sizeof(int)-1
NvFillAt<50, 4> cleared;           // 50..53 = 0xFF
NvArrayAt<uint8_t, E2END - 9, 10> last_bytes; // 1014..1023

static int failures = 0;

#define CHECK(cond)                                                     \
    do {                                                                \
        if (!(cond)) {                                                  \
            printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);      \
            failures++;                                                 \
        }                                                               \
    } while (0)

int main() {
    // No address member: the static value is just the base type.
    static_assert(sizeof(NonVolatileAt<uint16_t, 0>) == sizeof(uint16_t),
                  "NonVolatileAt must not store an address");
    static_assert(sizeof(NonVolatileAt<uint32_t, 2>) == sizeof(uint32_t),
                  "NonVolatileAt must not store an address");
    static_assert(sizeof(NonVolatileAt<bool, 6>) == sizeof(bool),
                  "NonVolatileAt must not store an address");
    static_assert(sizeof(NonVolatileAt<uint16_t, 0>) <
                  sizeof(NonVolatile<uint16_t>),
                  "static values must be smaller than runtime values");
    static_assert(sizeof(NvCounterAt<uint32_t, 5, 7>) <
                  sizeof(NvCounter<uint32_t, 5>),
                  "static counters must be smaller than runtime counters");
    static_assert(NvArrayAt<int, 20, 4>::size() == 4, "wrong array size");

    // Exact-fit and one-past-the-end boundaries.
    static_assert(NvBounds<NvStaticAddress<E2END>, sizeof(uint8_t)>::ok,
                  "the last byte should fit");
    static_assert(NvBounds<NvStaticAddress<E2END - 1>, sizeof(uint16_t)>::ok,
                  "the last word should fit");
    static_assert(!NvBounds<NvStaticAddress<E2END>, sizeof(uint16_t)>::ok,
                  "a word at the last byte must not fit");
    static_assert(NvBounds<NvStaticAddress<E2END - 6>,
                           (size_t)sizeof(uint16_t) + 5>::ok,
                  "the last counter should fit");
    static_assert(!NvBounds<NvStaticAddress<E2END - 5>,
                            (size_t)sizeof(uint16_t) + 5>::ok,
                  "a counter one byte past the end must not fit");

    CHECK(a.start() == 0 && a.end() == 1);
    CHECK(b.start() == 2 && b.end() == 5);
    CHECK(flag.start() == 6 && flag.end() == 6);
    CHECK(nvc.start() == 7 && nvc.end() == 15);
    CHECK(arr[0].start() == 20);
    CHECK(arr[3].start() == 20 + 3 * sizeof(int));
    CHECK(arr[3].end() == 20 + 4 * sizeof(int) - 1);
    CHECK((size_t)(uintptr_t)nv_index() == 0);

    // Scalar operators and persistence.
    a = 10;
    CHECK((uint16_t)a == 10);
    CHECK(nvtest::mem()[0] == 10);
    CHECK(++a == 11);
    CHECK(a++ == 11);
    CHECK((uint16_t)a == 12);
    a += 4;  CHECK((uint16_t)a == 16);
    a -= 6;  CHECK((uint16_t)a == 10);
    a *= 3;  CHECK((uint16_t)a == 30);
    a /= 5;  CHECK((uint16_t)a == 6);
    CHECK(--a == 5);
    CHECK(a-- == 5);
    CHECK((uint16_t)a == 4);

    b = 0x11223344UL;
    CHECK((uint32_t)b == 0x11223344UL);
    CHECK(nvtest::mem()[2] == 0x44 && nvtest::mem()[5] == 0x11);

    flag = true;
    CHECK((bool)flag == true);
    flag = false;
    CHECK((bool)flag == false);

    // Reading an erased bool cell must be normalized, not UB.
    nvtest::mem()[6] = 0xFF;
    new (&flag) NonVolatileAt<bool, 6>();
    CHECK((int)(bool)flag == 1);

    // Array proxy operators.
    for (int i = 0; i < 4; i++) {
        arr[i] = 0;
    }
    arr[1] = 42;
    CHECK((int)arr[1] == 42);
    CHECK(arr[1].start() == (int)(20 + sizeof(int)));
    arr[1]++;
    CHECK((int)arr[1] == 43);
    arr[1] += 7;
    CHECK((int)arr[1] == 50);
    arr[2] = arr[1];
    CHECK((int)arr[2] == 50);
    int x = arr[1];
    CHECK(x == 50);

    // Static counter, including a simulated reset.
    nvc = 0;
    for (int i = 0; i < 300; i++) {
        ++nvc;
    }
    CHECK((uint32_t)nvc == 300);
    new (&nvc) NvCounterAt<uint32_t, 5, 7>();
    CHECK((uint32_t)nvc == 300);
    ++nvc;
    CHECK((uint32_t)nvc == 301);
    CHECK(nvc.selected() < 5);

    // NvFillAt wrote its region and left the following byte alone.
    for (int i = 50; i < 54; i++) {
        CHECK(nvtest::mem()[i] == 0xFF);
    }
    CHECK(nvtest::mem()[54] != 0xFF);

    // Boundary array: element 9 is the last EEPROM byte.
    CHECK(last_bytes[0].start() == (size_t)E2END - 9);
    CHECK(last_bytes[9].start() == (size_t)E2END);
    CHECK(last_bytes[9].end() == (size_t)E2END);
    last_bytes[9] = 0x5A;
    CHECK((uint8_t)last_bytes[9] == 0x5A);
    CHECK(nvtest::mem()[E2END] == 0x5A);

    CHECK((size_t)(uintptr_t)nv_index() == 0);

    if (failures != 0) {
        printf("test_static: %d failure(s)\n", failures);
        return 1;
    }
    printf("test_static: OK\n");
    return 0;
}
