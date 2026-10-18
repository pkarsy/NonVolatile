#include <NonVolatile.h>

/* Only global definitions are allowed
*  Ok global variables are a BAD thing
* but do not try to enforce this on NonVolatile variables */
NonVolatile<int> intvar;

/* Leaves 4 EEPROM bytes unused. Generally you don't need/want this */
// NvSpace _dummy_obj1(4);

NonVolatile<bool> boolvar;
NonVolatile<long> longarray[5];

// The "100" means the next NonVolatile variable will use EEPROM location 100
// The "_dummy_obj2" object is needed only for the syntax, it is not used anywhere.
// Generally you don't need/want this.
// NvAddress _dummy_obj2(100);

NonVolatile<float> floatvar;
NonVolatile<unsigned int> eeprom_guard;

// Compile-time EEPROM addresses. These are independent of the runtime
// cursor and are laid out at fixed locations.
NonVolatileAt<uint16_t, 200> fixed_counter;   // EEPROM 200..201
NvArrayAt<int, 202, 3> fixed_array;           // EEPROM 202..207 on AVR

// These macros are for convenience, to avoid tedious Serial.print(F("message"));
#define PRINT(x) Serial.print(F(x));
#define PRINTLN(x) Serial.println(F(x));

void setup() {
    Serial.begin(57600);
    delay(1);
    Serial.println();
    PRINTLN("############# NonVolatile tests #############");
    PRINTLN("##### The values persist between resets #####");

    // This code runs at first upload,
    // when the EEPROM_GUARD_SIGNATURE changes,
    // or when some NonVolatile declarations change,
    // causing the eeprom_guard variable to point to a different
    // eeprom loaction
    #define EEPROM_GUARD_SIGNATURE 12345
    if (eeprom_guard!=EEPROM_GUARD_SIGNATURE) {
        PRINTLN("eeprom_guard != EEPROM_GUARD_SIGNATURE");
        PRINTLN("NonVolatile variables set to 0");
        intvar=0;
        boolvar= false;
        for (int i=0;i<5;i++) longarray[i]=0;
        floatvar=0;
        fixed_counter=0;
        for (int i=0;i<3;i++) fixed_array[i]=0;
        eeprom_guard=EEPROM_GUARD_SIGNATURE;
    }

    // The new (2 bytes) value is written to EEPROM
    intvar++;
    PRINT("intvar=");
    Serial.print(intvar);
    PRINT(" EEPROM : ");
    Serial.print(intvar.start());
    PRINT("..");
    Serial.println(intvar.end());

    // The new (1 byte) value is written to EEPROM
    boolvar = ! boolvar;
    PRINT("boolvar=");
    Serial.print(boolvar);
    PRINT(" EEPROM : ");
    Serial.print(boolvar.start());
    PRINT("..");
    Serial.println(boolvar.end());

    longarray[3]+=1000;

    for (unsigned int i=0;i<sizeof(longarray)/sizeof(longarray[0]);i++) {
        PRINT("longarray["); Serial.print(i); PRINT("] = ");
        Serial.print(longarray[i]); PRINT(" EEPROM : ");
        Serial.print(longarray[i].start());
        PRINT("..");
        Serial.println(longarray[i].end());
    }

    floatvar+=0.2;
    PRINT("floatvar=");
    Serial.print(floatvar);
    PRINT(" EEPROM : ");
    Serial.print(floatvar.start());
    PRINT("..");
    Serial.println(floatvar.end());

    // Compile-time addressed variables support the same operators.
    fixed_counter++;
    PRINT("fixed_counter=");
    Serial.print((unsigned int)fixed_counter);
    PRINT(" EEPROM : ");
    Serial.print(fixed_counter.start());
    PRINT("..");
    Serial.println(fixed_counter.end());

    fixed_array[1]+=5;
    for (int i=0;i<3;i++) {
        PRINT("fixed_array["); Serial.print(i); PRINT("] = ");
        Serial.print((int)fixed_array[i]); PRINT(" EEPROM : ");
        Serial.print(fixed_array[i].start());
        PRINT("..");
        Serial.println(fixed_array[i].end());
    }

    {
        // Never declare NonVolatile variables with local scope
        // not even static variables
        // The eeprom location is determined at runtime
        // and an automatic variable will point to different (elevated)
        // eeprom location every time
    }

    // Change to false to hide RAM report
    if (true) {
        PRINTLN("RAM:");
        PRINT("sizeof(boolvar)=");Serial.println(sizeof(boolvar));
        PRINT("For comparison sizeof(bool)=");Serial.println(sizeof(bool));
        PRINTLN("RAM usage is the RAM the basetype uses, plus 2");
    }

    PRINTLN("##### Reset the module to repeat the test #####");
}

void loop() {
}
