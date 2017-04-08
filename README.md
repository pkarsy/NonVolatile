# eepromVar
Arduino persistent numeric variables. They can be used just like
normal variables, but their value is stored in EEPROM

In global section:
```C++
#include <eepromVar.h>
// All these variables get whatever value is in EEPROM
eepromVar<int> counter;
eepromVar<byte> ledState;
eepromVar<byte> triesNumber;
eepromVar<uint32_t> key(12);
eeprom<uint16_t> elevation[10];

```
Now in the code
```C++

// ++ -- += -= *= /= etc are supported
counter++; // The change is written to EEPROM

for (byte i=0;i<triesNumber;i++) {
	// Try something
}
Serial.print(elevation[5]);
// we can use elevation[0] - elevation[9]
elevation[5]=100;
elevation[5]++;
....
// Set all values to 0
for (int i=0;i<5;i++) elevation[i]=0;

```
### Installation
Put the directory "eepromVar" in the Arduino "libraries" location.
Or you can put the "eepromVar.h" file in the same location as the .ino file

### Caveats
Although eepromVar variables look and feel just like normal variables,
perhaps too much, there are some things to keep in mind:
- **You must make sure you are not modify them too often** or the EEPROM
will wear soon (about 100000 writes).
- **You must declare them as global variables.** The eeprom location is
determined at runtime. If they are local, they point to differnet EEPROM
location every time.
Declare all of them one after another, like the example above.
- In contrast with normal global variables, they are not get 0 at boot.
This is the purpose of their existence.
- In contrast with avr-libc EEMEM variables they are not updated with
code upload. If the initial value is important (and usually is) you can use the trick of
a "guard" variable (see the example).
- Reading an eepromVar variable should be fast, but writing is slow
(about 3ms/byte), due to EEPROM write. This can be important in time critical
code.
- Every eepromVar variable eats some RAM, in addition to the EEPROM space
it uses. sizeof(var) returns the RAM usage, and NOT the RAM  of the type it holds.
- The actual value of such a variable is also cached in RAM. This means faster reads
but every variable uses additional RAM. This behaviour is probably going to change.
- Do no try to change the eepromVar values by direct EEPROM manipulations.

