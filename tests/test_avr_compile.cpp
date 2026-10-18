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

int main() {
    counter = 1;
    counter++;
    counter += 2;
    flag = true;
    flag = !flag;
    values[0] = 3;
    nv_counter = 0;
    nv_counter++;
    return (int)counter + (int)after_fixed;
}
