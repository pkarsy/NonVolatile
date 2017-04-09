/*
 *
 * Copyright 2017 Panagiotis Karagiannis
 * nonVolatile library: persistent (on EEPROM) Arduino variables
 * 
 * This code is licenced under the
 * Apache License 2.0
 *
 * This file is part of the nonVolatile library
 * Project site
 * https://github.com/pkarsy/nonVolatile
 *
*/


// The eeprom_idx is global and points to the EEPROM location
// of the next nonVolatile to be declared
#include <avr/eeprom.h>
#ifdef EEPROM_START
static byte* eeprom_idx=(byte*)EEPROM_START;
#else
static byte* eeprom_idx;
#endif

template <typename T>
class nonVolatile {
	private:
	
	T ram_value;
	byte* address;

	void update_eeprom() {
		eeprom_update_block( &ram_value, address, sizeof(T) );
	}

	public:
	
	nonVolatile() {
		address = eeprom_idx;
		eeprom_read_block(&ram_value, (byte*)address, sizeof(T) );
		eeprom_idx+=sizeof(T);
	}

	operator T() const { return ram_value; }

	nonVolatile & operator = (T v) {
		ram_value=v;
		update_eeprom();
		return *this;
	}

	nonVolatile & operator -= (T v) {
		ram_value -= v;
		update_eeprom();
		return *this;
	}

	nonVolatile & operator += (T v) {
		ram_value += v;
		update_eeprom();
		return *this;
	}

	nonVolatile & operator *= (T v) {
		ram_value *= v;
		update_eeprom();
		return *this;
	}

	nonVolatile & operator /= (T v) {
		ram_value /= v;
		update_eeprom();
		return *this;
	}

	nonVolatile & operator ++() { // prefix ++v
		ram_value ++;
		update_eeprom();
		return *this;
	}

	T operator ++(int) {		// postfix v++
		T val = ram_value; 		// make a copy of the old value
        ++(*this);              // Now use the prefix version to do the work        
		return val;				// return the copy (the old) value.
	}

	nonVolatile & operator --() { // prefix --v
		ram_value --;
		update_eeprom();
		return *this;
	}
	
	T operator --(int) {		// postfix v--
		T val = ram_value; 		// make a copy for result
        --(*this);              // Now use the prefix version to do the work
		return val;				// return the copy (the old) value.
	}

	size_t addr() {
		return size_t(address);
	}
};

