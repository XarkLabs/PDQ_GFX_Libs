#ifndef _PDQ_GFX_H
#define _PDQ_GFX_H

#include "Arduino.h"
#include "Print.h"

template <class HW>
class PDQ_GFX : public Print {

public:
	PDQ_GFX(int16_t w, int16_t h);	// Constructor (called by HW driver)

	// Graphic primitives
	// drawPixel MUST be defined by the driver subclass (and has no generic fall-back):
	
	// These are generic versions of routines for drivers that don't provide device-optimized code.
	// Drivers are required to have these functions (without "_" postfix), but can fall back to using
	// these if needed (they should not be called directly with "_" postfix or it will bypass any
	// device-optimized implementations).
	static void drawLine_(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
	static void drawFastVLine_(int16_t x, int16_t y, int16_t h, uint16_t color);
	static void drawFastHLine_(int16_t x, int16_t y, int16_t w, uint16_t color);
	static void fillRect_(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
	static void fillScreen_(uint16_t color);

	// These are usually overridden in the driver subclass to be useful (but not internally referenced)
	static void setRotation(uint8_t r);	// only swaps width/height if not supported by driver
	static void invertDisplay(boolean i);	// only if supported by driver
	
	// These exist in PDQ_GFX (and generally have no subclass override)
	static void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
	static void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
	static void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
	static void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
	static void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
	static void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
	static void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
	static void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,	int16_t radius, uint16_t color);
	static void fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,	int16_t radius, uint16_t color);
	static void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
	static void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
	static inline void setCursor(int16_t x, int16_t y);
	static inline void setTextColor(uint16_t c);
	static inline void setTextColor(uint16_t c, uint16_t bg);
	static inline void setTextSize(uint8_t s);
	static inline void setTextWrap(boolean w);

	static inline int16_t width(void) __attribute__ ((always_inline))		{ return _width; }
	static inline int16_t height(void) __attribute__ ((always_inline))		{ return _height; }
	static inline uint8_t getRotation(void) __attribute__ ((always_inline))		{ return rotation; }

	virtual size_t write(uint8_t);		// used by Arduino "Print.h" (and the one required virtual function)

	// swap any type
	template<typename T>
	static inline void swap(T& x, T& y)
	{
		T tmp = x;
		x = y;
		y = tmp;
	}

protected:
	static int16_t	WIDTH, HEIGHT;		// This is the 'raw' display w/h - never changes
	static int16_t	_width, _height;	// Display w/h as modified by current rotation
	static int16_t	cursor_x, cursor_y;
	static uint16_t	textcolor, textbgcolor;
	static uint8_t	textsize;
	static uint8_t	wrap;			// If set, 'wrap' text at right edge of display
	static uint8_t	rotation;
};

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

extern const unsigned char font[] PROGMEM;

template<class HW>
int16_t		PDQ_GFX<HW>::WIDTH;		// This is the 'raw' display w/h - never changes
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
uint16_t	PDQ_GFX<HW>::textcolor;
template<class HW>
uint16_t	PDQ_GFX<HW>::textbgcolor;
template<class HW>
uint8_t		PDQ_GFX<HW>::textsize;
template<class HW>
uint8_t		PDQ_GFX<HW>::rotation;
template<class HW>
uint8_t		PDQ_GFX<HW>::wrap;			// If set, 'wrap' text at right edge of display

template<class HW>
PDQ_GFX<HW>::PDQ_GFX(int16_t w, int16_t h)
{
	WIDTH		= w;
	HEIGHT		= h;
	_width		= w;
	_height		= h;
	rotation	= 0;
	cursor_x	= 0;
	cursor_y	= 0;
	textsize	= 1;
	textcolor	= 0xffff;
	textbgcolor	= 0xffff;
	wrap		= true;
}

// Draw a circle outline
template<class HW>
void PDQ_GFX<HW>::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	int16_t f	= 1 - r;
	int16_t ddF_x	= 1;
	int16_t ddF_y	= -2 * r;
	int16_t x	= 0;
	int16_t y	= r;

	HW::drawPixel(x0  , y0+r, color);
	HW::drawPixel(x0  , y0-r, color);
	HW::drawPixel(x0+r, y0  , color);
	HW::drawPixel(x0-r, y0  , color);

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
void PDQ_GFX<HW>::drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color)
{
	int16_t f	= 1 - r;
	int16_t ddF_x	= 1;
	int16_t ddF_y	= -2 * r;
	int16_t x	= 0;
	int16_t y	= r;

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
void PDQ_GFX<HW>::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	HW::drawFastVLine(x0, y0-r, 2*r+1, color);
	fillCircleHelper(x0, y0, r, 3, 0, color);
}

// Used to do circles and roundrects
template<class HW>
void PDQ_GFX<HW>::fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color)
{

	int16_t f	= 1 - r;
	int16_t ddF_x	= 1;
	int16_t ddF_y	= -2 * r;
	int16_t x	= 0;
	int16_t y	= r;

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
void PDQ_GFX<HW>::drawLine_(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
	int8_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep)
	{
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1)
	{
		swap(x0, x1);
		swap(y0, y1);
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

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

template<class HW>
void PDQ_GFX<HW>::drawFastVLine_(int16_t x, int16_t y, int16_t h, uint16_t color)
{
	// Used by driver when it has no special support
	HW::drawLine(x, y, x, y+h-1, color);
}

template<class HW>
void PDQ_GFX<HW>::drawFastHLine_(int16_t x, int16_t y, int16_t w, uint16_t color)
{
	// Used by driver when it has no special support
	HW::drawLine(x, y, x+w-1, y, color);
}

template<class HW>
void PDQ_GFX<HW>::fillRect_(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	// Used by driver when it has no special support
	for (int16_t i=x; i<x+w; i++)
	{
		HW::drawFastVLine(i, y, h, color);
	}
}

template<class HW>
void PDQ_GFX<HW>::fillScreen_(uint16_t color)
{
	// Used by driver when it has no special support
	HW::fillRect(0, 0, _width, _height, color);
}

// Draw a rectangle
template<class HW>
void PDQ_GFX<HW>::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	HW::drawFastHLine(x    ,  y    , w, color);
	HW::drawFastHLine(x    ,  y+h-1, w, color);
	HW::drawFastVLine(x    ,  y    , h, color);
	HW::drawFastVLine(x+w-1,  y    , h, color);
}

// Draw a rounded rectangle
template<class HW>
void PDQ_GFX<HW>::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
	// smarter version
	HW::drawFastHLine(x+r  , y    , w-2*r, color); // Top
	HW::drawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
	HW::drawFastVLine(x    , y+r  , h-2*r, color); // Left
	HW::drawFastVLine(x+w-1, y+r  , h-2*r, color); // Right
	// draw four corners
	drawCircleHelper(x+r	, y+r	 , r, 1, color);
	drawCircleHelper(x+w-r-1, y+r	 , r, 2, color);
	drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
	drawCircleHelper(x+r	, y+h-r-1, r, 8, color);
}

// Fill a rounded rectangle
template<class HW>
void PDQ_GFX<HW>::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
	// smarter version
	HW::fillRect(x+r, y, w-2*r, h, color);

	// draw four corners
	fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
	fillCircleHelper(x+r	, y+r, r, 2, h-2*r-1, color);
}

// Draw a triangle
template<class HW>
void PDQ_GFX<HW>::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
	HW::drawLine(x0, y0, x1, y1, color);
	HW::drawLine(x1, y1, x2, y2, color);
	HW::drawLine(x2, y2, x0, y0, color);
}

// Fill a triangle
template<class HW>
void PDQ_GFX<HW>::fillTriangle( int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
	int16_t a, b, y, last;

	// Sort coordinates by Y order (y2 >= y1 >= y0)
	if (y0 > y1)
	{
		swap(y0, y1); swap(x0, x1);
	}
	if (y1 > y2)
	{
		swap(y2, y1); swap(x2, x1);
	}
	if (y0 > y1)
	{
		swap(y0, y1); swap(x0, x1);
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

	int16_t	dx01 = x1 - x0;
	int16_t	dy01 = y1 - y0;
	int16_t	dx02 = x2 - x0;
	int16_t	dy02 = y2 - y0;
	int16_t	dx12 = x2 - x1;
	int16_t	dy12 = y2 - y1;
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
			swap(a,b);
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
			swap(a,b);
		HW::drawFastHLine(a, y, b-a+1, color);
	}
}

template<class HW>
void PDQ_GFX<HW>::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color)
{
	int16_t i, j, byteWidth = (w + 7) / 8;

	for (j = 0; j < h; j++)
	{
		for (i = 0; i < w; i++)
		{
			if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7)))
			{
				HW::drawPixel(x+i, y+j, color);
			}
		}
	}
}

template<class HW>
size_t PDQ_GFX<HW>::write(uint8_t c)
{
	if (c == '\n')
	{
		cursor_y += textsize*8;
		cursor_x = 0;
	}
	else if (c == '\r')
	{
		// skip em
	}
	else
	{
		drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
		cursor_x += textsize*6;
		if (wrap && (cursor_x > (_width - textsize*6)))
		{
			cursor_y += textsize*8;
			cursor_x = 0;
		}
	}
	return 1;
}

// Draw a character
template<class HW>
void PDQ_GFX<HW>::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size)
{
	if ((x >= _width)  ||
	    (y >= _height) ||
	    ((x + (6 * size) - 1) < 0) ||
	    ((y + (8 * size) - 1) < 0))
		return;

	uint8_t is_opaque = (bg != color);

	for (int8_t i=0; i<6; i++)
	{
		uint8_t line;

		if (i == 5)
			line = 0x0;
		else
			line = pgm_read_byte(font+(c*5)+i);

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

template<class HW>
void PDQ_GFX<HW>::setCursor(int16_t x, int16_t y)
{
	cursor_x = x;
	cursor_y = y;
}

template<class HW>
void PDQ_GFX<HW>::setTextSize(uint8_t s)
{
	textsize = (s > 0) ? s : 1;
}

template<class HW>
void PDQ_GFX<HW>::setTextColor(uint16_t c)
{
	// For 'transparent' background, we'll set the bg
	// to the same as fg instead of using a flag
	textcolor = c;
	textbgcolor = c;
}

template<class HW>
void PDQ_GFX<HW>::setTextColor(uint16_t c, uint16_t b)
{
	textcolor   = c;
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
		_width  = WIDTH;
		_height = HEIGHT;
		break;
	 case 1:
	 case 3:
		_width  = HEIGHT;
		_height = WIDTH;
		break;
	}
}

template<class HW>
void PDQ_GFX<HW>::invertDisplay(boolean i)
{
	// Used by driver when it has no special support
	// Do nothing, must be supported by driver
}

#endif // _PDQ_GFX_H
