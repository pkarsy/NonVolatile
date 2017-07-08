#include <NonVolatile.h>

// Only global definitions are allowed
// Ok global variables are a BAD thing
// but do not try to enforce this on NonVolatile variables

NonVolatile<int> intvar;

// Leaves 4 EEPROM bytes unused. Generally you don't need/want this
// NvSpace _dummy_obj1(4);

NonVolatile<bool> boolvar;
NonVolatile<long> longarray[5];

// The "100" means the next NonVolatile variable will use EEPROM location 100
// The "_dummy_obj2" object is needed only for the syntax, it is not consuming RAM and is not used anywhere.
// the compiled code will contain the equivalent of
// non_volatile_idx = 100;
// Generally you don't need/want this
// NvAddress _dummy_obj2(100);

NonVolatile<float> floatvar;
NonVolatile<unsigned long> eeprom_guard;

////// These macros are for convenience, to avoid tedious Serial.print(F("message"));
#define PRINT(x) Serial.print(F(x));
#define PRINTLN(x) Serial.println(F(x));
////////////////////////////////////////////////////////////////////////////////

void setup() {
    Serial.begin(57600);
    delay(1);
    Serial.println();
    PRINTLN("############# NonVolatile tests #############");
    PRINTLN("##### The values persist between resets #####");

    // This code runs at first upload
    // or when the EEPROM_GUARD_SIGNATURE changes
    #define EEPROM_GUARD_SIGNATURE 12345
    if (eeprom_guard!=EEPROM_GUARD_SIGNATURE) {
        PRINTLN("eeprom_guard != EEPROM_GUARD_SIGNATURE");
        PRINTLN("NonVolatile variables set to 0");
        intvar=0;
        boolvar= false;
        for (int i=0;i<5;i++) longarray[i]=0;
        floatvar=0;
        //float2=0;
        eeprom_guard=EEPROM_GUARD_SIGNATURE;
    }

    intvar++; // The new int(2 bytes) value is written to EEPROM
    PRINT("intvar=");
    Serial.print(intvar);
    PRINT(" EEPROM : ");
    Serial.print(intvar.start());
    PRINT("..");
    Serial.println(intvar.end());

    boolvar = ! boolvar; // The new int(2 bytes) value is written to EEPROM
    PRINT("boolvar=");
    Serial.print(boolvar);
    PRINT(" EEPROM : ");
    Serial.print(boolvar.start());
    PRINT("..");
    Serial.println(boolvar.end());

    longarray[3]+=1000;
    //PRINT("longarray[3]=");
    //Serial.print(longarray[3]);
    //PRINT(" EEPROM : ");
    //Serial.print(longarray[3].start());
    //PRINT("..");
    //Serial.println(longarray[3].end());
    for (unsigned int i=0;i<sizeof(longarray)/sizeof(longarray[0]);i++) {
        PRINT("longarray["); Serial.print(i); PRINT("] = ");
        Serial.print(longarray[i]); PRINT(" EEPROM : ");
        Serial.print(longarray[i].start());
        PRINT("..");
        Serial.print(longarray[i].end());
        //PRINT(" ");
        Serial.println();
    }

    floatvar+=0.2;
    PRINT("floatvar=");
    Serial.print(floatvar);
    PRINT(" EEPROM : ");
    Serial.print(floatvar.start());
    PRINT("..");
    Serial.println(floatvar.end());

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
        PRINTLN("RAM usage is the RAM the basetype uses plus 2");
        //PRINT("sizeof(longarray)=");Serial.print(sizeof(longarray));PRINTLN("(bytes of RAM)");
    }

    PRINTLN("##### END #####");
}

void loop() {
    // Never declare NonVolatile variables with local scope
    // not even static variables
    // NonVolatile<int> i; // VERY BAD !
    // Serial.println(i.start());
    delay(500);
}
