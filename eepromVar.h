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

// eepromArray variables generate sometimes errorn 

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

	void update_eeprom() {
		eeprom_update_block( &ram_value, address, sizeof(T) );
	}

	public:
	
	byte* address;
	
	eepromVar() {
		address = eepromvar_idx;
		eepromvar_idx+=sizeof(T);
		eeprom_read_block(&ram_value, (byte*)address, sizeof(T) );
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

	eepromVar & operator ++(int) {
		ram_value ++;
		update_eeprom();
		return *this;
	}

	eepromVar & operator --(int) {
		ram_value --;
		update_eeprom();
		return *this;
	}
};

