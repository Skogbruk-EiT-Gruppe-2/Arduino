New I2S src files:
They replaces the previous I2S src files. The path on my computer are
/Users/christopherbjorge/Library/Arduino15/packages/arduino/hardware/samd/1.8.14/libraries/I2S/src/I2S.h
follow an equivalent path on your computer. To show hidden folders on mac (to see the folder Library), press cmd + shift + .

Connect the to the pins as described in the .h file,
#define PIN_I2S_SD          (PIN_A6)
#define PIN_I2S_SCK         (2u)
#define PIN_I2S_FS          (3u)

A6 is the analog pin, and is an input pin.

