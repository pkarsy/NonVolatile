// WARNING this sketch can write to the EEPROM
// 71582 times (!) if you leave it running for 50days

#include <eepromVar.h>

// All these variables are not initialized with zero but they get
// whatever value is in EEPROM
//eepromVar<int> setupCounter(0); // Uses EEPROM location 0-1 (2 bytes)
//eepromVar<byte> ledState(4); // Uses EEPROM location 4 (1 byte)
//eepromVar<byte> triesNumber(8); // Uses EEPROM location 8 (1 byte)
eepromVar<uint32_t> uptime(0); // Uses EEPROM location 12-15 (4 bytes)
eepromVar<uint16_t> eepromGuard(4); // Uses EEPROM location 16-17 (2 bytes)

eepromArray<uint16_t> keycounter(100,10);

#define EEPROM_IS_SET 0xBEEF

void setup() {
	Serial.begin(57600);
	Serial.println();
	Serial.println("eepromVar example 2");
	Serial.println("Best uptime, and digits counter");
	
	// The variables are set in the first run
	if (eepromGuard!=EEPROM_IS_SET) {
		Serial.println("Initializing eepromVar variables");
		//counter=0;
		//setupCounter=0;
		uptime=0;
		keycounter.fill(0); // all elements set to 0
		eepromGuard=EEPROM_IS_SET;
	}
	//setupCounter++; // The new counter is written to EEPROM
	Serial.println();
	Serial.print("Best uptime = ");
	Serial.print(uptime/60000);
	Serial.println("min");
	Serial.println();

	Serial.println("Press keys 0-9");

}

void loop() {
	if ( millis()>uptime and millis()-uptime>60000 ) {
		uptime += 60000;
		
		Serial.print("New uptime record : ");
		Serial.println(uptime);
		
	}
	int a = Serial.read();
	if (a>='0' and a<='9') {
		a-='0';
		// keycounter[a]++ does not work unfortunately
		keycounter.write( a, keycounter[a]+1);
		Serial.print("You pressed the key \"");
		Serial.print(a);
		Serial.print("\", ");
		Serial.print(keycounter[a]);
		Serial.println(" times");
	}
}
