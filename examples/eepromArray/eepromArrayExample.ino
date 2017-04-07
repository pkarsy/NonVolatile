// WARNING this sketch can write to the EEPROM
// 71582 times (!) if you leave it running for 50 days

#include <eepromVar.h>

// All eepromVar and eepromArray variables are not initialized with zero but
// they get whatever value is in EEPROM

eepromVar<uint16_t> eepromGuard(4); // Uses EEPROM location 4-5 (2 bytes)

eepromArray<uint16_t> keycounter(100,10); // EEPROM 100-119 

#define EEPROM_IS_SET 0xBEEF // Change this if you need to reset the eepromVar variables

void setup() {
	Serial.begin(57600);
	Serial.println();
	Serial.println("eepromArray example");
	//Serial.println("Counts keypress");
	
	// The variables are set in the first run
	if (eepromGuard!=EEPROM_IS_SET) {
		Serial.println("Initializing eepromVar variables");
		for (byte i=0;i<keycounter.size;i++) keycounter[i]=0;
		eepromGuard=EEPROM_IS_SET;
	}
	Serial.println("keypress statistics");
	for (byte i=0;i<keycounter.size;i++) {
		Serial.print("key \"");
		Serial.write(i+'0');
		Serial.print("\":");
		Serial.println(keycounter[i]);
	}
	
	Serial.println("Press keys 0-9");

}

void loop() {

	int a = Serial.read();
	if (a>='0' and a<='9') {
		a-='0';
		keycounter[a]++;
		Serial.print("You pressed the key \"");
		Serial.print(a);
		Serial.print("\", ");
		Serial.print(keycounter[a]);
		Serial.println(" times");
	}
}
