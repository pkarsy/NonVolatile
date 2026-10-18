# NonVolatile (Non-Volatile Variable)

Persistent bool and numeric variables for Arduino. They can be used like
global variables, but underneath, all changes are saved to the EEPROM.

In the global section:

```C++
#include <NonVolatile.h>
// WARNING: NonVolatile variables must be GLOBAL
NonVolatile<uint16_t> counter;      // Uses EEPROM addresses 0-1
NonVolatile<int> elevation[10];     // Uses addresses 2-21
```

Then, in the code:

```C++
// =, ++v, v++, --v, v--, +=, -=, *=, /= are supported; chaining is not fully tested
counter++; // The change is written to the EEPROM
elevation[5]=100; // Immediately written to the EEPROM
elevation[5]=100; // The changes are NOT written again, to reduce EEPROM wear
Serial.print(elevation[5]++);   // postfix
Serial.print(++elevation[5]);   // prefix
for (int i=0;i<10;i++) elevation[i]=0;
```

### Installation

Put the `NonVolatile` directory in the Arduino `libraries` folder, or put
the `NonVolatile.h` file in the same folder as the `.ino` file.

### Types

- **NonVolatile\<T\>** The most important type this library offers. **T** can be
  `bool`, `char`, `int`, `long`, `float`, or the unsigned counterparts of the
  integer types.
- **NvCounter\<T,N\>** Usually **T** is `uint32_t` (`unsigned long`). This
  special type is useful only as a counter (**=** and **++** are the supported
  operations). It uses more EEPROM bytes but spreads the writes across the N
  extra bytes, doing EEPROM wear leveling. So if every EEPROM cell withstands
  100000 writes, the counter can be incremented about 100000*N times.
  See the NvCounter example.

The next 3 types are not variables. They let you reserve EEPROM space or set
the address used by the variables. Usually they are not needed, and they can
lead to hard-to-examine bugs if you set the EEPROM location wrong.

- **NvAddress _dummy_var(addr)** Sets the EEPROM location of the next
  NonVolatile or NvCounter variable to **addr**. See the NvExample sketch.
- **NvSpace _dummy_var2(no_of_bytes)** Reserves some space in the EEPROM, in
  case you need it for another purpose. The next defined variable will point
  to the next byte after the reserved space. See the NvExample sketch.
- **NvFill _dummy_var3(no_of_bytes)** Like **NvSpace**, but also overwrites the
  reserved bytes with 0xFF, so stale values from an old declaration layout are
  cleared. Useful at the end of the declarations. Note that filling is slow
  (about 3 ms per byte at startup).

### Intended use

NonVolatile has a very specific purpose: to make it easier to store and use the
(usually few) settings an Arduino project needs. It is unsuitable for data
logging. Generally, if you need to use a significant percentage of the EEPROM,
you have to use another method.

### Caveats

Although NonVolatile variables look and feel just like normal variables, there
are some things to keep in mind:

- **You must declare them as global variables.** The EEPROM location is
  determined at runtime. If they are local, they point to an elevated EEPROM
  location every time. Declare all of them one after another, like the example
  above.
- Declarations that do not fit in the EEPROM are detected at startup. The sketch
  hangs instead of wrapping around and overwriting other values (a reset could
  cause EEPROM wear).
- The location in EEPROM is determined at runtime. **If the order of the
  declarations changes, the values of NonVolatile variables can change to
  different and unexpected values.** If you change the order during development,
  you must also reset all NonVolatile variables. See "guard variable" below.
- **You must make sure you do not modify them too often**, or the EEPROM will
  wear out quickly (after about 100000 writes).
- In contrast to normal global variables, they are not set to 0 at boot. This
  is the purpose of their existence.
- In contrast to avr-libc EEMEM variables, they are not updated when code is
  uploaded. If the initial value is important (and it usually is), you can use a
  "guard" variable (see below).
- Reading a NonVolatile is fast, but writing is slow (about 3 ms/byte) due to
  the EEPROM write. This can be important in time-critical code.
- Every NonVolatile variable uses some RAM, in addition to the EEPROM space it
  occupies. `sizeof(var)` returns the RAM usage of the object, not the size of
  the base type.
- Do not change NonVolatile variables by direct EEPROM manipulation.
- If you feel a NonVolatile looks like a normal variable more than it should,
  you can prefix it with something meaningful, like **nv_elevation**.
- Large arrays are RAM hungry. `sizeof(arr)` gives the RAM they use.

### Data integrity caveats

EEPROM writes are not atomic. A reset or power loss in the middle of a
multi-byte write (or in the middle of an **NvCounter** rotation) can leave a
partially written value. The library does not store a checksum or a second copy,
so a torn write is not detected or repaired.

The read-modify-write operators (**++**, **--**, **+=**, ...) are not atomic
with respect to interrupts. If the same variable is modified from an ISR and
from `loop()`, guard the accesses (for example with
`noInterrupts()`/`interrupts()`) or make sure there is only one writer.

EEPROM cells tolerate roughly 100000 writes. The library skips bytes that did
not change, but frequent updates still wear the EEPROM; see **NvCounter** for
counters that are updated often.

### Guard variable

This fragment of code inside `setup()` contains all the NonVolatile variables we
need:

```C++
#define GUARD_SIGN 12345
if (eeprom_guard!=GUARD_SIGN) {
    // NonVolatile variables are set to 0
    counter=0;
    longvar=0;
    for (int i=0;i<5;i++) arr[i]=0;
    float1=0;
    float2=0;
    eeprom_guard=GUARD_SIGN;
}
```

It runs only once. If we change the declarations, most likely the code will run
again (due to `eeprom_guard` misalignment), but to be sure, we can change the
`GUARD_SIGN` to 12340. Note that `eeprom_guard` must itself be a NonVolatile.
See the examples.

### Tests

Host-side regression tests (no Arduino toolchain needed; the EEPROM is mocked)
live in `tests/`:

```sh
cd tests
make test
```

They cover address allocation, the path from RAM to EEPROM for all operators,
`NvCounter` wear leveling across simulated resets, the header include guard,
multi-translation-unit allocation, and the out-of-EEPROM fatal path.
