// This is the PDQ re-mixed version of Adafruit's library from Xark
// here is the original copyright notice and license:

/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).	It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!

Copyright (c) 2013 Adafruit Industries.	All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

// This PDQ optimized version is by Xark
//
// Inspiration from Paul Stoffregen and the Teensy 3.1 community.
//
// GOALS:
//  1) Maintain "sketch" compatibility with original Adafruit libraries.
//  2) Be as much faster as is reasonably possible honoring goal 1. :-)
//  3) Be at least as small as Adafruit libraries.
//
// I believe all three of these have largely been achieved:
// 1) Near full compatibility.  Only minor initialization changes in original sketch.
// 2) Between ~2.5 and ~12 times faster (fillRect ~2.5x, drawLine ~12x).
// An average of ~4x faster over entire "graphictest.ino" benchmark.
//
// Even if this library is faster, it was based on the Adafruit original. 
// Adafruit deserves your support for making their library open-source (and
// for having some nice LCD modules and all kinds of other great parts too).
// Consider giving them your support if possible!

#ifndef _PDQ_GFX_H
#define _PDQ_GFX_H

#include "Arduino.h"
#include "Print.h"

#ifndef pgm_read_byte
	#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#ifndef pgm_read_word
	#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif
#ifndef pgm_read_dword
	#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#endif
#if !defined(__INT_MAX__) || (__INT_MAX__ > 0xFFFFL)
	#define pgm_read_pointer(addr) ((void *)pgm_read_dword(addr))
#else
	#define pgm_read_pointer(addr) ((void *)pgm_read_word(addr))
#endif

#include "gfxfont.h"

#define GFX_FONT_PACKED

typedef int			coord_t;	// type used for coordinates (signed) for parameters (int16_t used for storage)
typedef uint16_t	color_t;	// type used for colors (unsigned)

// swap any type
template<typename T>
static inline void swapValue(T& x, T& y)
{
	T tmp = x;
	x = y;
	y = tmp;
}

// minimum value for any type
template<typename T>
static inline T minValue(T& x, T& y)
{
	return x < y ? x : y;
}

// maximum value for any type
template<typename T>
static inline T maxValue(T& x, T& y)
{
	return x >= y ? x : y;
}

template <class HW>
class PDQ_GFX : public Print {

public:
	PDQ_GFX(coord_t w, coord_t h);	// Constructor (called by HW driver)

	// Graphic primitives
	// drawPixel MUST be defined by the driver subclass (and has no generic fall-back):

	// These are generic versions of routines for drivers that don't provide device-optimized code.
	// Drivers are required to have these functions (without "_" postfix), but can fall back to using
	// these if needed (they should not be called directly with "_" postfix or it will bypass any
	// device-optimized implementations).
	static void drawLine_(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color);
	static void drawFastVLine_(coord_t x, coord_t y, coord_t h, color_t color);
	static void drawFastHLine_(coord_t x, coord_t y, coord_t w, color_t color);
	static void fillRect_(coord_t x, coord_t y, coord_t w, coord_t h, color_t color);
	static void fillScreen_(color_t color);

	// These are usually overridden in the driver subclass to be useful (but not internally referenced)
	static void setRotation(uint8_t r);	// only swaps width/height if not supported by driver
	static void invertDisplay(boolean i);	// only if supported by driver

	// These exist in PDQ_GFX (and generally have no subclass override)
	static void drawRect(coord_t x, coord_t y, coord_t w, coord_t h, color_t color);
	static void drawCircle(coord_t x0, coord_t y0, coord_t r, color_t color);
	static void drawCircleHelper(coord_t x0, coord_t y0, coord_t r, uint8_t cornername, color_t color);
	static void fillCircle(coord_t x0, coord_t y0, coord_t r, color_t color);
	static void fillCircleHelper(coord_t x0, coord_t y0, coord_t r, uint8_t cornername, coord_t delta, color_t color);
	static void drawTriangle(coord_t x0, coord_t y0, coord_t x1, coord_t y1, coord_t x2, coord_t y2, color_t color);
	static void fillTriangle(coord_t x0, coord_t y0, coord_t x1, coord_t y1, coord_t x2, coord_t y2, color_t color);
	static void drawRoundRect(coord_t x0, coord_t y0, coord_t w, coord_t h, coord_t radius, color_t color);
	static void fillRoundRect(coord_t x0, coord_t y0, coord_t w, coord_t h, coord_t radius, color_t color);
	static void drawBitmap(coord_t x, coord_t y, const uint8_t *bitmap, coord_t w, coord_t h, color_t color);
	static void drawBitmap(coord_t x, coord_t y, const uint8_t *bitmap, coord_t w, coord_t h, color_t color, color_t bg);
	static void drawBitmap(coord_t x, coord_t y, uint8_t *bitmap, coord_t w, coord_t h, color_t color);
	static void drawBitmap(coord_t x, coord_t y, uint8_t *bitmap, coord_t w, coord_t h, color_t color, color_t bg);
	static void drawXBitmap(coord_t x, coord_t y, const uint8_t *bitmap, coord_t w, coord_t h, color_t color);
	static void drawChar(coord_t x, coord_t y, unsigned char c, color_t color, color_t bg, uint8_t size);
	static void drawCharGFX(coord_t x, coord_t y, unsigned char c, color_t color, color_t bg, uint8_t size);
	static inline void setCursor(coord_t x, coord_t y);
	static inline void setTextColor(color_t c);
	static inline void setTextColor(color_t c, color_t bg);
	static inline void setTextSize(uint8_t s);
	static inline void setTextWrap(boolean w);
	static inline void cp437(boolean x=true);
	static inline void setFont(const GFXfont *f = NULL);

	static inline coord_t width() __attribute__ ((always_inline))				{ return _width; }
	static inline coord_t height() __attribute__ ((always_inline))			{ return _height; }
	static inline uint8_t getRotation() __attribute__ ((always_inline))		{ return rotation; }
	static inline coord_t getCursorX() __attribute__ ((always_inline))		{ return cursor_x; }
	static inline coord_t getCursorY() __attribute__ ((always_inline))		{ return cursor_y; }
	static inline void getTextBounds(char *string, coord_t x, coord_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
	static inline void getTextBounds(const __FlashStringHelper *s, coord_t x, coord_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);

	virtual size_t write(uint8_t);		// used by Arduino "Print.h" (and the one required virtual function)

protected:
	static GFXfont*	gfxFont;
	static coord_t	WIDTH, HEIGHT;		// This is the 'raw' display w/h - never changes
	static coord_t	_width, _height;	// Display w/h as modified by current rotation
	static coord_t	cursor_x, cursor_y;
	static color_t	textcolor, textbgcolor;
	static uint8_t	textsize;
	static uint8_t	rotation;
	static boolean	wrap;				// If set, 'wrap' text at right edge of display
	static boolean	_cp437;				// If set, use correct CP437 charset (default is off)
};


template <class HW>
class PDQ_GFX_Button_
{
public:
	PDQ_GFX_Button_();
	void		initButton(PDQ_GFX<HW> *gfx, coord_t x, coord_t y, coord_t w, coord_t h, color_t outline, color_t fill, color_t textcolor, const char *label, uint8_t textsize);
	void		drawButton(boolean inverted = false);
	boolean		contains(coord_t x, coord_t y);

	void		press(boolean p);
	boolean		isPressed();
	boolean		justPressed();
	boolean		justReleased();

private:
	PDQ_GFX<HW>	*_gfx;
	int16_t		_x, _y;
	int16_t		_w, _h;
	uint8_t		_textsize;
	color_t		_outlinecolor, _fillcolor, _textcolor;
	char		_label[10];

	boolean		currstate, laststate;
};

// -----------------------------------------------
extern const unsigned char glcdfont[] PROGMEM;

template<class HW>
int16_t		PDQ_GFX<HW>::WIDTH;			// This is the 'raw' display w/h - never changes
template<class HW>
int16_t		PDQ_GFX<HW>::HEIGHT;
template<class HW>
int16_t		PDQ_GFX<HW>::_width;		// Display w/h as modified by current rotation
template<class HW>
int16_t		PDQ_GFX<HW>::_height;
template<class HW>
int16_t		PDQ_GFX<HW>::cursor_x;
template<class HW>
int16_t		PDQ_GFX<HW>::cursor_y;
template<class HW>
color_t		PDQ_GFX<HW>::textcolor;
template<class HW>
color_t		PDQ_GFX<HW>::textbgcolor;
template<class HW>
uint8_t		PDQ_GFX<HW>::textsize;
template<class HW>
uint8_t		PDQ_GFX<HW>::rotation;
template<class HW>
boolean		PDQ_GFX<HW>::wrap;			// If set, 'wrap' text at right edge of display
template<class HW>
boolean		PDQ_GFX<HW>::_cp437;		// If set, use correct CP437 charset (default is off)
template<class HW>
GFXfont		*PDQ_GFX<HW>::gfxFont;

template<class HW>
PDQ_GFX<HW>::PDQ_GFX(coord_t w, coord_t h)
{
	WIDTH		= (int16_t)w;
	HEIGHT		= (int16_t)h;
	_width		= (int16_t)w;
	_height		= (int16_t)h;
	cursor_x	= 0;
	cursor_y	= 0;
	rotation	= 0;
	textsize	= 1;
	textcolor	= 0xffff;
	textbgcolor	= 0xffff;
	wrap		= true;
	_cp437		= false;
	gfxFont		= NULL;
}

// Draw a circle outline
template<class HW>
void PDQ_GFX<HW>::drawCircle(coord_t x0, coord_t y0, coord_t r, color_t color)
{
	coord_t f		= 1 - r;
	coord_t ddF_x	= 1;
	coord_t ddF_y	= -2 * r;
	coord_t x		= 0;
	coord_t y		= r;

	HW::drawPixel(x0  , y0+r, color);
	HW::drawPixel(x0  , y0-r, color);
	HW::drawPixel(x0+r, y0	, color);
	HW::drawPixel(x0-r, y0	, color);

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		HW::drawPixel(x0 + x, y0 + y, color);
		HW::drawPixel(x0 - x, y0 + y, color);
		HW::drawPixel(x0 + x, y0 - y, color);
		HW::drawPixel(x0 - x, y0 - y, color);
		HW::drawPixel(x0 + y, y0 + x, color);
		HW::drawPixel(x0 - y, y0 + x, color);
		HW::drawPixel(x0 + y, y0 - x, color);
		HW::drawPixel(x0 - y, y0 - x, color);
	}
}

template<class HW>
void PDQ_GFX<HW>::drawCircleHelper( coord_t x0, coord_t y0, coord_t r, uint8_t cornername, color_t color)
{
	coord_t f	= 1 - r;
	coord_t ddF_x	= 1;
	coord_t ddF_y	= -2 * r;
	coord_t x	= 0;
	coord_t y	= r;

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		if (cornername & 0x4)
		{
			HW::drawPixel(x0 + x, y0 + y, color);
			HW::drawPixel(x0 + y, y0 + x, color);
		}
		if (cornername & 0x2)
		{
			HW::drawPixel(x0 + x, y0 - y, color);
			HW::drawPixel(x0 + y, y0 - x, color);
		}
		if (cornername & 0x8)
		{
			HW::drawPixel(x0 - y, y0 + x, color);
			HW::drawPixel(x0 - x, y0 + y, color);
		}
		if (cornername & 0x1)
		{
			HW::drawPixel(x0 - y, y0 - x, color);
			HW::drawPixel(x0 - x, y0 - y, color);
		}
	}
}

template<class HW>
void PDQ_GFX<HW>::fillCircle(coord_t x0, coord_t y0, coord_t r, color_t color)
{
	HW::drawFastVLine(x0, y0-r, 2*r+1, color);
	fillCircleHelper(x0, y0, r, 3, 0, color);
}

// Used to do circles and roundrects
template<class HW>
void PDQ_GFX<HW>::fillCircleHelper(coord_t x0, coord_t y0, coord_t r, uint8_t cornername, coord_t delta, color_t color)
{
	coord_t f	= 1 - r;
	coord_t ddF_x	= 1;
	coord_t ddF_y	= -2 * r;
	coord_t x	= 0;
	coord_t y	= r;

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		if (cornername & 0x1)
		{
			HW::drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
			HW::drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
		}
		if (cornername & 0x2)
		{
			HW::drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
			HW::drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
		}
	}
}

// Bresenham's algorithm - thx Wikipedia
template<class HW>
void PDQ_GFX<HW>::drawLine_(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color)
{
	int8_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep)
	{
		swapValue(x0, y0);
		swapValue(x1, y1);
	}

	if (x0 > x1)
	{
		swapValue(x0, x1);
		swapValue(y0, y1);
	}

	coord_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	coord_t err = dx / 2;
	coord_t ystep;

	if (y0 < y1)
	{
		ystep = 1;
	}
	else
	{
		ystep = -1;
	}

	for (; x0<=x1; x0++)
	{
		if (steep)
		{
			HW::drawPixel(y0, x0, color);
		}
		else
		{
			HW::drawPixel(x0, y0, color);
		}
		err -= dy;
		if (err < 0)
		{
			y0 += ystep;
			err += dx;
		}
	}
}

// Draw a rectangle
template<class HW>
void PDQ_GFX<HW>::drawRect(coord_t x, coord_t y, coord_t w, coord_t h, color_t color)
{
	HW::drawFastHLine(x	,  y	, w, color);
	HW::drawFastHLine(x	,  y+h-1, w, color);
	HW::drawFastVLine(x	,  y	, h, color);
	HW::drawFastVLine(x+w-1,  y	, h, color);
}

template<class HW>
void PDQ_GFX<HW>::drawFastVLine_(coord_t x, coord_t y, coord_t h, color_t color)
{
	// Used by driver when it has no special support
	HW::drawLine(x, y, x, y+h-1, color);
}

template<class HW>
void PDQ_GFX<HW>::drawFastHLine_(coord_t x, coord_t y, coord_t w, color_t color)
{
	// Used by driver when it has no special support
	HW::drawLine(x, y, x+w-1, y, color);
}

template<class HW>
void PDQ_GFX<HW>::fillRect_(coord_t x, coord_t y, coord_t w, coord_t h, color_t color)
{
	// Used by driver when it has no special support
	for (coord_t i=x; i<x+w; i++)
	{
		HW::drawFastVLine(i, y, h, color);
	}
}

template<class HW>
void PDQ_GFX<HW>::fillScreen_(color_t color)
{
	// Used by driver when it has no special support
	HW::fillRect(0, 0, _width, _height, color);
}

// Draw a rounded rectangle
template<class HW>
void PDQ_GFX<HW>::drawRoundRect(coord_t x, coord_t y, coord_t w, coord_t h, coord_t r, color_t color)
{
	// smarter version
	HW::drawFastHLine(x+r  , y	, w-2*r, color); // Top
	HW::drawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
	HW::drawFastVLine(x	, y+r  , h-2*r, color); // Left
	HW::drawFastVLine(x+w-1, y+r  , h-2*r, color); // Right
	// draw four corners
	drawCircleHelper(x+r	, y+r	, r, 1, color);
	drawCircleHelper(x+w-r-1, y+r	, r, 2, color);
	drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
	drawCircleHelper(x+r	, y+h-r-1, r, 8, color);
}

// Fill a rounded rectangle
template<class HW>
void PDQ_GFX<HW>::fillRoundRect(coord_t x, coord_t y, coord_t w, coord_t h, coord_t r, color_t color)
{
	// smarter version
	HW::fillRect(x+r, y, w-2*r, h, color);

	// draw four corners
	fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
	fillCircleHelper(x+r	, y+r, r, 2, h-2*r-1, color);
}

// Draw a triangle
template<class HW>
void PDQ_GFX<HW>::drawTriangle(coord_t x0, coord_t y0, coord_t x1, coord_t y1, coord_t x2, coord_t y2, color_t color)
{
	HW::drawLine(x0, y0, x1, y1, color);
	HW::drawLine(x1, y1, x2, y2, color);
	HW::drawLine(x2, y2, x0, y0, color);
}

// Fill a triangle
template<class HW>
void PDQ_GFX<HW>::fillTriangle( coord_t x0, coord_t y0, coord_t x1, coord_t y1, coord_t x2, coord_t y2, color_t color)
{
	coord_t a, b, y, last;

	// Sort coordinates by Y order (y2 >= y1 >= y0)
	if (y0 > y1)
	{
		swapValue(y0, y1);
		swapValue(x0, x1);
	}
	if (y1 > y2)
	{
		swapValue(y2, y1);
		swapValue(x2, x1);
	}
	if (y0 > y1)
	{
		swapValue(y0, y1);
		swapValue(x0, x1);
	}

	if (y0 == y2) // Handle awkward all-on-same-line case as its own thing
	{
		a = b = x0;
		if (x1 < a)
			a = x1;
		else if (x1 > b)
			b = x1;
		if (x2 < a)
			a = x2;
		else if (x2 > b)
			b = x2;
		HW::drawFastHLine(a, y0, b-a+1, color);
		return;
	}

	coord_t	dx01 = x1 - x0;
	coord_t	dy01 = y1 - y0;
	coord_t	dx02 = x2 - x0;
	coord_t	dy02 = y2 - y0;
	coord_t	dx12 = x2 - x1;
	coord_t	dy12 = y2 - y1;
	int32_t	sa = 0;
	int32_t	sb = 0;

	// For upper part of triangle, find scanline crossings for segments
	// 0-1 and 0-2.	If y1=y2 (flat-bottomed triangle), the scanline y1
	// is included here (and second loop will be skipped, avoiding a /0
	// error there), otherwise scanline y1 is skipped here and handled
	// in the second loop...which also avoids a /0 error here if y0=y1
	// (flat-topped triangle).
	if (y1 == y2)
		last = y1;	// Include y1 scanline
	else
		last = y1-1;	// Skip it

	for (y = y0; y <= last; y++)
	{
		a = x0 + sa / dy01;
		b = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;
		/* longhand:
		a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
		b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		*/
		if (a > b)
			swapValue(a, b);
		HW::drawFastHLine(a, y, b-a+1, color);
	}

	// For lower part of triangle, find scanline crossings for segments
	// 0-2 and 1-2.	This loop is skipped if y1=y2.
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for (; y <= y2; y++)
	{
		a = x1 + sa / dy12;
		b = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		/* longhand:
		a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
		b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		*/
		if (a > b)
			swapValue(a, b);
		HW::drawFastHLine(a, y, b-a+1, color);
	}
}

// Draw a 1-bit image (bitmap) at the specified (x, y) position from the
// provided bitmap buffer (must be PROGMEM memory) using the specified
// foreground color (unset bits are transparent).
template<class HW>
void PDQ_GFX<HW>::drawBitmap(coord_t x, coord_t y, const uint8_t *bitmap, coord_t w, coord_t h, color_t color)
{
	coord_t i, j, byteWidth = (w + 7) / 8;
	uint8_t byte;

	for (j = 0; j < h; j++)
	{
		for (i = 0; i < w; i++)
		{
			if (i % 8 == 0)
				byte = pgm_read_byte(bitmap + j * byteWidth + i / 8);
			else
				byte <<= 1;

			if (byte & 0x80)
				HW::drawPixel(x+i, y+j, color);
		}
	}
}

// Draw a 1-bit image (bitmap) at the specified (x, y) position from the
// provided bitmap buffer (must be PROGMEM memory) using the specified
// foreground (for set bits) and background (for clear bits) colors.
template<class HW>
void PDQ_GFX<HW>::drawBitmap(coord_t x, coord_t y, const uint8_t *bitmap, coord_t w, coord_t h, color_t color, color_t bg)
{
	coord_t i, j, byteWidth = (w + 7) / 8;
	uint8_t byte;

	for (j = 0; j < h; j++)
	{
		for (i = 0; i < w; i++)
		{
			if (i % 8 == 0)
				byte = pgm_read_byte(bitmap + j * byteWidth + i / 8);
			else
				byte <<= 1;

			if (byte & 0x80)
				HW::drawPixel(x+i, y+j, color);
			else
				HW::drawPixel(x+i, y+j, bg);
		}
	}
}

// drawBitmap() variant for RAM-resident (not PROGMEM) bitmaps.
template<class HW>
void PDQ_GFX<HW>::drawBitmap(coord_t x, coord_t y, uint8_t *bitmap, coord_t w, coord_t h, color_t color)
{
	coord_t i, j, byteWidth = (w + 7) / 8;
	uint8_t byte;

	for (j = 0; j < h; j++)
	{
		for (i = 0; i < w; i++)
		{
			if (i % 8 == 0)
				byte = bitmap[j * byteWidth + i / 8];
			else
				byte <<= 1;

			if (byte & 0x80)
				HW::drawPixel(x+i, y+j, color);
		}
	}
}

// drawBitmap() variant w/background for RAM-resident (not PROGMEM) bitmaps.
template<class HW>
void PDQ_GFX<HW>::drawBitmap(coord_t x, coord_t y, uint8_t *bitmap, coord_t w, coord_t h, color_t color, color_t bg)
{
	coord_t i, j, byteWidth = (w + 7) / 8;
	uint8_t byte;

	for (j = 0; j < h; j++)
	{
		for (i = 0; i < w; i++)
		{
			if (i % 8 == 0)
				byte = bitmap[j * byteWidth + i / 8];
			else
				byte <<= 1;

			if (byte & 0x80)
				HW::drawPixel(x+i, y+j, color);
			else
				HW::drawPixel(x+i, y+j, bg);
		}
	}
}

// Draw XBitMap Files (*.xbm), exported from GIMP,
// Usage: Export from GIMP to *.xbm, rename *.xbm to *.c and open in editor.
// C Array can be directly used with this function
template<class HW>
void PDQ_GFX<HW>::drawXBitmap(coord_t x, coord_t y, const uint8_t *bitmap, coord_t w, coord_t h, color_t color)
{
	coord_t i, j, byteWidth = (w + 7) / 8;
	uint8_t byte;

	for (j = 0; j < h; j++)
	{
		for (i = 0; i < w; i++)
		{
			if (i % 8 == 0)
				byte = pgm_read_byte(bitmap + j * byteWidth + i / 8);
			else
				byte >>= 1;

			if (byte & 0x01)
				HW::drawPixel(x+i, y+j, color);
		}
	}
}

template<class HW>
size_t PDQ_GFX<HW>::write(uint8_t c)
{
	// 'Classic' built-in font
	if (!gfxFont)
	{
		if (c == '\n')
		{
			cursor_x = 0;
			cursor_y += (coord_t)textsize*8;
		}
		else if (c != '\r')
		{
			HW::drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
			cursor_x += textsize*6;
			if (wrap && (cursor_x > (_width - textsize*6)))
			{
				cursor_x = 0;
				cursor_y += textsize*8;
			}
		}
	}
	else
	{
		if(c == '\n')
		{
			cursor_x	= 0;
			cursor_y += (coord_t)textsize * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
		}
		else if (c != '\r')
		{
			uint8_t first = pgm_read_byte(&gfxFont->first);
			if ((c >= first) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last)))
			{
				uint8_t	c2		= c - pgm_read_byte(&gfxFont->first);
				GFXglyph *glyph = &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c2]);
				uint8_t	w		= pgm_read_byte(&glyph->width);
				uint8_t	h		= pgm_read_byte(&glyph->height);
				// Is there an associated bitmap?
				if ((w > 0) && (h > 0))
				{
					coord_t xo = (int8_t)pgm_read_byte(&glyph->xOffset); // sic
					if(wrap && ((cursor_x + textsize * (xo + w)) >= _width))
					{
						// Drawing character would go off right edge; wrap to new line
						cursor_x	= 0;
						cursor_y += (coord_t)textsize *
						(uint8_t)pgm_read_byte(&gfxFont->yAdvance);
					}
					HW::drawCharGFX(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
				}
				cursor_x += pgm_read_byte(&glyph->xAdvance) * (coord_t)textsize;
			}
		}
	}
	return 1;
}

// Draw a character with built-in font
template<class HW>
void PDQ_GFX<HW>::drawChar(coord_t x, coord_t y, unsigned char c, color_t color, color_t bg, uint8_t size)
{
  if ((x >= _width)  ||
    (y >= _height) ||
    ((x + (6 * size) - 1) < 0) ||
    ((y + (8 * size) - 1) < 0))
    return;

  uint8_t is_opaque = (bg != color);

  if(!_cp437 && (c >= 176))	// Handle 'classic' charset behavior
    c++;

  for (int8_t i=0; i<6; i++)
  {
    uint8_t line;

    if (i == 5)
      line = 0x0;
    else
      line = pgm_read_byte(glcdfont+(c*5)+i);

    if (size == 1)
    {
      for (int8_t j = 0; j < 8; j++)
      {
        if (line & 0x1)
        {
          HW::drawPixel(x+i, y+j, color);
        }
        else if (is_opaque)
        {
          HW::drawPixel(x+i, y+j, bg);
        }
        line >>= 1;
      }
    }
    else
    {
      for (int8_t j = 0; j < 8; j++)
      {
        if (line & 0x1)
        {
          HW::fillRect(x+(i*size), y+(j*size), size, size, color);
        }
        else if (is_opaque)
        {
          HW::fillRect(x+(i*size), y+(j*size), size, size, bg);
        }
        line >>= 1;
      }
    }
  }
}

// Draw a character with GFX font
template<class HW>
void PDQ_GFX<HW>::drawCharGFX(coord_t x, coord_t y, unsigned char c, color_t color, color_t bg, uint8_t size)
{
  // Character is assumed previously filtered by write() to eliminate
  // newlines, returns, non-printable characters, etc.	Calling drawChar()
  // directly with 'bad' characters of font may cause mayhem!

  c -= pgm_read_byte(&gfxFont->first);
  GFXglyph *glyph	= &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c]);
  uint8_t	*bitmap = (uint8_t *)pgm_read_pointer(&gfxFont->bitmap);

  uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
  uint8_t	w	= pgm_read_byte(&glyph->width);
  uint8_t	h	= pgm_read_byte(&glyph->height);
  // uint8_t	xa	= pgm_read_byte(&glyph->xAdvance);
  int8_t	xo	= pgm_read_byte(&glyph->xOffset);
  int8_t	yo	= pgm_read_byte(&glyph->yOffset);

  // Todo: Add character clipping here

  // NOTE: THERE IS NO 'BACKGROUND' COLOR OPTION ON CUSTOM FONTS.
  // THIS IS ON PURPOSE AND BY DESIGN.  The background color feature
  // has typically been used with the 'classic' font to overwrite old
  // screen contents with new data.  This ONLY works because the
  // characters are a uniform size; it's not a sensible thing to do with
  // proportionally-spaced fonts with glyphs of varying sizes (and that
  // may overlap).  To replace previously-drawn text when using a custom
  // font, use the getTextBounds() function to determine the smallest
  // rectangle encompassing a string, erase the area with fillRect(),
  // then draw new text.  This WILL infortunately 'blink' the text, but
  // is unavoidable.  Drawing 'background' pixels will NOT fix this,
  // only creates a new set of problems.  Have an idea to work around
  // this (a canvas object type for MCUs that can afford the RAM and
  // displays supporting setAddrWindow() and pushColors()), but haven't
  // implemented this yet.

  if (bo & 0x8000) {
    // packed font
    uint8_t	bits_cnt = 0;
    uint8_t	bits;
    uint8_t	cnt,cnt2;
    bo &= 0x7FFF;
    coord_t _y = y+yo;
    for (coord_t yy=0; yy<h; yy++, _y++)
    {
      coord_t _x = x+xo;
      for (coord_t xx=0; xx<w; xx+=cnt2)
      {

        if (bits_cnt == 0)
        {
          bits = pgm_read_byte(&bitmap[bo++]);
          bits_cnt = 2;
          cnt = (bits & 0x7)+1;
        }

        if (xx+cnt >= w)
        {
            cnt2 = w-xx;
        } else {
            cnt2 = cnt;
        }

        if (bits & 0x8)
        {
          if (size == 1)
          {
            HW::drawFastHLine(_x, _y, cnt2, color);
            _x += cnt2;
          }
          else
          {
            HW::fillRect(x+xo+xx*size, y+yo+yy*size, cnt2*size, size, color);
            // _x not used if size > 1, so not need to increment it
          }
        } else {
          _x += cnt2;
        }

        cnt -= cnt2;
        if (cnt == 0) {
          bits >>= 4;
          cnt = (bits & 0x7)+1;
          bits_cnt--;
        }
      }
    }
  } else {
    uint8_t	bit = 0;
    uint8_t	bits = 0;
    if (size == 1)
    {
      coord_t _y = y+yo;
      for (coord_t yy=0; yy<h; yy++, _y++)
      {
        coord_t _x = x+xo;
        for (coord_t xx=0; xx<w; xx++, _x++)
        {
          if (bit == 0)
          {
            bits = pgm_read_byte(&bitmap[bo++]);
          }
          bit = (bit+1) & 0x7;

          if (bits & 0x80)
          {
            HW::drawPixel(_x, _y, color);
          }
          bits <<= 1;
        }
      }
    } else {
      for (coord_t yy=0; yy<h; yy++)
      {
        for (coord_t xx=0; xx<w; xx++)
        {
          if (bit == 0)
          {
            bits = pgm_read_byte(&bitmap[bo++]);
          }
          bit = (bit+1) & 0x7;

          if (bits & 0x80)
          {
            HW::fillRect(x+xo+xx*size, y+yo+yy*size, size, size, color);
          }
          bits <<= 1;
        }
      }
    }
  }
}

template<class HW>
void PDQ_GFX<HW>::setCursor(coord_t x, coord_t y)
{
	cursor_x = (int16_t)x;
	cursor_y = (int16_t)y;
}

template<class HW>
void PDQ_GFX<HW>::setTextSize(uint8_t s)
{
	textsize = (s > 0) ? s : 1;
}

template<class HW>
void PDQ_GFX<HW>::setTextColor(color_t c)
{
	// For 'transparent' background, we'll set the bg
	// to the same as fg instead of using a flag
	textcolor = c;
	textbgcolor = c;
}

template<class HW>
void PDQ_GFX<HW>::setTextColor(color_t c, color_t b)
{
	textcolor	= c;
	textbgcolor = b;
}

template<class HW>
void PDQ_GFX<HW>::setTextWrap(boolean w)
{
	wrap = w;
}

template<class HW>
void PDQ_GFX<HW>::setRotation(uint8_t x)
{
	// Used by driver when it has no special support
	rotation = x & 3;
	switch(rotation) {
	case 0:
	case 2:
		_width	= WIDTH;
		_height = HEIGHT;
		break;
	case 1:
	case 3:
		_width	= HEIGHT;
		_height = WIDTH;
		break;
	}
}

template<class HW>
void PDQ_GFX<HW>::cp437(boolean x)
{
	_cp437 = x;
}

template<class HW>
void PDQ_GFX<HW>::setFont(const GFXfont *f)
{
	if (f)		// Font struct pointer passed in?
	{
		if (!gfxFont) // And no current font struct?
		{
			// Switching from classic to new font behavior.
			// Move cursor pos down 6 pixels so it's on baseline.
			cursor_y += 6;
		}
	}
	else if (gfxFont) // NULL passed.	Current font struct defined?
	{
		// Switching from new to classic font behavior.
		// Move cursor pos up 6 pixels so it's at top-left of char.
		cursor_y -= 6;
	}
	gfxFont = (GFXfont *)f;
}

// Pass string and a cursor position, returns UL corner and W,H.
template<class HW>
void PDQ_GFX<HW>::getTextBounds(char *str, coord_t x, coord_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h)
{
	uint8_t c; // Current character

	*x1 = x;
	*y1 = y;
	*w	= *h = 0;

	if (gfxFont)
	{
		GFXglyph *glyph;
		uint8_t	first	= pgm_read_byte(&gfxFont->first);
		uint8_t	last	= pgm_read_byte(&gfxFont->last);
		uint8_t	gw, gh, xa;
		int8_t	xo, yo;
		int16_t	minx = _width, miny = _height, maxx = -1, maxy = -1;
		coord_t	gx1, gy1, gx2, gy2;
		coord_t	ts = (coord_t)textsize, ya = (coord_t)textsize * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);

		while((c = *str++))
		{
			if (c != '\n')	// Not a newline
			{
				if (c != '\r')	// Not a carriage return, is normal char
				{
					if ((c >= first) && (c <= last))	// Char present in current font
					{
						c		-= first;
						glyph = &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c]);
						gw		= pgm_read_byte(&glyph->width);
						gh		= pgm_read_byte(&glyph->height);
						xa		= pgm_read_byte(&glyph->xAdvance);
						xo		= pgm_read_byte(&glyph->xOffset);
						yo		= pgm_read_byte(&glyph->yOffset);
						if (wrap && ((x + (((int16_t)xo + gw) * ts)) >= _width)) // Line wrap
						{
							x = 0;	// Reset x to 0
							y += ya; // Advance y by 1 line
						}
						gx1 = x	+ xo * ts;
						gy1 = y	+ yo * ts;
						gx2 = gx1 + gw * ts - 1;
						gy2 = gy1 + gh * ts - 1;
						if (gx1 < minx)
							minx = gx1;
						if (gy1 < miny)
							miny = gy1;
						if (gx2 > maxx)
							maxx = gx2;
						if (gy2 > maxy)
							maxy = gy2;
						x += xa * ts;
					}
				} // Carriage return = do nothing
			}
			else	// Newline
			{
				x	= 0;	// Reset x
				y += ya; // Advance y by 1 line
			}
		}
		// End of string
		*x1 = minx;
		*y1 = miny;
		if (maxx >= minx)
			*w	= maxx - minx + 1;
		if (maxy >= miny)
			*h	= maxy - miny + 1;

	}
	else	// Default font
	{
		uint16_t lineWidth = 0, maxWidth = 0; // Width of current, all lines

		while((c = *str++))
		{
			if (c != '\n') // Not a newline
			{
				if (c != '\r') // Not a carriage return, is normal char
				{
					lineWidth += textsize * 6; // Includes interchar x gap
					if (wrap && (cursor_x > (_width - textsize*6)))
					{
						x = 0;
						y += textsize*8;

						if (lineWidth > maxWidth)	// Save widest line
							maxWidth = lineWidth;
						lineWidth	= textsize * 6; // First char on new line
					}
				} // Carriage return = do nothing
			}
			else // Newline
			{
				x	= 0;						// Reset x to 0
				y += textsize * 8; 				// Advance y by 1 line
				if (lineWidth > maxWidth)		// Save widest line
					maxWidth = lineWidth;
				lineWidth = 0;					// Reset lineWidth for new line
			}
		}
		// End of string
		if (lineWidth) 							// Add height of last (or only) line
			y += textsize * 8;
		*w = maxWidth - 1;						// Don't include last interchar x gap
		*h = y - *y1;

	} // End classic vs custom font
}

// Same as above, but for PROGMEM strings
template<class HW>
void PDQ_GFX<HW>::getTextBounds(const __FlashStringHelper *str, coord_t x, coord_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h)
{
	uint8_t *s = (uint8_t *)str;
	uint8_t c;

	*x1 = x;
	*y1 = y;
	*w	= *h = 0;

	if (gfxFont)
	{
		GFXglyph *glyph;
		uint8_t	first	= pgm_read_byte(&gfxFont->first);
		uint8_t	last	= pgm_read_byte(&gfxFont->last);
		uint8_t			gw, gh, xa;
		int8_t			xo, yo;
		int16_t	minx = _width, miny = _height, maxx = -1, maxy = -1;
		coord_t			gx1, gy1, gx2, gy2;
		coord_t			ts = (coord_t)textsize;
		coord_t			ya = ts * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);

		while((c = pgm_read_byte(s++)))
		{
			if (c != '\n') // Not a newline
			{
				if (c != '\r') // Not a carriage return, is normal char
				{
					if ((c >= first) && (c <= last)) // Char present in current font
					{
						c		-= first;
						glyph = &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c]);
						gw		= pgm_read_byte(&glyph->width);
						gh		= pgm_read_byte(&glyph->height);
						xa		= pgm_read_byte(&glyph->xAdvance);
						xo		= pgm_read_byte(&glyph->xOffset);
						yo		= pgm_read_byte(&glyph->yOffset);
						if (wrap && ((x + (((int16_t)xo + gw) * ts)) >= _width)) // Line wrap
						{
							x	= 0;	// Reset x to 0
							y += ya;	// Advance y by 1 line
						}
						gx1 = x	+ xo * ts;
						gy1 = y	+ yo * ts;
						gx2 = gx1 + gw * ts - 1;
						gy2 = gy1 + gh * ts - 1;
						if (gx1 < minx)
							minx = gx1;
						if (gy1 < miny)
							miny = gy1;
						if (gx2 > maxx)
							maxx = gx2;
						if (gy2 > maxy)
							maxy = gy2;
						x += xa * ts;
					}
				} // Carriage return = do nothing
			}
			else // Newline
			{
				x	= 0;	// Reset x
				y += ya;	// Advance y by 1 line
			}
		}
		// End of string
		*x1 = minx;
		*y1 = miny;
		if (maxx >= minx)
			*w	= maxx - minx + 1;
		if (maxy >= miny)
			*h	= maxy - miny + 1;

	}
	else	 // Default font
	{
		uint16_t lineWidth = 0, maxWidth = 0; // Width of current, all lines

		while((c = pgm_read_byte(s++)))
		{
			if (c != '\n') // Not a newline
			{
				if (c != '\r') // Not a carriage return, is normal char
				{
					if (wrap && ((x + textsize * 6) >= _width))
					{
						x = 0;						// Reset x to 0
						y += textsize * 8;			// Advance y by 1 line
						if (lineWidth > maxWidth)	// Save widest line
							maxWidth = lineWidth;
						lineWidth	= textsize * 6; // First char on new line
					}
					else // No line wrap, just keep incrementing X
					{
						lineWidth += textsize * 6; // Includes interchar x gap
					}
				} // Carriage return = do nothing
			}
			else // Newline
			{
				x = 0;								// Reset x to 0
				y += textsize * 8;					// Advance y by 1 line
				if (lineWidth > maxWidth)			// Save widest line
					maxWidth = lineWidth;
				lineWidth = 0;						// Reset lineWidth for new line
			}
		}
		// End of string
		if (lineWidth) 								// Add height of last (or only) line
			y += textsize * 8;
		*w = maxWidth - 1;							// Don't include last interchar x gap
		*h = y - *y1;

	} // End classic vs custom font
}

template<class HW>
void PDQ_GFX<HW>::invertDisplay(boolean i)
{
	// Used by driver when it has no special support
	// Do nothing, must be supported by driver
}

/***************************************************************************/
// code for the GFX button UI element

template <class HW>
PDQ_GFX_Button_<HW>::PDQ_GFX_Button_()
{
	_gfx = 0;
}

template <class HW>
void PDQ_GFX_Button_<HW>::initButton(PDQ_GFX<HW> *gfx, coord_t x, coord_t y, coord_t w, coord_t h, color_t outline, color_t fill, color_t textcolor, const char *label, uint8_t textsize)
{
	_gfx			= gfx;
	_x				= x;
	_y				= y;
	_w				= w;
	_h				= h;
	_outlinecolor	= outline;
	_fillcolor		= fill;
	_textcolor		= textcolor;
	_textsize		= textsize;
	strncpy(_label, label, 9);
	_label[9] = 0;
}

template <class HW>
void PDQ_GFX_Button_<HW>::drawButton(boolean inverted)
{
	uint16_t fill, outline, text;

	if (!inverted)
	{
		fill	= _fillcolor;
		outline = _outlinecolor;
		text	= _textcolor;
	}
	else
	{
		fill	= _textcolor;
		outline = _outlinecolor;
		text	= _fillcolor;
	}

	_gfx->fillRoundRect(_x - (_w/2), _y - (_h/2), _w, _h, min(_w,_h)/4, fill);
	_gfx->drawRoundRect(_x - (_w/2), _y - (_h/2), _w, _h, min(_w,_h)/4, outline);

	_gfx->setCursor(_x - strlen(_label)*3*_textsize, _y-4*_textsize);
	_gfx->setTextColor(text);
	_gfx->setTextSize(_textsize);
	_gfx->print(_label);
}

template <class HW>
boolean PDQ_GFX_Button_<HW>::contains(coord_t x, coord_t y)
{
	if ((x < (_x - _w/2)) || (x > (_x + _w/2)))
		return false;
	if ((y < (_y - _h/2)) || (y > (_y + _h/2)))
		return false;
	return true;
}

template <class HW>
void PDQ_GFX_Button_<HW>::press(boolean p)
{
	laststate = currstate;
	currstate = p;
}

template <class HW>
boolean PDQ_GFX_Button_<HW>::isPressed()
{
	return currstate;
}

template <class HW>
boolean PDQ_GFX_Button_<HW>::justPressed()
{
	return (currstate && !laststate);
}

template <class HW>
boolean PDQ_GFX_Button_<HW>::justReleased()
{
	return (!currstate && laststate);
}

#endif // _PDQ_GFX_H
