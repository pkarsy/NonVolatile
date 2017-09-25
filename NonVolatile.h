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


// The non_volatile_idx points to the EEPROM location
// of the next NonVolatile to be declared
#include <avr/eeprom.h>
static byte* non_volatile_idx;


// To keep some space reserved, in the declarations
class NvSpace
{
    public:
    NvSpace(const unsigned int size) {
        non_volatile_idx += size;
    }
};

// to set a new EEPROM location for NonVolatile
class NvAddress
{
    public:
    NvAddress(const unsigned int new_addr) {
        non_volatile_idx = (byte*)new_addr;
    }
};

class NvFill
{
    public:
    NvFill(const unsigned int size) {
        byte* i = non_volatile_idx;
        non_volatile_idx += size;
        while (i<non_volatile_idx) {
            eeprom_update_byte(i,0xff);
            i++;
        }
    }
};

template <typename T>
class NonVolatile {
    private:

    T ram_value;
    byte* address;

    void update_eeprom() {
        eeprom_update_block( &ram_value, address, sizeof(T) );
    }

    public:

    NonVolatile() {
        address = non_volatile_idx;
        eeprom_read_block(&ram_value, (byte*)address, sizeof(T) );
        non_volatile_idx+=sizeof(T);
    }

    operator T() const { return ram_value; }

    //NonVolatile & operator = (T v) {
    //    ram_value=v;
    //    update_eeprom();
    //    return *this;
    //}
    T operator = (T v) {
        ram_value=v;
        update_eeprom();
        return ram_value;
    }

    // This one is importand,
    T operator = (NonVolatile<T> v) {
        //ram_value=T(v);
        //update_eeprom();
        *this = T(v);
        return ram_value;
    }

    T operator -= (T v) {
        ram_value -= v;
        update_eeprom();
        return ram_value;
    }

    T operator += (T v) {
        ram_value += v;
        update_eeprom();
        return ram_value;
    }

    T operator *= (T v) {
        ram_value *= v;
        update_eeprom();
        return ram_value;
    }

    T operator /= (T v) {
        ram_value /= v;
        update_eeprom();
        return ram_value;
    }

    T operator ++() { // prefix ++v
        ram_value ++;
        update_eeprom();
        return ram_value;
    }

    T operator ++(int) {        // postfix v++
        T val = ram_value;      // make a copy of the old value
        ++(*this);              // Now use the prefix version to do the work
        return val;             // return the copy (the old) value.
    }

    T operator --() { // prefix --v
        ram_value --;
        update_eeprom();
        return ram_value;
    }

    T operator --(int) {        // postfix v--
        T val = ram_value;      // make a copy for result
        --(*this);              // Now use the prefix version to do the work
        return val;             // return the copy (the old) value.
    }

    size_t start() {
        return size_t(address);
    }

    size_t end() {
        return size_t(address)+sizeof(T)-1;
    }
};

// wear leveling counter
// only for uint8_t uint16_t uint32_t
template <typename T, byte N>
class NvCounter {
    private:

    T ram_value;
    byte* address;
    byte select;

    void update_eeprom() {
        if (ram_value%256==0) {
            select = (select+1)%N;
            eeprom_update_block(&select, address, 1); // Set select value
            // For debug only, comment out if you are not testing the library
            // if (select==0 and sizeof(T)>1) for(byte i=1;i<sizeof(T);i++) eeprom_update_byte(address+N+i,0xCC);
        }
        eeprom_update_block( &ram_value, address+1+select, sizeof(T) );
    }

    public:

    NvCounter() {
        address = non_volatile_idx;
        eeprom_read_block(&select, address, 1); // Get select value
        select = select%N;
        eeprom_read_block( &ram_value, address+1+select, sizeof(T) );
        non_volatile_idx += sizeof(T)+N;
    }

    operator T() const { return ram_value; }

    T operator = (T v) {
        if (v != ram_value) {
            ram_value=v;
            update_eeprom();
        }
        return ram_value;
    }

    T operator = (NvCounter<T,N> v) {
        *this = T(v);
        return ram_value;
    }

    T operator ++() { // prefix ++v
        ram_value ++;
        update_eeprom();
        return ram_value;
    }

    T operator ++(int) {        // postfix v++
        T val = ram_value;      // make a copy of the old value
        ++(*this);              // Now use the prefix version to do the work
        return val;             // return the copy (the old) value.
    }

    size_t start() {
        return size_t(address);
    }

    size_t end() {
        return size_t(address)+sizeof(T)+N-1;
    }

    // For debugging purposes. Returns the value of the "select" variable
    byte selected() {
        return select;
    }
};
