PDQ_GFX_Libs
============

Optimized fork of Adafruit GFX library (and drivers) for Arduino.

This is a replacement "re-mix" of the Adafruit GFX library and hardware drivers for ILI9340, ILI9341 and S7735 LCD drivers.

It is between 3.5x and 12x faster than the Adafruit libraries, but it is completely "sketch compatible" (so you can easily speed
up your sketches that use Adafruit's library).  Just need to change the name of the #include and "tft" object.  A version of the
Adafruit "benchmark" example is included.

This includes the 1.8" and 2.2" SPI TFT LCD boards or "touch shields" that are commonly available from Adafruit and
many other vendors.
