
#include <eepromVar.h>

eepromVar<int> counter(0); // Uses EEPROM location 0-1 (2 bytes)
eepromVar<byte> ledState(4); // Uses EEPROM location 4 (1 byte)

eepromVar<int> a[10](8); //[10];
int b[10];

#define LED_PIN 13

void setup() {
	Serial.begin(57600);
	Serial.println();
	Serial.println("eepromVar example 1");
	pinMode(LED_PIN, OUTPUT);
	// To initialize the variables change this to true and upload
	// then back to false and upload again
	// for a different approach see the second example
	if (false) {
		counter=0;
		ledState=0;
	}
	counter++; // The new counter is written to EEPROM 
	Serial.print("counter=");
	Serial.print(counter);
	ledState=! ledState; // The new state is written to EEPROM
	digitalWrite(LED_PIN, ledState );
	Serial.print(" LED=");
	Serial.println(ledState);
	Serial.println("Press the Arduino reset button");

}

void loop() {
	
}
