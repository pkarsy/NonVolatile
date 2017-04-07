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

#include <avr/eeprom.h>

template <typename T>
class eepromVar {
	private:

	T ram_value;

	void update_eeprom() {
		eeprom_update_block( &ram_value, (byte*)address, sizeof(T) );
	}

	public:
	const uint16_t address;

	eepromVar(const uint16_t _address): address(_address) {
		eeprom_read_block(&ram_value, (byte*)address, sizeof(T) );
		//read();
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

template <typename U>
class eepromArray {
	private:

	public:
	
	const unsigned int addr;
	const unsigned int size;


	eepromArray(const unsigned int _address, const uint8_t _size): addr(_address), size(_size) {};


	eepromVar<U> operator [](const unsigned int i) const {
		eepromVar<U> item(addr + sizeof(U)*(i%size));
		return item;
	}

	//void fill(const U c) const {
	//	for (unsigned int i=0;i<size;i++) {
	//		write(i,c);
	//	}
	//}
};

