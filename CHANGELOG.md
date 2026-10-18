# Changelog

## 1.2.0

- Add compile-time EEPROM addresses:
  - `NonVolatileAt<T, ADDR>` and `NvCounterAt<T, N, ADDR>`
  - `NvArrayAt<T, BASE, COUNT>` with an on-demand element proxy
  - `NvFillAt<ADDR, SIZE>` for clearing a static region
- Static declarations store no address in RAM, are checked against `E2END` with
  `static_assert`, and do not use the runtime allocation cursor.
- Document how to mix static and runtime address styles, and the array-proxy
  temporary limitation (`auto& x = array[i]` is not supported).
- Add host regression tests for the static API, including multi-TU declarations,
  boundary addresses and a compile-failure check, and extend the AVR compile test.
- Bump `library.properties` to 1.2.0 and update `keywords.txt` and the examples.

## 1.1.0

- Add an include guard and one shared allocation cursor across translation units.
- Check EEPROM capacity before every access (`NvSpace`, `NvFill`, `NvAddress`,
  `NonVolatile`, `NvCounter`), and make the fatal path an explicit spin loop.
- Read `bool` values as raw bytes, avoiding undefined behavior when the EEPROM
  cell is erased (`0xFF`).
- Return references from assignment, compound and prefix operators; make
  `start()`, `end()` and `selected()` const.
- Add host regression tests with a mocked EEPROM, an AVR compile CI job,
  `library.properties` and `keywords.txt`.
- Document data-integrity caveats (torn writes, ISR races, endurance).
