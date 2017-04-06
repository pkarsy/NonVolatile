/*
 * 
 * Copyright 2016 Panagiotis Karagiannis
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


///////////////////////////////////////////////////////////////////////////////
//template <typename T>
//class eepromArrayItem {
//
//};


template <typename T>
class eepromArray {
	private:
	
	public:
	const unsigned int addr;
	const unsigned int size;
	
	void write(const uint16_t n, const T val) const {
		//if (n>=size) PRINTLN("WARNING: index out of bounds %u", n);
		// void eeprom_update_block(const void * __src, void * __dst, size_t __n )
		volatile uint16_t t = addr+sizeof(T)*(n%size);
		volatile uint16_t val1=val;
		eeprom_update_block( (void*)&val1, (void*)t , sizeof(T) );
		
		//PRINTLN("eepromArray (%u,%u)=%u", addr+sizeof(T)*(n%size), sizeof(T),uint16_t(val) );
	}

	eepromArray(const unsigned int _address, const uint8_t _size): addr(_address), size(_size) {}

	
	//operator T() const { return ram_value; }

	T operator [](const unsigned int i) const {
		//return read(i);
		//if (i>=size) PRINTLN("WARNING: index out of bounds %u", i);
		T val;
		// void eeprom_read_block (void * __dst, const void * __src, size_t __n )
		eeprom_read_block(&val, (byte*)addr + sizeof(T)*(i%size), sizeof(T) );
		//PRINTLN("eepromArray read(%u,%u)=0x%x", addr+sizeof(T)*(i%size), sizeof(T),uint16_t(val) );
		return val;
	}

	void fill(const T c) const {
		for (unsigned int i=0;i<size;i++) {
			write(i,c);
		}
	}
};

