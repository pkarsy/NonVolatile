# NonVolatile (Non Volatile Variable)
Arduino persistent bool and numeric variables. They can be used like
global variables, but underneath all changes are saved to EEPROM

In global section:

```C++
#include <NonVolatile.h>
// WARNING : NonVolatile can only be GLOBAL variables
NonVolatile<uint16_t> counter;      // Uses EEPROM addreeses 0-1
NonVolatile<int> elevation[10];     // Uses addresses 3-22
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
Put the directory "NonVolatile" in the Arduino "libraries" location.
Or you can put the "NonVolatile.h" file in the same location as the .ino file

### Types
- **NonVolatile\<T\>** The most importand type this library offers. **T** can be bool,char,int,long
and the unsigned counterparts.
- **NvCounter\<T,N\>** usually **T** is uint32_t(unsigned long). This special type
is usefull only as counter (++ is the only operator working), and uses more EEPROM bytes
but spreads the writes across the N extra bytes doind EEPROM wear leveling. So if every
EEPROM cell withands 100000 writes, the counter can be incremented about 100000*N times.
See NvCounter Example.

The next 2 types are not variables but can be used to set the eeprom address used by the variables. Ususally they are
not needed and can lead to hard to examine bugs, in case you set the EEPROM location wrong.

- **NvAddress _dummy_var(addr)** Sets the EEPROM location of the next NonVolatile or NvCounter
variable to **addr**. See NvExample.
- **NvSpace _dummy_var2(no_of_bytes)** Reserves some space in EEPROM, in case you need it for other purpose.
The next defined variable, will point to the next byte after the reserved space. See NvExample.

### Intended use
NonVolatile has a very specific purpose. To make it easier to store and use, the (usually few)
settings an Arduino project needs. It is unsuitable for data logging. Generally if there
is a need to use a significant percentage of the EEPROM, you have to use another method.

### Caveats
Although NonVolatile variables look and feel just like normal variables,
there are some things to keep in mind:
- **You must declare them as global variables.** The eeprom location is
determined at runtime. If they are local, they point to elevated EEPROM
location every time. **There are no boundary checks in the code**.
Declare all of them one after another,
like the example above.
- The location in EEPROM is determined at runtime. **If the order of the
declarations change, the values of NonVolatile variables
can change to different and unexpected values.** If you change the order
during development, you must also reset all NonVolatile variables. See
"guard variable" below.
- **You must make sure you are not modify them too often** or the EEPROM
will wear soon (about 100000 writes).
- In contrast to normal global variables, they are not get 0 at boot.
This is the purpose of their existence.
- In contrast with avr-libc EEMEM variables they are not updated with
code upload. If the initial value is important (and usually is) you can
use a "guard" variable (see below).
- Reading a NonVolatile should be fast, but writing is slow
(about 3ms/byte), due to EEPROM write. This can be important in time
critical code.
- Every NonVolatile variable eats some RAM, in addition to the EEPROM space
it uses. sizeof(var) returns the RAM usage of the object, and not the RAM of
the basetype.
- Do not change NonVolatile variables by direct EEPROM manipulations.
- If you feel a NonVolatile looks like a normal variable more than it
should, you can prefix it with something meaningful like **nv_elevation**
- Large arrays are RAM hungry. sizeof(arr) gives the RAM they use.


### Guard variabe

This fragment of code inside setup() contains all the NonVolatile variables we need
```C++
#define MAGIC_VALUE 12345
if (eeprom_guard!=MAGIC_VALUE) {
    // NonVolatile variables are set to 0
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
Note that "eeprom_guard" must be itself an NonVolatile. See the
[example](https://github.com/pkarsy/NonVolatile/blob/master/example/example.ino)



