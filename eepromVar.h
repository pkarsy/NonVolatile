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

	void update_eeprom2(const T v) {
		byte * ramptr = (byte*)&ram_value;
		byte * newval = (byte*)&v;
		for (uint16_t i=0; i<sizeof(T); i++) {
			if (ramptr[i]!=newval[i]) {
				eeprom_write_byte( (byte*)address+i, newval[i]);
				ramptr[i]=newval[i];
			}
		}
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
		//eeprom_update_block( &ram_value, (byte*)address, sizeof(T) );
		return *this;
	}
	//T & operator = (T v) {
	//	ram_value=v;
	//	update_eeprom();
	//	return ram_value;
	//}

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
	//eepromVar<U> item2(0);


	eepromArray(const unsigned int _address, const uint8_t _size): addr(_address), size(_size) {};


	eepromVar<U> operator [](const unsigned int i) {
		eepromVar<U> item(addr + sizeof(U)*(i%size));
		return item;
	}

	//void fill(const U c) const {
	//	for (unsigned int i=0;i<size;i++) {
	//		write(i,c);
	//	}
	//}
};

