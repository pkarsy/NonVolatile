template <typename T>
class eepromVar {
	private:
	
	T ram_value;
	
	void update_eeprom() {
		//void eeprom_update_block(const void * __src, void * __dst, size_t __n )
		//PRINTLN("eepromVar (%u,%u)=%d",address,sizeof(T),int(ram_value) );
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

	//bool operator > (T t) const {
	//	return ram_value>t;
	//}
};

/* 
template <typename T>
class eepromVarDir {
	private:
	
	void update_eeprom(T val) const {
		//void eeprom_update_block(const void * __src, void * __dst, size_t __n )
		PRINTLN("eepromVarDir (%u,%u)=%d",address,sizeof(T),int(val) );
		eeprom_update_block( &val, (byte*)address, sizeof(T) );
	}

	T read() const {
		T val;
		eeprom_read_block(&val, (byte*)address, sizeof(T) );
		return val;
	}

	public:
	const uint16_t address;

	eepromVarDir(const uint16_t _address): address(_address) {
		//eeprom_read_block(&ram_value, (byte*)address, sizeof(T) );
	}

	operator T() const  { return read(); }

	eepromVarDir & operator = (T v) {
		//ram_value=v;
		update_eeprom(v);
		return *this;
	}

	eepromVarDir & operator -= (T v) {
		//ram_value -= v;
		update_eeprom(read()-v);
		return *this;
	}

	eepromVarDir & operator += (T v) {
		//ram_value += v;
		update_eeprom(read()+v);
		return *this;
	}
	
	eepromVarDir & operator ++(int) {
		//ram_value ++;
		update_eeprom(read()+1);
		return *this;
	}
	
	eepromVarDir & operator --(int) {
		//ram_value --;
		update_eeprom(read()-1);
		return *this;
	}
}; */




///////////////////////////////////////////////////////////////////////////////
template <typename T>
class eepromArrayItem {

};


template <typename T>
class eepromArray {
	private:
	
	public:
	const unsigned int addr;
	const unsigned int size;

	//T read(const unsigned int n) const {
	//}
	
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

