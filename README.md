PDQ_GFX_Libs
============

An optimized fork of Adafruit's GFX library (and drivers) for Arduino (AVR).

This is a replacement "re-mix" of the Adafruit GFX library and associated hardware drivers.

Currently supported are ILI9340, ILI9341, ST7735 and ST7781 LCD drivers (and compatible chipsets).

It is between 3.5x and 14x faster than the Adafruit libraries for SPI LCDs, but it aims to be completely "sketch compatible" (so you
can easily speed up your sketches that are using Adafruit's library).  You just need to change the name of the #include and "tft"
object.  A version of the Adafruit "benchmark" example is included (PDQ_graphicsbest.ino) with each driver library.

This includes the 1.8", 2.2" and 2.8" SPI TFT LCD boards or "touch shields" that are commonly available from Adafruit and
many other vendors.  These are commonly 128x128, 128x160 or 240x320 (but the library supports rotation).

New features in latest commit (2015-05-30) include:

 * Arduino IDE 1.6.x support (mainly information in library.properties, but also tested and 100% warning free).
 * New ATtiny85 support for IL934x using USI SPI (not quite as fast as 328P - but can run 20MHz PLL to make up).
 * New support for parallel ST7781 driver as used in Seeed Studio 2.8" Touch Shield (also sold by Radio Shack).  This is the fastest LCD supported currently.
 * Added pushColor with a count that can speed up application rendering runs of same color.
 * Tidied up files and made sure all drivers were updated with latest tweaks.
 
Suggestions, issues, bugs and comments welcome.  Via https://hackaday.io/Xark or visit #Arduino channel on Freenode.net IRC.
