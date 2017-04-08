/*
 *
 * Copyright 2016 Panagiotis Karagiannis
 * eepromVar library: persistent (on EEPROM) Arduino variables
 * 
 * This code is licenced under the
 * Apache License 2.0
 *
 * Project site
 * https://github.com/pkarsy/eepromVar
 *
*/


// The eepromvar_idx is global and points to the EEPROM location
// of the next eepromVar to be declared
#include <avr/eeprom.h>
#ifdef EEPROMVAR_START
static byte* eepromvar_idx=(byte*)EEPROMVAR_START;
#else
static byte* eepromvar_idx;
#endif

template <typename T>
class eepromVar {
	private:
	
	T ram_value;
	byte* address;

	void update_eeprom() {
		eeprom_update_block( &ram_value, address, sizeof(T) );
	}

	public:
	
	
	
	eepromVar() {
		address = eepromvar_idx;
		eeprom_read_block(&ram_value, (byte*)address, sizeof(T) );
		eepromvar_idx+=sizeof(T);
	}

	operator T() const { return ram_value; }

	eepromVar & operator = (T v) {
		ram_value=v;
		update_eeprom();
		return *this;
	}

	eepromVar & operator -= (T v) {
		ram_value -= v;
		update_eeprom();
		return *this;
	}

	eepromVar & operator += (T v) {
		ram_value += v;
		update_eeprom();
		return *this;
	}

	eepromVar & operator *= (T v) {
		ram_value *= v;
		update_eeprom();
		return *this;
	}

	eepromVar & operator /= (T v) {
		ram_value /= v;
		update_eeprom();
		return *this;
	}

	eepromVar & operator ++() { // prefix ++v
		ram_value ++;
		update_eeprom();
		return *this;
	}

	T operator ++(int) { // postfix v++
		T val = ram_value; 		// make a copy of the old value
        ++(*this);              // Now use the prefix version to do the work        
		return val; // return the copy (the old) value.
	}

	eepromVar & operator --() { // prefix --v
		ram_value --;
		update_eeprom();
		return *this;
	}
	
	T operator --(int) { // postfix v--
		T val = ram_value; 		// make a copy for result
        --(*this);              // Now use the prefix version to do the work
		return val;				// return the copy (the old) value.
	}

	size_t addr() {
		return size_t(address);
	}
};

