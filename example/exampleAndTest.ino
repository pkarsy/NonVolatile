// Uncomment to set another location for eepromVar variables
// #define EEPROMVAR_START 100

#include <eepromVar.h>

eepromVar<int> counter; // Uses EEPROM location 0-1 (2 bytes)
eepromVar<long> longvar; // Uses EEPROM location 2-5 (4 bytes)
// Change "long" to "int" to see what happens
eepromVar<long> arr[5]; // Uses EEPROM locations 6-25 (1 byte)
eepromVar<float> float1;
eepromVar<float> float2;
eepromVar<unsigned long> eeprom_guard;

#define PRINT(x) Serial.print(F(x));
#define PRINTLN(x) Serial.println(F(x));


void setup() {
	Serial.begin(57600);
	
	//Serial.println("eepromVar example");
	Serial.println(F("############# eepromVar tests ###############"));
	Serial.println(F("##### The values persist between resets #####"));
	
	// This code runs at first upload
	// or when tha MAGIC_VALUE changes
	// this is recommended if you change the declarations
	#define MAGIC_VALUE 12345
	if (eeprom_guard!=MAGIC_VALUE) {
		PRINTLN("WARNING: eepromVariables set to 0")
		counter=0;
		longvar=0;
		for (int i=0;i<5;i++) arr[i]=0;
		float1=0;
		float2=0;
		eeprom_guard=MAGIC_VALUE;
	}
	counter--; // The new counter is written to EEPROM 
	Serial.print("counter=");
	Serial.println(counter);

	arr[3]+=1000;
	Serial.print("arr[3]=");
	Serial.println(arr[3]);
	
	float1+=0.2;
	Serial.print("float1=");
	Serial.println(float1);
	{
		// Never declare eepromVar variables with local scope
		// not even static variables
		// The eeprom location is determined at runtime
		// and an automatic variable will point to different
		// eeprom location every time 
	}

	// Change to false to hide addresses report
	if (true) {
		Serial.println(F("Any change in eepromVar declarations can change the addresses"));
		Serial.print("counter.address=");Serial.println(counter.addr());
		Serial.print("longvar.address=");Serial.println(longvar.addr());
		Serial.print("sizeof(longvar)=");Serial.println(sizeof(longvar));
		Serial.print("arr[] EEPROM addresses=[");
		for (int i=0;i<5;i++) {
			Serial.print(arr[i].addr());
			Serial.print(" ");
		}
		Serial.println("]");
		PRINT("sizeof(arr)=");Serial.println(sizeof(arr));
		Serial.print("float1.addr()=");Serial.println(float1.addr());
		Serial.print("float2.addr()=");Serial.println(float2.addr());
		Serial.print("eeprom_guard.addr()=");Serial.println(eeprom_guard.addr());
	}
}

void loop() {
	// Never declare eepromVar variables with local scope
	// not even static variables
}
