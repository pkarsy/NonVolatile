/*
 *
 * Copyright 2017 Panagiotis Karagiannis
 * NonVolatile library: persistent (on EEPROM) Arduino variables
 *
 * This code is licenced under the
 * Apache License 2.0
 *
 * This file is part of the NonVolatile library
 * Project site
 * https://github.com/pkarsy/NonVolatile
 *
 */

#ifndef NONVOLATILE_H
#define NONVOLATILE_H

#include <stddef.h>
#include <stdint.h>
#include <avr/eeprom.h>

// The index points to the EEPROM location (an address, not a RAM
// pointer) of the next NonVolatile to be declared.
//
// A function-local static inside an inline function gives us exactly
// one shared cursor for every translation unit that includes this
// header. A plain "static" variable in a header would give each
// translation unit its own cursor, and the variables would overlap.
inline uint8_t*& nv_index() {
    static uint8_t* idx = 0;
    return idx;
}

// Fatal error: the declarations do not fit in the EEPROM.
// Hang instead of resetting, because a reset can cause EEPROM wear.
inline void nv_fatal() {
    for (;;) {
        __asm__ __volatile__("");
    }
}

// Reserve n bytes at the cursor, without touching the EEPROM.
// Returns false when the request does not fit.
// Integer arithmetic is used so the check cannot be bypassed by
// 16-bit pointer wrap-around or out-of-bounds pointer arithmetic.
inline bool nv_reserve(size_t n) {
    const size_t limit = (size_t)E2END + 1;
    const size_t idx = (size_t)(uintptr_t)nv_index();
    if (n > limit || idx > limit - n) {
        return false;
    }
    nv_index() = (uint8_t*)(uintptr_t)(idx + n);
    return true;
}

inline void nv_reserve_or_die(size_t n) {
    if (!nv_reserve(n)) {
        nv_fatal();
    }
}

// Read a value from the EEPROM.
// Reading an erased cell (0xFF) directly into a bool is undefined
// behavior, so bool is read as raw bytes and then converted to a valid
// bool value. The generic version reads the object representation
// directly, as before.
inline void nv_read(bool& value, const uint8_t* from) {
    uint8_t raw[sizeof(bool)];
    eeprom_read_block(raw, from, sizeof(bool));
    value = (raw[0] != 0);
}

template <typename T>
inline void nv_read(T& value, const uint8_t* from) {
    eeprom_read_block(&value, from, sizeof(T));
}

// Address policies ---------------------------------------------------
//
// NonVolatile / NvCounter take an address policy as a template
// parameter. The policy decides where the value lives in the EEPROM:
//
// - NvRuntimeAddress keeps the historical behavior: the address is
//   allocated from the shared runtime cursor, in declaration order.
// - NvStaticAddress<ADDR> makes the address part of the type. It is
//   empty, so it costs no RAM and the address is known at compile time.
//   Static declarations do not advance the runtime cursor; when mixing
//   them with runtime declarations, keep the two regions disjoint
//   (NvAddress can move the runtime cursor past a static region).
// - NvExplicitAddress carries a runtime address. It is used internally
//   by the array proxy, where the address depends on the element index.

struct NvRuntimeAddress {
    uint8_t* address;

    explicit NvRuntimeAddress(uint8_t* a) : address(a) {}

    static NvRuntimeAddress allocate(size_t n) {
        uint8_t* start = nv_index();
        nv_reserve_or_die(n);
        return NvRuntimeAddress(start);
    }

    uint16_t get() const {
        return (uint16_t)(uintptr_t)address;
    }
};

template <uint16_t ADDR>
struct NvStaticAddress {
    static NvStaticAddress allocate(size_t) {
        return NvStaticAddress();
    }

    constexpr uint16_t get() const {
        return ADDR;
    }
};

struct NvExplicitAddress {
    uint16_t address;

    explicit NvExplicitAddress(uint16_t a) : address(a) {}

    // A default-constructed object has no meaningful address. This
    // path is only reachable if the policy is misused directly.
    static NvExplicitAddress allocate(size_t) {
        nv_fatal();
        return NvExplicitAddress(0);
    }

    uint16_t get() const {
        return address;
    }
};

// Compile-time EEPROM fit check. Static addresses are validated with
// static_assert in NonVolatile/NvCounter; runtime addresses are checked
// by nv_reserve_or_die() when they are allocated.
template <typename Policy, size_t Size>
struct NvBounds {
    static constexpr bool ok = true;
};

template <uint16_t ADDR, size_t Size>
struct NvBounds<NvStaticAddress<ADDR>, Size> {
    static constexpr bool ok =
        (size_t)ADDR + Size <= (size_t)E2END + 1;
};

// To keep some space reserved, in the declarations
class NvSpace
{
    public:
    NvSpace(const unsigned int size) {
        nv_reserve_or_die(size);
    }
};

// to set a new EEPROM location for NonVolatile
class NvAddress
{
    public:
    NvAddress(const unsigned int new_addr) {
        if ((size_t)new_addr > (size_t)E2END + 1) {
            nv_fatal();
        }
        nv_index() = (uint8_t*)(uintptr_t)new_addr;
    }
};

// This is like NvSpace but overwrites the contents of the EEPROM
// with 0xff
// usefull at the end of NonVolatile declarations to ensure that
// the nearby bytes of EEPROM are "clean" from old assignments.
class NvFill
{
    public:
    NvFill(const unsigned int size) {
        const size_t start = (size_t)(uintptr_t)nv_index();
        nv_reserve_or_die(size); // Check before writing anything
        for (size_t i = 0; i < (size_t)size; i++) {
            eeprom_update_byte((uint8_t*)(uintptr_t)(start + i),0xff);
        }
    }
};

// Compile-time fill: overwrite SIZE bytes starting at ADDR with 0xFF.
// Like NvFill, but the region is fixed at compile time and does not
// touch the runtime cursor. Useful at the end of the static
// declarations to clear stale data from an old layout.
template <uint16_t ADDR, size_t SIZE>
class NvFillAt {
    static_assert((size_t)ADDR + SIZE <= (size_t)E2END + 1,
                  "NvFillAt: region does not fit in the EEPROM");

    public:
    NvFillAt() {
        for (size_t i = 0; i < SIZE; i++) {
            eeprom_update_byte((uint8_t*)(uintptr_t)((size_t)ADDR + i),0xff);
        }
    }
};

template <typename T, typename Address = NvRuntimeAddress>
class NonVolatile : private Address {
    static_assert(NvBounds<Address, sizeof(T)>::ok,
                  "NonVolatile: address does not fit in the EEPROM");

    private:

    T ram_value;

    uint8_t* address_ptr() const {
        return (uint8_t*)(uintptr_t)Address::get();
    }

    void load_eeprom() {
        nv_read(ram_value, address_ptr());
    }

    void update_eeprom() {
        eeprom_update_block( &ram_value, address_ptr(), sizeof(T) );
    }

    public:

    NonVolatile() : Address(Address::allocate(sizeof(T))) {
        load_eeprom();
    }

    // Explicit-address constructor, used by the array proxy and by
    // advanced users. It does not allocate from the runtime cursor.
    explicit NonVolatile(const Address & address) : Address(address) {
        load_eeprom();
    }

    operator T() const { return ram_value; }

    NonVolatile & operator = (T v) {
        ram_value=v;
        update_eeprom();
        return *this;
    }

    // This one is importand,
    NonVolatile & operator = (const NonVolatile & v) {
        *this = T(v);
        return *this;
    }

    NonVolatile & operator -= (T v) {
        ram_value -= v;
        update_eeprom();
        return *this;
    }

    NonVolatile & operator += (T v) {
        ram_value += v;
        update_eeprom();
        return *this;
    }

    NonVolatile & operator *= (T v) {
        ram_value *= v;
        update_eeprom();
        return *this;
    }

    NonVolatile & operator /= (T v) {
        ram_value /= v;
        update_eeprom();
        return *this;
    }

    NonVolatile & operator ++() { // prefix ++v
        ram_value ++;
        update_eeprom();
        return *this;
    }

    T operator ++(int) {        // postfix v++
        T val = ram_value;      // make a copy of the old value
        ++(*this);              // Now use the prefix version to do the work
        return val;             // return the copy (the old) value.
    }

    NonVolatile & operator --() { // prefix --v
        ram_value --;
        update_eeprom();
        return *this;
    }

    T operator --(int) {        // postfix v--
        T val = ram_value;      // make a copy for result
        --(*this);              // Now use the prefix version to do the work
        return val;             // return the copy (the old) value.
    }

    size_t start() const {
        return (size_t)Address::get();
    }

    size_t end() const {
        return (size_t)Address::get()+sizeof(T)-1;
    }
};

// Compile-time (static) EEPROM address variant of NonVolatile.
template <typename T, uint16_t ADDR>
using NonVolatileAt = NonVolatile<T, NvStaticAddress<ADDR>>;

// Compile-time array. The array itself stores no elements: operator[]
// returns a proxy (a NonVolatile with an explicit address) that reads
// and writes BASE + i*sizeof(T) on demand.
//
// Because the proxy is a temporary, binding an lvalue reference to an
// element (auto& x = array[i]) is not supported.
template <typename T, uint16_t BASE, size_t COUNT>
class NvArrayAt {
    static_assert(COUNT > 0, "NvArrayAt: COUNT must be greater than zero");
    static_assert((size_t)BASE + COUNT * sizeof(T) <= (size_t)E2END + 1,
                  "NvArrayAt: array does not fit in the EEPROM");

    public:

    typedef NonVolatile<T, NvExplicitAddress> Element;

    Element operator[](size_t i) const {
        if (i >= COUNT) {
            nv_fatal();
        }
        return Element(NvExplicitAddress(
            (uint16_t)((size_t)BASE + i * sizeof(T))));
    }

    static constexpr size_t size() {
        return COUNT;
    }
};

// wear leveling counter
// only for uint8_t uint16_t uint32_t
template <typename T, uint8_t N, typename Address = NvRuntimeAddress>
class NvCounter : private Address {
    static_assert(N > 0, "NvCounter: N must be greater than zero");
    static_assert(NvBounds<Address, (size_t)sizeof(T) + (size_t)N>::ok,
                  "NvCounter: address does not fit in the EEPROM");

    private:

    T ram_value;
    uint8_t select;

    uint8_t* address_ptr() const {
        return (uint8_t*)(uintptr_t)Address::get();
    }

    void init() {
        eeprom_read_block(&select, address_ptr(), 1); // Get select value
        select = select%N;
        eeprom_read_block( &ram_value, address_ptr()+1+select, sizeof(T) );
    }

    void update_eeprom() {
        if (ram_value%256==0) {
            select = (select+1)%N;
            eeprom_update_block(&select, address_ptr(), 1); // Set select value
            // For debug only, comment out if you are not testing the library
            // if (select==0 and sizeof(T)>1) for(uint8_t i=1;i<sizeof(T);i++) eeprom_update_byte(address_ptr()+N+i,0xCC);
        }
        eeprom_update_block( &ram_value, address_ptr()+1+select, sizeof(T) );
    }

    public:

    NvCounter() : Address(Address::allocate((size_t)sizeof(T) + (size_t)N)) {
        init();
    }

    // Explicit-address constructor, used by array proxies and by
    // advanced users. It does not allocate from the runtime cursor.
    explicit NvCounter(const Address & address) : Address(address) {
        init();
    }

    operator T() const { return ram_value; }

    NvCounter & operator = (T v) {
        if (v != ram_value) {
            ram_value=v;
            update_eeprom();
        }
        return *this;
    }

    NvCounter & operator = (const NvCounter & v) {
        *this = T(v);
        return *this;
    }

    NvCounter & operator ++() { // prefix ++v
        ram_value ++;
        update_eeprom();
        return *this;
    }

    T operator ++(int) {        // postfix v++
        T val = ram_value;      // make a copy of the old value
        ++(*this);              // Now use the prefix version to do the work
        return val;             // return the copy (the old) value.
    }

    size_t start() const {
        return (size_t)Address::get();
    }

    size_t end() const {
        return (size_t)Address::get()+sizeof(T)+N-1;
    }

    // For debugging purposes. Returns the value of the "select" variable
    uint8_t selected() const {
        return select;
    }
};

// Compile-time (static) EEPROM address variant of NvCounter.
template <typename T, uint8_t N, uint16_t ADDR>
using NvCounterAt = NvCounter<T, N, NvStaticAddress<ADDR>>;

#endif // NONVOLATILE_H
