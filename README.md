# eepromVar
Arduino persistent numeric variables

In global section:
```C++
#include <eepromVar.h>
// All these variables are not initialized with zero but they get
// whatever value is in EEPROM
eepromVar<int> counter(0); // Uses EEPROM location 0-1 (2 bytes)
eepromVar<byte> ledState(4); // Uses EEPROM location 4 (1 byte)
eepromVar<byte> triesNumber(8); // Uses EEPROM location 8 (1 byte)
eepromVar<uint32_t> key(12); // Uses EEPROM location 12-15 (4 bytes)
// Multiples of 4 are a bit sparse but reduce the risk of collisions
...
eepromArray<uint16_t> elevation(80,10); // Uses eeprom locations 80-99 for 10 integers
```
Now in the code
```C++
// The operators + - += -= ++ -- = == are supported
// The change is written to EEPROM
counter++; 
ledState=HIGH; // 
ledState=HIGH; // The value is NOT written again in EERPOM
...
...
digitalWrite(LED_PIN, ledState); // The arduino LED is ON

for (byte i=0;i<triesNumber;i++) {
	// Try something
}
Serial.print(elevation[5]);
elevation.write(5,100);
// elevation[5]=100; does not work.
// If anyone knows how to fix this pls help
```
### Installation
Put the directory "eepromVar" in the Arduino "libraries" location.
Or you can put the "eepromVar.h" file in the same location as the .ino file

### Caveats
Although eepromVar variables look and feel just like normal variables,
there are some things to keep in mind:
- **You must make sure you are not modify them too often** or the EEPROM
will wear soon (about 100000 writes).
- Almost certainly you must declare them as global variables (or static)
If they are local, the initialization code will run at every loop. It is
advised to declare all of them one after another, like the example above,
to be sure every one is using its own EEPROM location.
- In contrast with normal global variables, they are not get 0 at boot.
- In contrast with avr-libc EMEM variables they are not initialized with
code upload. If the initial value is important you can use the trick of
a "guard" variable see example 2.
- Reading an eepromVar variable should be fast, as the value is
in RAM, but writing is slow (about 3ms), due to EEPROM write. This can be
important in time critical code.
- You cannot declare an array of eepromVar variables. There is a distinct
eepromArray type for this.
- Every eepromVar variable eats RAM, just like a normal variable, in
addition to the EEPROM space it uses.
