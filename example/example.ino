// Uncomment to set another starting location for NonVolatile variables
// #define EEPROM_START 100

#include <NonVolatile.h>

// Only global definitions allowed
// Ok global variables are a bad thing
// but do not try to enforce this on NonVolatile

NonVolatile<int> counter; // Uses EEPROM location 0-1 (2 EEPROM bytes)
NonVolatile<long> longvar; // Uses EEPROM location 2-5 (4 EEPROM bytes)
// Change "long" to "int" to see how eeprom locations change
NonVolatile<long> arr[5]; // Uses EEPROM locations 6-25 (20 EEPROM bytes)
NonVolatile<float> float1;
NonVolatile<float> float2;
NonVolatile<unsigned long> eeprom_guard;

#define PRINT(x) Serial.print(F(x));
#define PRINTLN(x) Serial.println(F(x));

void setup() {
    Serial.begin(57600);

    Serial.println(F("############# NonVolatile tests ###############"));
    Serial.println(F("##### The values persist between resets #####"));

    // This code runs at first upload
    // or when tha MAGIC_VALUE changes
    // this is recommended if you change the declarations
    #define MAGIC_VALUE 12345
    if (eeprom_guard!=MAGIC_VALUE) {
        PRINTLN("NonVolatile variables set to 0")
        counter=0;
        longvar=0;
        for (int i=0;i<5;i++) arr[i]=0;
        float1=0;
        float2=0;
        eeprom_guard=MAGIC_VALUE;
    }

    counter--; // The new counter is written to EEPROM
    PRINT("counter="); Serial.println(counter);

    arr[3]+=1000;
    PRINT("arr[3]="); Serial.println(arr[3]);

    float1+=0.2;
    PRINT("float1="); Serial.println(float1);
    {
        // Never declare NonVolatile variables with local scope
        // not even static variables
        // The eeprom location is determined at runtime
        // and an automatic variable will point to different
        // eeprom location every time
    }

    // Change to false to hide addresses report
    if (true) {
        PRINTLN("Any change in NonVolatile declarations can change the addresses");
        PRINT("counter.addr()=");Serial.println(counter.addr());
        PRINT("longvar.addr()=");Serial.println(longvar.addr());

        PRINT("arr[] EEPROM addresses=[");
        for (int i=0;i<5;i++) {
            Serial.print(arr[i].addr());
            Serial.print(" ");
        }
        PRINTLN("]");

        PRINT("float1.addr()=");Serial.println(float1.addr());
        PRINT("float2.addr()=");Serial.println(float2.addr());
        PRINT("eeprom_guard.addr()=");Serial.println(eeprom_guard.addr());
        PRINT("sizeof(longvar)=");Serial.println(sizeof(longvar));
        PRINT("For comparison sizeof(long)=");Serial.println(sizeof(long));
        PRINT("sizeof(arr)=");Serial.print(sizeof(arr));PRINTLN(" bytes of RAM");
    }
}

void loop() {
    // Never declare NonVolatile variables with local scope
    // not even static variables
    // NonVolatile<int> i; // VERY BAD
    // Serial.println(i.addr());
    delay(500);
}
