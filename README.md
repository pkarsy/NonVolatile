# NvVar (Non Volatile Variable)
Arduino persistent variables. They can be used like
global variables, but underneath all changes are saved to EEPROM

In global section:

```C++
#include <NvVar.h>
// WARNING : NvVar can only be GLOBAL variables
NvVar<uint16_t> counter;      // Uses EEPROM addreeses 0-1
NvVar<int> elevation[10];     // Uses addresses 3-22
```
Now in the code
```C++
// = ++v v++ --v v-- += -= *= /= etc are supported, chaining non tested
counter++; // The change is written to EEPROM
elevation[5]=100; // Immediatelly writen to EEPROM
elevation[5]=100; // The changes are NOT saved again to reduce EEPROM wear
elevation[5]++;   // postfix
Serial.print(--elevation[5]); // prefix
for (int i=0;i<10;i++) elevation[i]=0;
```
### Installation
Put the directory "NvVar" in the Arduino "libraries" location.
Or you can put the "NvVar.h" file in the same location as the .ino file

### Intended use
NvVar has a very specific purpose. To make it easier to store the (usually) very few
settings an Arduino project needs. It is unsuitable for data logging. Generally if there
is a need to use a significant percentage of the EEPROM, you have to use another method.

### Caveats
Although NvVar variables look and feel just like normal variables,
there are some things to keep in mind:
- **You must declare them as global variables.** The eeprom location is
determined at runtime. If they are local, they point to elevated EEPROM
location every time. **There are no boundary checks in the code**.
Declare all of them one after another,
like the example above.
- The location in EEPROM is determined at runtime. **If the order of the
declarations change, the values of NvVar variables
can change to different and unexpected values.** If you change the order
during development, you must also reset all NvVar variables. See
"guard variable" below.
- **You must make sure you are not modify them too often** or the EEPROM
will wear soon (about 100000 writes).
- In contrast to normal global variables, they are not get 0 at boot.
This is the purpose of their existence.
- In contrast with avr-libc EEMEM variables they are not updated with
code upload. If the initial value is important (and usually is) you can
use a "guard" variable (see below).
- Reading a NvVar should be fast, but writing is slow
(about 3ms/byte), due to EEPROM write. This can be important in time
critical code.
- Every NvVar variable eats some RAM, in addition to the EEPROM space
it uses. sizeof(var) returns the RAM usage of the object, and not the RAM of
the basetype.
- Do not change NvVar variables by direct EEPROM manipulations.
- If you feel a NvVar looks like a normal variable more than it
should, you can prefix it with something meaningful like **nv_elevation**
- Large arrays are RAM hungry. sizeof(arr) gives the RAM they use.


### Guard variabe

This fragment of code inside setup() contains all the NvVar variables we need
```C++
#define MAGIC_VALUE 12345
if (eeprom_guard!=MAGIC_VALUE) {
    // NvVar variables are set to 0
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
Note that "eeprom_guard" must be itself an NvVar. See the
[example](https://github.com/pkarsy/NvVar/blob/master/example/example.ino)



