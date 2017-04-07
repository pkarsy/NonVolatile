//
// This file is part of the eepromVar arduino library
//
#include <eepromVar.h>

eepromArray<uint16_t> a(80,4); // Uses eeprom locations 80-87 for 4 integers

void setup() {
	Serial.begin(57600);
	Serial.println("");
	Serial.println("eepromArray example");
	
	a[0]/=2;
	a[1]++;
	a[2]+=10;
	a[3]*=2;
	
	for (byte i=0;i<a.size;i++) {
		Serial.print(a[i]);
		Serial.print(' ');
	}
	Serial.println();
	Serial.println("Press Arduino Reset to see the values");
	Serial.println("Press \"1\" to set all values to 1000");
}

void loop() {
	if (Serial.read()=='1') {
		for (byte i=0;i<a.size;i++) a[i]=1000;
		Serial.println("All elements == 1000");
	}
}
