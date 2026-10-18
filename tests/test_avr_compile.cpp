// Compile-only smoke test for the real AVR toolchain.
//
// The host tests in this directory mock <avr/eeprom.h>; this file is
// compiled with avr-g++ for an actual AVR target (see the CI
// workflow) to make sure the library builds against avr-libc.

#include <NonVolatile.h>

NonVolatile<uint16_t> counter;
NonVolatile<bool> flag;
NonVolatile<int> values[4];
NvCounter<uint32_t,5> nv_counter;
NvAddress fixed(100);
NonVolatile<uint16_t> after_fixed;

// Compile-time address API.
NonVolatileAt<uint16_t, 200> fixed_counter;
NonVolatileAt<bool, 202> fixed_flag;
NvCounterAt<uint32_t, 5, 203> fixed_nv_counter;
NvArrayAt<uint16_t, 220, 4> fixed_values;
NvFillAt<230, 4> fixed_fill;

static_assert(sizeof(NonVolatileAt<uint16_t, 200>) == sizeof(uint16_t),
              "static NonVolatile must not store an address on AVR");
static_assert(sizeof(NonVolatileAt<bool, 202>) == sizeof(bool),
              "static NonVolatile must not store an address on AVR");
static_assert(sizeof(NvCounterAt<uint32_t, 5, 203>) == sizeof(uint32_t) + 1,
              "static NvCounter must not store an address on AVR");
static_assert(NvArrayAt<uint16_t, 220, 4>::size() == 4,
              "wrong compile-time array size");

int main() {
    counter = 1;
    counter++;
    counter += 2;
    flag = true;
    flag = !flag;
    values[0] = 3;
    nv_counter = 0;
    nv_counter++;

    fixed_counter = 3;
    fixed_counter++;
    fixed_flag = true;
    fixed_nv_counter = 0;
    fixed_nv_counter++;
    fixed_values[0] = 7;
    fixed_values[0] += 2;

    return (int)counter + (int)after_fixed +
           (int)fixed_counter + (int)fixed_values[0];
}
