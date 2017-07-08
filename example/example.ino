#include <NonVolatile.h>

// Only global definitions are allowed
// Ok global variables are a BAD thing
// but do not try to enforce this on NonVolatile variables

NonVolatile<int> intvar; // Uses EEPROM location 0-1 (2 EEPROM bytes)
NonVolatile<long> longvar; // Uses EEPROM location 2-5 (4 EEPROM bytes)
// Leaves 4 EEPROM bytes unused.
NvSpace _dummy_obj1(4);
NonVolatile<long> longarray[5]; // Uses EEPROM locations 10-29 (20 EEPROM bytes)

// EEPROM address 26-99 are not used. This can be usefull if you plan to add NonVolatile
// variables later, in the first group of variables.

// In global section we cannot just write :
// non_volatile_idx = 100;
// It is a compile error
// The 100 means the next NonVolatile variable will use EEPROM address 100
// The "_dummy_obj_" object is needed only for the syntax, it is not consuming RAM and is not used anywhere.
NvAddress _dummy_obj_(100);

NonVolatile<float> float1; // it uses EEPROM address 100-103 (8bit AVR floats are 4bytes)
NonVolatile<float> float2; // Address 104-107.
NonVolatile<unsigned long> eeprom_guard;

// These macros are for convenience, to avoid tedious Serial.print(F("message"));
#define PRINT(x) Serial.print(F(x));
#define PRINTLN(x) Serial.println(F(x));

void setup() {
    Serial.begin(57600);

    PRINTLN("############# NonVolatile tests ###############");
    PRINTLN("##### The values persist between resets #####");

    // This code runs at first upload
    // or when the EEPROM_GUARD_SIGNATURE changes
    #define EEPROM_GUARD_SIGNATURE 12345
    if (eeprom_guard!=EEPROM_GUARD_SIGNATURE) {
        PRINTLN("NonVolatile variables set to 0")
        intvar=0;
        longvar=0;
        for (int i=0;i<5;i++) longarray[i]=0;
        float1=0;
        float2=0;
        eeprom_guard=EEPROM_GUARD_SIGNATURE;
    }

    intvar--; // The new int(2 bytes) value is written to EEPROM
    PRINT("intvar="); Serial.println(intvar);

    longarray[3]+=1000;
    PRINT("longarray[3]="); Serial.println(longarray[3]);

    float1+=0.2;
    PRINT("float1="); Serial.println(float1);
    {
        // Never declare NonVolatile variables with local scope
        // not even static variables
        // The eeprom location is determined at runtime
        // and an automatic variable will point to different (elevated)
        // eeprom location every time
    }

    // Change to false to hide addresses report
    if (true) {
        PRINTLN("Any change in NonVolatile declarations can change the addresses");
        PRINT("intvar.addr()=");Serial.println(intvar.addr());
        PRINT("longvar.addr()=");Serial.println(longvar.addr());

        PRINT("longarray[] EEPROM addresses=[");
        for (int i=0;i<5;i++) {
            Serial.print(longarray[i].addr());
            PRINT(" ");
        }
        PRINTLN("]");

        PRINT("float1.addr()=");Serial.println(float1.addr());
        PRINT("float2.addr()=");Serial.println(float2.addr());
        PRINT("eeprom_guard.addr()=");Serial.println(eeprom_guard.addr());
        PRINT("sizeof(longvar)=");Serial.println(sizeof(longvar));
        PRINT("For comparison sizeof(long)=");Serial.println(sizeof(long));
        PRINT("sizeof(longarray)=");Serial.print(sizeof(longarray));PRINTLN(" bytes of RAM");
    }
}

void loop() {
    // Never declare NonVolatile variables with local scope
    // not even static variables
    // NonVolatile<int> i; // VERY BAD !
    // Serial.println(i.addr());
    delay(500);
}
