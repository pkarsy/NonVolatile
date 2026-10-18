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

template <typename T>
class NonVolatile {
    private:

    T ram_value;
    uint8_t* address;

    void update_eeprom() {
        eeprom_update_block( &ram_value, address, sizeof(T) );
    }

    public:

    NonVolatile() {
        const size_t start = (size_t)(uintptr_t)nv_index();
        nv_reserve_or_die(sizeof(T)); // Check before reading the EEPROM
        address = (uint8_t*)(uintptr_t)start;
        nv_read(ram_value, address);
    }

    operator T() const { return ram_value; }

    NonVolatile & operator = (T v) {
        ram_value=v;
        update_eeprom();
        return *this;
    }

    // This one is importand,
    NonVolatile & operator = (const NonVolatile<T> & v) {
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
        return size_t(address);
    }

    size_t end() const {
        return size_t(address)+sizeof(T)-1;
    }
};

// wear leveling counter
// only for uint8_t uint16_t uint32_t
template <typename T, uint8_t N>
class NvCounter {
    static_assert(N > 0, "NvCounter: N must be greater than zero");

    private:

    T ram_value;
    uint8_t* address;
    uint8_t select;

    void update_eeprom() {
        if (ram_value%256==0) {
            select = (select+1)%N;
            eeprom_update_block(&select, address, 1); // Set select value
            // For debug only, comment out if you are not testing the library
            // if (select==0 and sizeof(T)>1) for(uint8_t i=1;i<sizeof(T);i++) eeprom_update_byte(address+N+i,0xCC);
        }
        eeprom_update_block( &ram_value, address+1+select, sizeof(T) );
    }

    public:

    NvCounter() {
        const size_t start = (size_t)(uintptr_t)nv_index();
        nv_reserve_or_die((size_t)sizeof(T) + (size_t)N);
        address = (uint8_t*)(uintptr_t)start;
        eeprom_read_block(&select, address, 1); // Get select value
        select = select%N;
        eeprom_read_block( &ram_value, address+1+select, sizeof(T) );
    }

    operator T() const { return ram_value; }

    NvCounter & operator = (T v) {
        if (v != ram_value) {
            ram_value=v;
            update_eeprom();
        }
        return *this;
    }

    NvCounter & operator = (const NvCounter<T,N> & v) {
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
        return size_t(address);
    }

    size_t end() const {
        return size_t(address)+sizeof(T)+N-1;
    }

    // For debugging purposes. Returns the value of the "select" variable
    uint8_t selected() const {
        return select;
    }
};

#endif // NONVOLATILE_H
