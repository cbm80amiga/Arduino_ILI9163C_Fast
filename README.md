# Arduino_ILI9163C_Fast
Fast ILI9163C 1.44" 128x128 LCD SPI display library

YouTube video:
https://youtu.be/V1KBm99Qagw

Significantly optimized for 16MHz AVR Arduino boards (4-6x faster than other available libraries)

Achieved 6.9Mbps SPI transfer rate (at DIV2/16MHz clock)

## Configuration

Use "#define COMPATIBILITY_MODE" - then the library should work on all Arduino compatible boards
Remove above for the best performance on 16MHz AVR

Use "#define CS_ALWAYS_LOW" for LCD boards where CS pin is internally connected to the ground, it gives better performance

"#define __144_BLACK_PCB__"  for "black" PCB version

"#define __144_RED_PCB__" for "red" PCB version

Tested with Arduino IDE 1.6.5 and Adafruit_GFX 1.5.6

## Extra Features
- invertDisplay()
- sleepDisplay()
- enableDisplay()
- idleDisplay() - saves power by limiting colors to 3 bit mode (8 colors)
- resetDisplay() - software reset
- partialDisplay() and setPartArea() - limiting display area for power saving
- setScrollArea() and setScroll() - smooth vertical scrolling
- fast drawImage() from RAM
- fast drawImage() from flash (PROGMEM)

## Connections (header at the top):

|LCD pin|LCD pin name|Arduino|
|--|--|--|
 |#01| LED| 3.3V|
 |#02| SCK |D13/SCK|
 |#03| SCA |D11/MOSI|
 |#04| A0/DC|D8 or any digital
 |#05| RESET|D9 or any digital|
 |#06| CS|D10 or any digital|
 |#07| GND | GND|
 |#08| VCC | 3.3V|

If you find it useful and want to buy me a coffee or a beer:

https://www.paypal.me/cbm80amiga
