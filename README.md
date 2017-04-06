# eepromVar
Arduino persistent numeric variables

In global section:
```
eepromVar<int> counter(0); // Uses EEPROM location 0-1 (2 bytes)
eepromVar<bool> ledlight(4) // Uses EEPROM location 4 (1 byte)
eepromVar<byte> triesNumber(8) // Uses EEPROM location 8 (1 byte)
eepromVar<uint32_t> key(12) // Uses EEPROM location 12-15 (4 bytes)
// Multiples of 4 are a bit sparse but reduce the risk of colissions
...
eepromArray<uint16_t> elevation(80,10); // Uses eeprom locations 80-99 for 10 integers
```
Now in the setup()
```
counter++; // The change is written to EEPROM
leflight=true;

for (byte i=0;i<triesNumber;i++) {
	// Try something
}	
```
### Installation
It is just a single file: "eepromVar.h"
You can put the directory "eepromVar" in the "libraries" Arduino location
Or you can put the "eepromVar.h" file in the same location as the .ino file

### Caveats
Althrough eepromVar variables look and feel just like normal variables
there are some things to keep in mind:
- **You must make sure you are not modify them too often** or the EEPROM
will wear soon (about 100000 writes)
- Almost certainly you must declare them as global variables (or static)
If they are local, the initialization code will run at every loop. Even
better declare all of them one after another, like the example above,
to be sure every one is using its own EEPROM location.
- In contrast with avr-libc EMEM variables they are not initialized with
code upload. If the initial value is important you can use the trick of
a "guard" variable see example.
- Reading a eepromVar variable should be quite fast, as the variable is in
RAM but writing is slow (about 3ms) due to EEPROM write. This can be
important in time critical code.
- Do not try to use an eepromVar variable as array. Do not do
eepromVar<int> a(4)[10]; All 10 elements to the array will point to the
same EEPROM location 4. Use the eepromArray construct.

### eepromArray
The eepromVar.h defines another one contruct "eepromArray"
