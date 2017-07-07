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
//namespace NONVOL {
//#ifdef EEPROM_START
//static byte* non_volatile_idx=(byte*)EEPROM_START;
//#else
static byte* non_volatile_idx;
//#endif
//}

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
    //protected:
    //static byte* non_volatile_idx;
    public:
    NvAddress(const unsigned int new_addr) {
        non_volatile_idx = (byte*)new_addr;
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

    //NonVolatile(const unsigned int new_addr) {
    //    address = (byte*)new_addr;
    //    eeprom_read_block(&ram_value, address, sizeof(T) ); // (byte*)
    //    non_volatile_idx = (byte*)new_addr + sizeof(T);
    //}

    operator T() const { return ram_value; }

    NonVolatile & operator = (T v) {
        ram_value=v;
        update_eeprom();
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

    size_t addr() {
        return size_t(address);
    }
};

// only for uint8_t uint16_t uint32_t
// [ select=0 03 00 00 00 ex1 ex2 ex3 exN ] # N=4
template <typename T, byte N>
class NvCounter {
    private:

    T ram_value;
    byte* address;
    byte select;

    void update_eeprom() {
        if (ram_value%256==0) {
            //select++;
            //if (select==N) select = 0
            select = (select+1)%N;
            eeprom_update_block(&select, address, 1); // Set select value
        }
        eeprom_update_block( &ram_value, address+1+select, sizeof(T) );
    }

    public:

    NvCounter() {
        address = non_volatile_idx;

        eeprom_read_block(&select, address, 1); // Get select value
        select = select%N;
        eeprom_read_block( &ram_value, address+1+select, sizeof(T) );

        non_volatile_idx += (sizeof(T)+N+1);

    }

    operator T() const { return ram_value; }

    NvCounter & operator = (T v) {
        if (v != ram_value) {
            ram_value=v;
            update_eeprom();
        }
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

    NvCounter & operator --() { // prefix --v
        ram_value --;
        update_eeprom();
        return *this;
    }

    T operator --(int) {        // postfix v--
        T val = ram_value;      // make a copy for result
        --(*this);              // Now use the prefix version to do the work
        return val;             // return the copy (the old) value.
    }

};
