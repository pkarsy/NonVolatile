#include <EEPROM.h>
#include <NonVolatile.h>

const char* ANSI_YELLOW = "\e[33m";
const char* ANSI_CLEAR = "\e[0m";
const char* ANSI_GREEN = "\e[32m";

// WARNING: This test can write to the EEPROM too many times. Usually
// every EEPROM cell can be written reliably about 100000 times
// on AVR's

// Only global definitions are allowed

// This example/test is changing a counter
// and shows how the EEPROM is writen

// If we stress the EEPROM too much we can change the start address
// to continue reliably our tests. I did this during development
NvAddress _dummy_obj1(256);

NonVolatile<unsigned int> marker1;
NvCounter<unsigned int,5> counter;
NonVolatile<unsigned int> marker2;

NonVolatile<unsigned int> eeprom_guard;

////// These macros are for convenience, to avoid tedious Serial.print(F("message"));
#define PRINT(x) Serial.print(F(x));
#define PRINTLN(x) Serial.println(F(x));
////////////////////////////////////////////////////////////////////////////////

void printhex(byte i) {
    if (i<0x10) Serial.write('0');
    Serial.print(i,HEX);
}

void printhex(unsigned int i) {
    Serial.write(ANSI_YELLOW);
    printhex( (byte)(i/256) );
    printhex( (byte)(i%256) );
    Serial.write(ANSI_CLEAR);
}

void printline() {
    PRINT("counter=0x");
    printhex(counter);
    byte select = EEPROM.read( counter.start() );
    PRINT(" EEPROM: ");
    for (unsigned int i=marker1.start(); i<=marker2.end() ; i++) {
        unsigned int idx=counter.start()+1+select;

        if (i==idx) Serial.write(ANSI_YELLOW);
        else if (i==idx+2) Serial.write(ANSI_CLEAR);
        if (i==counter.start()) Serial.write(ANSI_GREEN);
        printhex(EEPROM.read(i));
        if (i==counter.start()) Serial.write(ANSI_CLEAR);
        PRINT(" ");
    }
    Serial.println();
    counter++;
}

void setup() {
    Serial.begin(57600);
    delay(1);
    Serial.println();

    marker1=0xEEEE;
    marker2=marker1;
    //marker1=counter;

    // This code runs at first upload
    // or when the EEPROM_GUARD_SIGNATURE changes
    #define EEPROM_GUARD_SIGNATURE 12346
    if (eeprom_guard!=EEPROM_GUARD_SIGNATURE) {
        PRINTLN("eeprom_guard != EEPROM_GUARD_SIGNATURE");
        PRINTLN("Init NonVolatile variables");
        counter = 0;
        eeprom_guard=EEPROM_GUARD_SIGNATURE;
    }

    PRINTLN("The \"EE EE\" at start and end are the marker1 and marker2");
    PRINTLN("You need to increment the counter at least 500 times to");
    PRINTLN("become apparent what it happens");
    PRINTLN("To increment the counter press SPACE");
}

void loop() {
    int c=Serial.read();
    if (c==' ') {
        printline();
    }
}
