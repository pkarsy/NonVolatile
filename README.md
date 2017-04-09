# nonVolatile
Arduino persistent numeric variables. They can be used like
global variables, but underneath all changes are saved to EEPROM

In global section:

```C++
#include <nonVolatile.h>

// WARNING : nonVolatile can only be GLOBAL variables
nonVolatile<int> counter;           // Uses EEPROM addrees 0-1
nonVolatile<byte> triesNumber;      // Uses address 2
nonVolatile<uint16_t> elevation[10];   // Uses address 3-22

```
Now in the code

```C++

// = ++v v++ --v v-- += -= *= /= etc are supported
counter++; // The change is written to EEPROM

for (byte i=0;i<triesNumber;i++) {
	// Try something
}
// we can use elevation[0] - elevation[9]
Serial.print(elevation[5]);

elevation[5]=100; // Immediatelly writen to EEPROM

elevation[5]=100; // The changes are not saved again to reduce EEPROM wear
elevation[5]++; // postfix
--elevation[6]; // prefix
....
// Set all values to 0
for (int i=0;i<10;i++) elevation[i]=0;

```
### Installation
Put the directory "nonVolatile" in the Arduino "libraries" location.
Or you can put the "nonVolatile.h" file in the same location as the .ino file

### Caveats
Although nonVolatile variables look and feel just like normal variables,
perhaps too much, there are some things to keep in mind:
- The location in EEPROM is determined at runtime. If the order of the
declarations change, in the next upload, the values of nonVolatile variables
can change to different and unexpected values. For a solution see "guard variable"
below.
- **You must make sure you are not modify them too often** or the EEPROM
will wear soon (about 100000 writes).
- **You must declare them as global variables.** The eeprom location is
determined at runtime. If they are local, they point to elevated EEPROM
location every time. **There is no boundary check in the code**.
Declare all of them one after another, like the example above. And of course change the
order only if you are going to reset the values.
- In contrast to normal global variables, they are not get 0 at boot.
This is the purpose of their existence.
- In contrast with avr-libc EEMEM variables they are not updated with
code upload. If the initial value is important (and usually is) you can use
a "guard" variable (see below).
- Reading a nonVolatile should be fast, but writing is slow
(about 3ms/byte), due to EEPROM write. This can be important in time critical
code.
- Every nonVolatile variable eats some RAM, in addition to the EEPROM space it uses.
sizeof(var) returns the RAM usage of the object, and not the RAM of the basetype.
- Do not change nonVolatile variables by direct EEPROM manipulations.
- If you feel a nonVolatile looks like a normal variable more than it should,
you can prefix it with something meaningful like **nv_elevation**
- Large arrays are RAM hungry. sizeof(arr) gives the RAM they use.

### Guard variabe

This fragment of code inside setup() contains all the nonVolatile variables we need
```C++
#define MAGIC_VALUE 12345
if (eeprom_guard!=MAGIC_VALUE) {
    PRINTLN("Variables set to 0")
    counter=0;
    longvar=0;
    for (int i=0;i<5;i++) arr[i]=0;
    float1=0;
    float2=0;
    eeprom_guard=MAGIC_VALUE;
}

```

and runs only once. If we change the declarations, most likely
the code will run again (due to eeprom_guard misaligment) but
to be sure we can change the MAGIC_VALUE to 12340.
Note that "eeprom_guard" must be itself an nonVolatile. See the
[example](https://github.com/pkarsy/nonVolatile/blob/master/example/example.ino)

