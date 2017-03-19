PDQ_GFX_Libs with packed font support
============
Version with special "packed" font support. Also include TTF2GFX for create custom GFX fonts from TTF.
Speed up improvement for GFX text drawing about 2-3 times. 
Allow to create custom fonts with subset of chars (not all ASCII table).
Special "packing" algorithm for GFX font allow to save 20-50% of space

PDQ_GFX_Libs
============

An optimized fork of Adafruit's GFX library (and LCD drivers) for Arduino (AVR).

This is a replacement "re-mix" of the Adafruit GFX library and associated hardware drivers.

Currently supported are ILI9340, ILI9341, ST7735 and ST7781 LCD drivers (and compatible chipsets).

It is between 2.5x and 12x faster than the Adafruit libraries for SPI LCDs, but it aims to be completely "sketch compatible" (so you
can easily speed up your sketches that are using Adafruit's library).  You just need to change the name of the #include and "tft"
object.  A version of the Adafruit "benchmark" example is included (PDQ_graphicsbest.ino) with each driver library.

This includes the 1.8", 2.2" and 2.8" SPI TFT LCD boards or "touch shields" that are commonly available from Adafruit and
many other vendors.  These are commonly 128x128, 128x160 or 240x320 (but the library supports rotation).

I would also like to thank the excellent http://fastled.io/ project for creating the "FastPin.h" template header that allows
for full speed GPIO while allowing the use of "friendly" Arduino pin numbering (it is included in the PDQ driver libraries).

New features in latest commit ("v1.1.5" 2016-04-09) include:

 * Synced core functions with Adafruit_GFX (few changes - I think their clipping is still broken [fixed in PDQ_GFX])
 * Support for new fonts as seen in Adafruit_GFX GitHub
 * Minor bugfixes

New features in latest commit ("v1.0.0" 2015-05-30) include:

 * Arduino IDE 1.6.x support (mainly information in library.properties, but also tested and 100% warning free).
 * New ATtiny85 support for IL934x using USI SPI (not quite as fast as 328P - but can run 20MHz PLL to make up).
 * New support for parallel ST7781 driver as used in Seeed Studio 2.8" Touch Shield (also sold by Radio Shack).  This is the fastest LCD supported currently.
 * "Bit-banged" SPI support.  Not as fast, but can use (nearly) any pins.
 * Added pushColor with a count that can speed up application rendering runs of same color.
 * Tidied up files and made sure all drivers were updated with latest tweaks.
 
Suggestions, issues, bugs and comments welcome.  Via https://hackaday.io/Xark or visit #Arduino channel on Freenode.net IRC.
I have also posted a write-up about the development of this library at http://hackaday.io/Xark (describes most of the optimizations done).


Issues
------

Currently, the library may only be used from the INO file in your project. You _cannot_ include it in a header file and
use it from other CPP files. The current workaround is to write wrapper functions or classes, declare them in a header
file, and then implement them in the INO file.