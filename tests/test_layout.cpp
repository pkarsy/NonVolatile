// Address allocation, NvSpace / NvFill / NvAddress, assignment and
// operator persistence.

#include <stdio.h>
#include <string.h>
#include <new>

#include "avr/eeprom.h"
#include "NonVolatile.h"

NonVolatile<uint16_t> first;   // 0..1
NvSpace gap(8);                // -> 10
NvFill fill(4);                // 10..13 set to 0xFF
NonVolatile<uint16_t> second;  // 14..15
NvAddress moved(100);          // jump
NonVolatile<uint16_t> third;   // 100..101

NvAddress counter_at(200);
NvCounter<uint16_t,5> counter; // 200..206 (1 select byte + 2 + 5)
NonVolatile<uint16_t> after;   // 207..208

static int failures = 0;

#define CHECK(cond)                                                     \
    do {                                                                \
        if (!(cond)) {                                                  \
            printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);      \
            failures++;                                                 \
        }                                                               \
    } while (0)

int main() {
    CHECK(first.start() == 0 && first.end() == 1);
    CHECK(second.start() == 14 && second.end() == 15);
    CHECK(third.start() == 100 && third.end() == 101);

    for (int i = 10; i < 14; i++) {
        CHECK(nvtest::mem()[i] == 0xFF);
    }

    second = 0x1234;
    CHECK(nvtest::mem()[14] == 0x34);
    CHECK(nvtest::mem()[15] == 0x12);
    CHECK((uint16_t)second == 0x1234);

    // Assigning the same value must not write to the EEPROM.
    const long writes_before = nvtest::writes();
    second = 0x1234;
    CHECK(nvtest::writes() == writes_before);

    // Prefix / postfix / compound operators.
    CHECK(++second == 0x1235);
    CHECK(second++ == 0x1235);
    CHECK((uint16_t)second == 0x1236);
    second += 10;
    CHECK((uint16_t)second == 0x1240);
    second -= 0x40;
    CHECK((uint16_t)second == 0x1200);
    CHECK((uint16_t)(second *= 2) == 0x2400);
    CHECK((uint16_t)(second /= 2) == 0x1200);
    CHECK(--second == 0x11FF);
    CHECK(second-- == 0x11FF);

    // Chaining now works because the operators return a reference.
    first = 1;
    second = 0;
    first = second = 7;
    CHECK((uint16_t)first == 7);
    CHECK((uint16_t)second == 7);

    // NvCounter footprint: sizeof(T) + N bytes.
    CHECK(counter.start() == 200 && counter.end() == 206);
    CHECK(after.start() == 207 && after.end() == 208);

    // A bool read from an erased cell (0xFF) must be normalized to true
    // instead of leaving an invalid bool object representation.
    nvtest::mem()[300] = 0xFF;
    nv_index() = (uint8_t*)(uintptr_t)300;
    alignas(NonVolatile<bool>) static unsigned char bool_storage[sizeof(NonVolatile<bool>)];
    NonVolatile<bool>* erased_bool = new (bool_storage) NonVolatile<bool>();
    CHECK((int)(bool)*erased_bool == 1);

    if (failures != 0) {
        printf("test_layout: %d failure(s)\n", failures);
        return 1;
    }
    printf("test_layout: OK\n");
    return 0;
}
