// Host-side mock of <avr/eeprom.h> used by the regression tests.
// It simulates a 1 KB EEPROM and aborts if the library touches an
// address outside it, so out-of-range accesses are caught as they
// happen instead of silently reading/writing unrelated memory.

#ifndef NVTEST_AVR_EEPROM_H
#define NVTEST_AVR_EEPROM_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef E2END
#define E2END 1023
#endif

namespace nvtest {

inline uint8_t* mem() {
    static uint8_t m[E2END + 1];
    return m;
}

inline long* byte_writes() {
    static long w[E2END + 1];
    return w;
}

inline long& writes() {
    static long n = 0;
    return n;
}

inline void reset() {
    memset(mem(), 0xFF, E2END + 1);
    memset(byte_writes(), 0, sizeof(long) * (E2END + 1));
    writes() = 0;
}

inline void check(size_t off, size_t n) {
    if (off > (size_t)E2END || n > (size_t)E2END + 1 - off) {
        fprintf(stderr, "out-of-range EEPROM access at %lu (size %lu)\n",
                (unsigned long)off, (unsigned long)n);
        exit(99);
    }
}

} // namespace nvtest

inline void eeprom_read_block(void* dst, const void* src, size_t n) {
    const size_t off = (size_t)(uintptr_t)src;
    nvtest::check(off, n);
    memcpy(dst, nvtest::mem() + off, n);
}

inline void eeprom_update_block(const void* src, void* dst, size_t n) {
    const size_t off = (size_t)(uintptr_t)dst;
    nvtest::check(off, n);
    const uint8_t* s = (const uint8_t*)src;
    for (size_t i = 0; i < n; i++) {
        if (nvtest::mem()[off + i] != s[i]) {
            nvtest::mem()[off + i] = s[i];
            nvtest::writes()++;
            nvtest::byte_writes()[off + i]++;
        }
    }
}

inline void eeprom_update_byte(uint8_t* dst, uint8_t value) {
    const size_t off = (size_t)(uintptr_t)dst;
    nvtest::check(off, 1);
    if (nvtest::mem()[off] != value) {
        nvtest::mem()[off] = value;
        nvtest::writes()++;
        nvtest::byte_writes()[off]++;
    }
}

inline uint8_t eeprom_read_byte(const uint8_t* src) {
    const size_t off = (size_t)(uintptr_t)src;
    nvtest::check(off, 1);
    return nvtest::mem()[off];
}

#endif // NVTEST_AVR_EEPROM_H
