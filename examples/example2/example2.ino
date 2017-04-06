
#include <eepromVar.h>

// All these variables are not initialized with zero but they get
// whatever value is in EEPROM
eepromVar<int> setupCounter(0); // Uses EEPROM location 0-1 (2 bytes)
//eepromVar<byte> ledState(4); // Uses EEPROM location 4 (1 byte)
//eepromVar<byte> triesNumber(8); // Uses EEPROM location 8 (1 byte)
eepromVar<uint32_t> uptime(12); // Uses EEPROM location 12-15 (4 bytes)
eepromVar<uint16_t> eepromGuard(16); // Uses EEPROM location 16-17 (2 bytes)

eepromArray<int> elevation(100,20);

#define EEPROM_IS_SET 0xBEEF

void setup() {
	Serial.begin(57600);
	Serial.println();
	Serial.println("eepromVar example 2");
	Serial.println("Best uptime");
	
	// The variables are set in the first run
	if (eepromGuard!=EEPROM_IS_SET) {
		Serial.println("Initializing eepromVar variables");
		//counter=0;
		setupCounter=0;
		uptime=100000;
		eepromGuard=EEPROM_IS_SET;
	}
	setupCounter++; // The new counter is written to EEPROM 
	Serial.print("Boot counter = ");
	Serial.println(setupCounter);

	Serial.println("Press the Arduino reset button");
	Serial.println(uptime);
}

void loop() {
	if ( millis()>uptime and millis()-uptime>60000 ) {
		uptime += 60000;
		Serial.print("New uptime record : ");
		Serial.println(uptime);
	}
}
