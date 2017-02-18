// This is the PDQ re-mixed version of Adafruit's library
// here is the original copyright notice:

/***************************************************
This is a library for the Adafruit 2.8" TFT display.
This library works with the Adafruit 2.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/335
as well as Adafruit TFT Touch Shield
  ----> http://www.adafruit.com/products/376
 
Check out the links above for our tutorials and wiring diagrams.
These displays use 8-bit parallel to communicate, 12 or 13 pins are required
to interface (RST is optional).
Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.
MIT license, all text above must be included in any redistribution

To download. click the DOWNLOADS button in the top right corner, rename the uncompressed folder Adafruit_TFTLCD. Check that the Adafruit_TFTLCD folder contains Adafruit_TFTLCD.cpp and Adafruit_TFTLCD.

Place the Adafruit_TFT library folder your <arduinosketchfolder>/libraries/ folder. You may need to create the libraries subfolder if its your first library. Restart the IDE

Also requires the Adafruit_GFX library for Arduino. https://github.com/adafruit/Adafruit-GFX-Library
 ****************************************************/

// Graphics library by ladyada/adafruit with init code from Rossum
// MIT license

//===============================================================
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
// 2) Significantly faster than original (can't easily test as differs per board type).
//
// Even if this library is faster, it was based on the Adafruit original. 
// Adafruit deserves your support for making their library open-source (and
// for having some nice LCD modules and all kinds of other great parts too).
// Consider giving them your support if possible!

#if !defined(_PDQ_ST7781H_)
#define _PDQ_ST7781H_

#include "Arduino.h"
#include "Print.h"

#include <PDQ_GFX.h>

#include <avr/pgmspace.h>

#if defined(USE_PORT_ACCESS)
#if !defined(PORT_DATALO6)
#error Oops!  You need to #include "PDQ_ST7781_config.h" (modified with your pin configuration and options) from your sketch before #include "PDQ_ST7781.h".
#endif
#else
#include <PDQ_FastPin.h>	// for pin access
#if !defined(LCD_D0)
#error Oops!  You need to #include "PDQ_ST7781_config.h" (modified with your pin configuration and options) from your sketch before #include "PDQ_ST7781.h".
#endif
#endif

#if !defined(__AVR_ATtiny85__) && !defined(__AVR_ATtiny45__)
#define INLINE		inline
#define INLINE_OPT	__attribute__((always_inline))
#else
#define INLINE
#define INLINE_OPT
#endif

// Color definitions
enum
{
	ST7781_BLACK	= 0x0000,
	ST7781_BLUE		= 0x001F,
	ST7781_RED		= 0xF800,
	ST7781_GREEN	= 0x07E0,
	ST7781_CYAN		= 0x07FF,
	ST7781_MAGENTA	= 0xF81F,
	ST7781_YELLOW	= 0xFFE0,	
	ST7781_WHITE	= 0xFFFF,
};

class PDQ_ST7781 : public PDQ_GFX<PDQ_ST7781>
{
 public:
	// ST7781 commands
	// For datasheet see http://garden.seeedstudio.com/images/4/4e/ST7781R_datasheet.pdf
 	enum
	{
		ST7781_DRIVERRD	= 0x00,
		ST7781_DRIVROPC	= 0x01,
		ST7781_DRIVRWVC	= 0x02,
		ST7781_ENTERMOD	= 0x03,
		ST7781_RESIZECN	= 0x04,
		ST7781_DISPCON1	= 0x07,
		ST7781_DISPCON2	= 0x08,
		ST7781_DISPCON3	= 0x09,
		ST7781_DISPCON4	= 0x0a,
		ST7781_RGBIFCN1	= 0x0c,
		ST7781_FRAMMKRP	= 0x0d,
		ST7781_RGBIFCN2	= 0x0f,
		ST7781_POWERCN1	= 0x10,
		ST7781_HORZADDR	= 0x20,
		ST7781_VERTADDR	= 0x21,
		ST7781_WR_GRAM	= 0x22,
		ST7781_RD_GRAM	= 0x22,
		ST7781_GAMMA_01	= 0x30,
		ST7781_GAMMA_02	= 0x31,
		ST7781_GAMMA_03	= 0x32,
		ST7781_GAMMA_04	= 0x35,
		ST7781_GAMMA_05	= 0x36,
		ST7781_GAMMA_06	= 0x37,
		ST7781_GAMMA_07	= 0x38,
		ST7781_GAMMA_08	= 0x39,
		ST7781_GAMMA_09	= 0x3c,
		ST7781_GAMMA_10	= 0x3d,
		ST7781_HSTRTADR	= 0x50,
		ST7781_HENDADR	= 0x51,
		ST7781_VSTRTADR	= 0x52,
		ST7781_VENDADR	= 0x53,
		ST7781_DRIVRSC1	= 0x60,
		ST7781_BASEIMGC	= 0x61,
		ST7781_VSCROLLC	= 0x6a,
		ST7781_PIMG1POS	= 0x80,
		ST7781_PIMG1BEG	= 0x81,
		ST7781_PIMG1END	= 0x82,
		ST7781_PIMG2POS	= 0x83,
		ST7781_PIMG2BEG	= 0x84,
		ST7781_PIMG2END	= 0x85,
		ST7781_PANICTL1	= 0x90,
		ST7781_PANICTL2	= 0x95,
		ST7781_POWERCN2	= 0xb0,
		ST7781_POWERCN3	= 0xb1,
		ST7781_POWERCN4	= 0xb2,
		ST7781_POWERCN5	= 0xb3,
		ST7781_POWERCN6	= 0xb5,
		ST7781_POWERCN7	= 0xb6,
		ST7781_NVMIDCOD	= 0xd2,
		ST7781_NVMCTLST	= 0xd9,
		ST7781_NMMWRCMD	= 0xdf,
		ST7781_NVMENABL	= 0xfa,
		ST7781_NVMVCOMO	= 0xfe,
		ST7781_NVMCMDEN	= 0xff,
	};

	// some other misc. constants
	enum
	{
		// screen dimensions
		ST7781_TFTWIDTH		= 240,
		ST7781_TFTHEIGHT	= 320,

		// delay indicator bit for commandList()
		DELAY			= 0xaf	// unused ST7781 command
	};
	
	struct reg_cmd { uint8_t r; uint16_t v; };

	// higher-level routines
	PDQ_ST7781();
	static void inline begin(void);
	static void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
	static void pushColor(uint16_t color);
	static void pushColor(uint16_t color, int count);

	// Pass 8-bit (each) R,G,B, get back 16-bit packed color
	static INLINE uint16_t color565(uint8_t r, uint8_t g, uint8_t b)
	{
		return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
	}
	static INLINE uint16_t Color565(uint8_t r, uint8_t g, uint8_t b)	// older inconsistent name for compatibility
	{
		return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
	}
	
	// required driver primitive methods (all except drawPixel can call generic version in PDQ_GFX with "_" postfix).
	static void drawPixel(int x, int y, uint16_t color);
	static void drawFastVLine(int x, int y, int h, uint16_t color);
	static void drawFastHLine(int x, int y, int w, uint16_t color);
	static void setRotation(uint8_t r);
	static void invertDisplay(boolean i);

	static inline void fillScreen(uint16_t color) __attribute__((always_inline))
	{
		fillScreen_(color);			// call generic version
	}

	static void drawLine(int x0, int y0, int x1, int y1, uint16_t color);
	static void fillRect(int x, int y, int w, int h, uint16_t color);

	// === lower-level internal routines =========
	static void commandList(const struct reg_cmd *cmd, uint8_t numCommands);

	// set CS back to low (LCD selected)
	static inline void lcd_begin() __attribute__((always_inline))
	{
#if defined(USE_PORT_ACCESS)
		PORT_DATAHI2 &= ~CS_BIT;		// CS <= LOW (selected)
#else
	FastPin<LCD_CS>::lo();				// CS <= LOW (selected)
#endif
	}

	// reset CS back to high (LCD unselected)
	static inline void lcd_end() __attribute__((always_inline))
	{
#if defined(USE_PORT_ACCESS)
		PORT_DATAHI2 |= CS_BIT;		// CS <= HIGH (deselected)
#else
	FastPin<LCD_CS>::hi();			// CS <= HIGH (deselected)
#endif
	}
	
#if !defined(USE_PORT_ACCESS)
	static void writeByte(uint8_t byte) __attribute__ ((noinline))	// too big to inline
	{
		FastPin<LCD_WR>::lo();

		FastPin<LCD_D7>::lo();
		FastPin<LCD_D6>::lo();
		FastPin<LCD_D5>::lo();
		FastPin<LCD_D4>::lo();
		FastPin<LCD_D3>::lo();
		FastPin<LCD_D2>::lo();
		FastPin<LCD_D1>::lo();
		FastPin<LCD_D0>::lo();

		if (byte & (1<< 7)) { FastPin<LCD_D7>::hi(); }
		if (byte & (1<< 6)) { FastPin<LCD_D6>::hi(); }
		if (byte & (1<< 5)) { FastPin<LCD_D5>::hi(); }
		if (byte & (1<< 4)) { FastPin<LCD_D4>::hi(); }
		if (byte & (1<< 3)) { FastPin<LCD_D3>::hi(); }
		if (byte & (1<< 2)) { FastPin<LCD_D2>::hi(); }
		if (byte & (1<< 1)) { FastPin<LCD_D1>::hi(); }
		if (byte & (1<< 0)) { FastPin<LCD_D0>::hi(); }

		FastPin<LCD_WR>::hi();
	}
#endif

	// write 16-bit data
	static INLINE void writeData(uint16_t data) INLINE_OPT
	{
#if defined(USE_PORT_ACCESS)
		PORT_DATAHI2 = RD_BIT | /*WR_BIT |*/ RS_BIT | /*CS_BIT |*/ ((((uint8_t)(data>>8))>>6)&DATAHI2_MASK);
		PORT_DATALO6 = (((uint8_t)(data>>8))<<2);
		PORT_DATAHI2 |= WR_BIT;
		PORT_DATAHI2 = RD_BIT | /*WR_BIT |*/ RS_BIT | /*CS_BIT |*/ ((((uint8_t)data)>>6)&DATAHI2_MASK);
		PORT_DATALO6 = (((uint8_t)data)<<2);
		PORT_DATAHI2 |= WR_BIT;
#else
		writeByte(data>>8);
		writeByte(data);
#endif
	} 

	// repeat 16-bit data
	static INLINE void writeData(uint16_t data, int count)
	{
		while (count-- > 0)
		{
#if defined(USE_PORT_ACCESS)
			PORT_DATAHI2 = RD_BIT | /*WR_BIT |*/ RS_BIT | /*CS_BIT |*/ ((((uint8_t)(data>>8))>>6)&DATAHI2_MASK);
			PORT_DATALO6 = (((uint8_t)(data>>8))<<2);
			PORT_DATAHI2 |= WR_BIT;
			PORT_DATAHI2 = RD_BIT | /*WR_BIT |*/ RS_BIT | /*CS_BIT |*/ ((((uint8_t)data)>>6)&DATAHI2_MASK);
			PORT_DATALO6 = (((uint8_t)data)<<2);
			PORT_DATAHI2 |= WR_BIT;
#else
			writeData(data);
#endif
		}
	} 

	// write command byte
	static INLINE void writeCommand(uint8_t command) INLINE_OPT
	{
#if defined(USE_PORT_ACCESS)
		PORT_DATAHI2 = RD_BIT /*| WR_BIT | RS_BIT | CS_BIT |*/ | 0;
		PORT_DATALO6 = 0;
		PORT_DATAHI2 |= WR_BIT;
		PORT_DATAHI2 = RD_BIT /*| WR_BIT | RS_BIT | CS_BIT |*/ | (command>>6);
		PORT_DATALO6 = (command<<2);
		PORT_DATAHI2 = RD_BIT | WR_BIT | RS_BIT /*| CS_BIT*/ | (command>>6);
#else
		FastPin<LCD_RS>::lo();
		writeData(command);
		FastPin<LCD_RS>::hi();
#endif
	} 

	// write command byte followed by 16-bit data
	static INLINE void writeRegister(uint8_t reg, uint16_t data) INLINE_OPT
	{
		writeCommand(reg);
		writeData(data);
	}

	// internal version that does not lcd_begin()/lcd_end()
	static INLINE void setAddrWindow_(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) INLINE_OPT
	{
		int	x, y;
#if defined(FIXED_ROTATION)
		switch (FIXED_ROTATION)
#else
		switch (rotation)
#endif
		{
		default:
		case 0:
			x = x0;
			y = y0;
			break;
		case 1:
			x = y0;
			y0 = x0;
			x0 = ST7781_TFTWIDTH-1-y1;
			y1 = x1;
			x1 = ST7781_TFTWIDTH-1-x;
			x = x1;
			y = y0;
			break;
		case 2:
			x = x0;
			x0 = ST7781_TFTWIDTH-1-x1;
			x1 = ST7781_TFTWIDTH-1-x;
			x = y0;
			y0 = ST7781_TFTHEIGHT-1-y1;
			y1 = ST7781_TFTHEIGHT-1-x;
			x = x1;
			y = y1;
			break;
		case 3:
			x = x0;
			x0 = y0;
			y0 = ST7781_TFTHEIGHT-1-x1;
			x1 = y1;
			y1 = ST7781_TFTHEIGHT-1-x;
			x = x0;
			y = y1;
			break;
		}
		
		writeRegister(ST7781_HSTRTADR, x0);
		writeRegister(ST7781_HENDADR,  x1);
		writeRegister(ST7781_VSTRTADR, y0);
		writeRegister(ST7781_VENDADR,  y1);
		writeRegister(ST7781_HORZADDR, x);
		writeRegister(ST7781_VERTADDR, y);
		writeCommand(ST7781_WR_GRAM);
	}
};

/***************************************************
This is a library for the Adafruit 2.8" TFT display.
This library works with the Adafruit 2.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/335
as well as Adafruit TFT Touch Shield
  ----> http://www.adafruit.com/products/376
 
Check out the links above for our tutorials and wiring diagrams.
These displays use 8-bit parallel to communicate, 12 or 13 pins are required
to interface (RST is optional).
Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.
MIT license, all text above must be included in any redistribution

 ****************************************************/

// Constructor
PDQ_ST7781::PDQ_ST7781() : PDQ_GFX<PDQ_ST7781>(ST7781_TFTWIDTH, ST7781_TFTHEIGHT)
{
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void PDQ_ST7781::commandList(const struct reg_cmd *cmd, uint8_t numCommands)
{
	while (numCommands--)				// For each command...
	{
		uint8_t r = pgm_read_byte(&cmd->r);
		uint16_t v = pgm_read_word(&cmd->v);
		cmd++;
		if (r == DELAY)
		{
			lcd_end();
			delay(v);
			lcd_begin();
		}
		else
		{
			writeRegister(r, v);
		}
	}
}
      
void PDQ_ST7781::begin(void)
{
#if defined(USE_PORT_ACCESS)
	// set pin directions to output
	DDR_DATALO6 = DATALO6_MASK;
	DDR_DATAHI2 = RD_BIT|WR_BIT|RS_BIT|CS_BIT|DATAHI2_MASK;
	
	PORT_DATALO6 = 0;				// write to bits 0 & 1 UART ignored
	PORT_DATAHI2 = RD_BIT|WR_BIT|RS_BIT|CS_BIT;	// write to bits 6 & 7 XTAL ignored
#else

	FastPin<LCD_D0>::lo();
	FastPin<LCD_D0>::setOutput();
	FastPin<LCD_D1>::lo();
	FastPin<LCD_D1>::setOutput();
	FastPin<LCD_D2>::lo();
	FastPin<LCD_D2>::setOutput();
	FastPin<LCD_D3>::lo();
	FastPin<LCD_D3>::setOutput();
	FastPin<LCD_D4>::lo();
	FastPin<LCD_D4>::setOutput();
	FastPin<LCD_D5>::lo();
	FastPin<LCD_D5>::setOutput();
	FastPin<LCD_D6>::lo();
	FastPin<LCD_D6>::setOutput();
	FastPin<LCD_D7>::lo();
	FastPin<LCD_D7>::setOutput();
	FastPin<LCD_CS>::hi();
	FastPin<LCD_CS>::setOutput();
	FastPin<LCD_RS>::hi();
	FastPin<LCD_RS>::setOutput();
	FastPin<LCD_WR>::hi();
	FastPin<LCD_WR>::setOutput();
#if defined(LCD_RD)
	FastPin<LCD_RD>::hi();
	FastPin<LCD_RD>::setOutput();
#endif
#endif

	lcd_begin();
	
	static const struct reg_cmd ST7781_cmds[] PROGMEM =
	{
		{ 0x01, 0x0100 },
		{ 0x02, 0x0700 },
		{ 0x03, 0x1030 },
		{ 0x04, 0x0000 },
		{ 0x08, 0x0302 },
		{ 0x0A, 0x0000 },
		{ 0x0C, 0x0000 },
		{ 0x0D, 0x0000 },
		{ 0x0F, 0x0000 },
                                 
		{ DELAY,  100  },
                                 
		{ 0x30, 0x0000 },
		{ 0x31, 0x0405 },
		{ 0x32, 0x0203 },
		{ 0x35, 0x0004 },
		{ 0x36, 0x0B07 },
		{ 0x37, 0x0000 },
		{ 0x38, 0x0405 },
		{ 0x39, 0x0203 },
		{ 0x3c, 0x0004 },
		{ 0x3d, 0x0B07 },
		{ 0x20, 0x0000 },
		{ 0x21, 0x0000 },
		{ 0x50, 0x0000 },
		{ 0x51, 0x00ef },
		{ 0x52, 0x0000 },
		{ 0x53, 0x013f },

		{ DELAY,  100  },

		{ 0x60, 0xa700 },
		{ 0x61, 0x0001 },
		{ 0x90, 0x003A },
		{ 0x95, 0x021E },
		{ 0x80, 0x0000 },
		{ 0x81, 0x0000 },
		{ 0x82, 0x0000 },
		{ 0x83, 0x0000 },
		{ 0x84, 0x0000 },
		{ 0x85, 0x0000 },
		{ 0xFF, 0x0001 },
		{ 0xB0, 0x140D },
		{ 0xFF, 0x0000 },

		{ DELAY,  100  },
		
		{ 0x07, 0x0133 },

		{ DELAY,  50   },
		
		{ 0x10, 0x14e0 },

		{ DELAY,  100  },
		
		{ 0x07, 0x0133 },
	};

	commandList(ST7781_cmds, sizeof (ST7781_cmds) / sizeof (ST7781_cmds[0]));

	lcd_end();

	fillScreen(ST7781_BLACK);
	setRotation(rotation);
}

void PDQ_ST7781::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	lcd_begin();

	setAddrWindow_(x0, y0, x1, y1);

	lcd_end();
}

void PDQ_ST7781::pushColor(uint16_t color)
{
	lcd_begin();

	writeData(color);

	lcd_end();
}

void PDQ_ST7781::pushColor(uint16_t color, int count)
{
	lcd_begin();

	writeData(color, count);

	lcd_end();
}

void PDQ_ST7781::drawPixel(int x, int y, uint16_t color)
{
	if ((x < 0) ||(x >= _width) || (y < 0) || (y >= _height))
		return;

	lcd_begin();

	setAddrWindow_(x, y, x, y);

	writeData(color);

	lcd_end();
}

void PDQ_ST7781::drawFastVLine(int x, int y, int h, uint16_t color)
{
	// clipping
	if ((x < 0) || (x >= _width) || (y >= _height))
		return;
		
	if (y < 0)
	{
		h += y;
		y = 0;
	}

	int y1 = y+h;

	if (y1 < 0)
		return;

	if (y1 > _height) 
		h = _height-y;

	lcd_begin();

	setAddrWindow_(x, y, x, _height);
	for (; h > 0; h--)
	{
		writeData(color);
	}

	lcd_end();
}


void PDQ_ST7781::drawFastHLine(int x, int y, int w, uint16_t color)
{
	// clipping
	if ((x >= _width) || (y < 0) || (y >= _height))
		return;
		
	if (x < 0)
	{
		w += x;
		x = 0;
	}

	int x1 = x+w;

	if (x1 < 0)
		return;

	if (x1 > _width) 
		w = _width-w;

	lcd_begin();
	
	setAddrWindow_(x, y, _width, y);
	writeData(color, w);

	lcd_end();
}

void PDQ_ST7781::fillRect(int x, int y, int w, int h, uint16_t color)
{
	// rudimentary clipping (drawChar w/big text requires this)
	if ((x >= _width) || (y >= _height))
		return;

	if (x < 0)
	{
		w += x;
		x = 0;
	}
	if (y < 0)
	{
		h += y;
		y = 0;
	}
	if ((x + w) > _width)
		w = _width  - x;
	if ((y + h) > _height)
		h = _height - y;

	lcd_begin();

	setAddrWindow_(x, y, x+w-1, _height);

	for (; h > 0; h--)
	{
		writeData(color, w);
	}

	lcd_end();
}

// Bresenham's algorithm - thx Wikipedia
void PDQ_ST7781::drawLine(int x0, int y0, int x1, int y1, uint16_t color)
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

	if (x1 < 0)
		return;

	int dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int err = dx / 2;
	int8_t ystep;

	if (y0 < y1)
	{
		ystep = 1;
	}
	else
	{
		ystep = -1;
	}
	
	uint8_t setaddr = 1;

	if (steep)	// y increments every iteration (y0 is x-axis, and x0 is y-axis)
	{
		if (x1 >= _height)
			x1 = _height - 1;

		for (; x0 <= x1; x0++)
		{
			if ((x0 >= 0) && (y0 >= 0) && (y0 < _width))
				break;

			err -= dy;
			if (err < 0)
			{
				err += dx;
				y0 += ystep;
			}
		}

		if (x0 > x1)
			return;

		lcd_begin();
	
		for (; x0 <= x1; x0++)
		{
			if (setaddr)
			{
				setAddrWindow_(y0, x0, y0, _height);
				setaddr = 0;
			}
			writeData(color);
			err -= dy;
			if (err < 0)
			{
				y0 += ystep;
				if ((y0 < 0) || (y0 >= _width))
					break;
				err += dx;
				setaddr = 1;
			}
		}
	}
	else		// x increments every iteration (x0 is x-axis, and y0 is y-axis)
	{
		if (x1 >= _width)
			x1 = _width - 1;

		for (; x0 <= x1; x0++)
		{
			if ((x0 >= 0) && (y0 >= 0) && (y0 < _height))
				break;

			err -= dy;
			if (err < 0)
			{
				err += dx;
				y0 += ystep;
			}
		}

		if (x0 > x1)
			return;

		lcd_begin();

		for (; x0 <= x1; x0++)
		{
			if (setaddr)
			{
				setAddrWindow_(x0, y0, _width, y0);
				setaddr = 0;
			}
			writeData(color);
			err -= dy;
			if (err < 0)
			{
				y0 += ystep;
				if ((y0 < 0) || (y0 >= _height))
					break;
				err += dx;
				setaddr = 1;
			}
		}
	}
	
	lcd_end();
}

void PDQ_ST7781::setRotation(uint8_t m)
{
	uint8_t id_am;

#if defined(FIXED_ROTATION)
	(void)m;
	switch (FIXED_ROTATION)
#else
	rotation = (m & 3); // can't be higher than 3
	switch (rotation)
#endif
	{
	default:
	case 0:
		id_am = 0x30;
		_width	= ST7781_TFTWIDTH;
		_height = ST7781_TFTHEIGHT;
		break;
	case 1:
		id_am = 0x28;
		_width	= ST7781_TFTHEIGHT;
		_height = ST7781_TFTWIDTH;
		break;
	case 2:
		id_am = 0x00;
		_width	= ST7781_TFTWIDTH;
		_height = ST7781_TFTHEIGHT;
		break;
	case 3:
		id_am = 0x18;
		_width	= ST7781_TFTHEIGHT;
		_height = ST7781_TFTWIDTH;
		break;
	}

	lcd_begin();

	writeRegister(ST7781_ENTERMOD, (0x10<<8) | id_am);
	setAddrWindow_(0, 0, _width - 1, _height - 1);

	lcd_end();
}

#endif		// !defined(_PDQ_ST7781H_)
