# NonVolatile TODO

Remaining work after version 1.2.0.

## Done in 1.2.0

- [x] Compile-time EEPROM addresses: `NonVolatileAt`, `NvCounterAt`, `NvArrayAt`, `NvFillAt`.
- [x] Compile-time bounds checks and host/AVR regression coverage.
- [x] Mixing/coexistence rules and the array-proxy temporary caveat documented.
- [x] `CHANGELOG.md` and the 1.2.0 version bump.

## P0 - verify on the real toolchain and hardware

- [ ] Confirm the `avr-compile` GitHub Actions job passes (`gcc-avr` + `avr-libc`, ATmega328P).
- [ ] Compile both examples with `arduino-cli` for `arduino:avr:uno`.
- [ ] On-device smoke test: values survive a reset for `NonVolatile`, arrays, `bool`, `float`.
- [ ] On-device test: `NvCounter` survives resets and the select window moves past 256 increments.
- [ ] On-device test: guard variable resets values exactly once.
- [ ] On-device test: an over-sized declaration hangs at startup instead of touching EEPROM.
- [ ] Add an `arduino-cli` CI job that compiles both examples.

## P1 - data integrity

- [ ] Decide the tear-safety policy for multi-byte writes and `NvCounter` rotation
      (currently a power loss can leave a partially written value; documented in the README).
- [ ] If required, implement a journal / double-buffer / CRC scheme. This changes the
      on-EEPROM layout and values, so it needs a migration plan and a guard signature change.
- [ ] Decide how ISR access should be handled. The read-modify-write operators are not
      atomic; either keep documenting "single writer", or provide a small lock/helper.

## P1 - fatal error UX

- [ ] Make the fatal path overridable, e.g. an `NV_FATAL()` macro or hook, so projects can
      log, blink an LED, or reset via the watchdog instead of hanging silently.
- [ ] Mark `nv_fatal()` with `__attribute__((noreturn))`.
- [ ] Decide whether the default should hang (current, avoids EEPROM wear) or reset.
- [ ] Document the startup-hang behavior in the examples too.

## P2 - type safety

- [ ] Add `static_assert(std::is_trivially_copyable<T>::value, ...)` to `NonVolatile`.
- [ ] Add a `static_assert` restricting `NvCounter`'s `T` to integral types.
- [ ] Document support/limitations for `bool`, `float`, enums and plain structs.

## P2 - documentation and examples

- [x] README said "See NvExample" for `NvFill` even though `NvExample.ino` has no
      `NvFill` declaration; the misleading reference was removed from the README.
- [x] Add `CHANGELOG.md` and tag `v1.2.0`.
- [ ] Document that the empty `NvAddress` / `NvSpace` / `NvFill` globals still occupy
      1 byte of RAM each.
- [ ] Update the installation instructions for Arduino Library Manager.
- [ ] Modernize the example Makefiles: they hardcode `/dev/ttyUSB3`, `gtkterm`, and the
      unmaintained `/usr/share/arduino/Arduino.mk`.

## P3 - tests and CI

- [ ] Add a host CI job with `-fsanitize=undefined` (optionally address sanitizer).
- [ ] Add `NvCounter<uint16_t,N>` and `NvCounter<uint8_t,N>` regression tests.
- [ ] Extend `test_multitu` to verify that values persist, not only that addresses differ.
- [ ] Remove the POSIX `timeout` dependency from `tests/Makefile` or document it.
- [ ] Add `clang++` and/or `cppcheck` to CI.

## P3 - cleanup

- [ ] Move the `nv_*` helpers into a `namespace nv` (cleanup for a future major version).
- [ ] Decide whether the empty marker objects can be made zero-size (C++20
      `[[no_unique_address]]`); otherwise keep the 1-byte cost documented.
- [ ] Consider `--`, `-=`, etc. for `NvCounter`; currently `=` and `++` only by design.

## After 1.2.0

- [ ] Automatic declaration-order compile-time allocation without explicit addresses.
      Not implemented: the static API requires an explicit address per declaration.
- [ ] Spike an `EEMEM`/linker-based automatic static layout as an alternative to
      explicit `NonVolatileAt` addresses.
- [ ] An `NvArrayAt` alternative that stores elements so `auto& x = array[i]` works
      (the current proxy is a temporary; documented in the README).
- [ ] Compile-time counter arrays (`NvCounterAt` currently covers single counters).
- [ ] Add `arduino-cli` example compilation to CI.

## Out of scope

- Data logging or frequent multi-byte writes; the library targets a few settings.
- EEPROM endurance beyond the documented ~100000 writes per cell.
- Replacing the guard-variable pattern with automatic initialization.
