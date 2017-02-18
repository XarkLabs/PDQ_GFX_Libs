// This is the PDQ re-mixed version of Adafruit's library
// here is the original copyright notice:

/***************************************************
  This is an Arduino Library for the Adafruit 2.2" SPI display.
  This library works with the Adafruit 2.2" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/1480
 
  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

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
// 2) Between ~2.5 and ~12 times faster (fillRect ~2.5x, drawLine ~12x).
// An average of ~4x faster over entire "graphictest.ino" benchmark.
//
// Even if this library is faster, it was based on the Adafruit original. 
// Adafruit deserves your support for making their library open-source (and
// for having some nice LCD modules and all kinds of other great parts too).
// Consider giving them your support if possible!

#if !defined(_PDQ_ILI9340H_)
#define _PDQ_ILI9340H_

#include "Arduino.h"
#include "Print.h"

#include <PDQ_GFX.h>

#include <avr/pgmspace.h>

#if !defined(ILI9340_CS_PIN) || !defined(ILI9340_DC_PIN)
#error Oops!  You need to #include "PDQ_ILI9340_config.h" (modified with your pin configuration and options) from your sketch before #include "PDQ_ILI9340.h".
#endif

#include <PDQ_FastPin.h>

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
	ILI9340_BLACK	= 0x0000,
	ILI9340_BLUE	= 0x001F,
	ILI9340_RED		= 0xF800,
	ILI9340_GREEN	= 0x07E0,
	ILI9340_CYAN	= 0x07FF,
	ILI9340_MAGENTA = 0xF81F,
	ILI9340_YELLOW	= 0xFFE0,	
	ILI9340_WHITE	= 0xFFFF,
};

class PDQ_ILI9340 : public PDQ_GFX<PDQ_ILI9340>
{
 public:
	// ILI9340 commands
	// For datasheet see https://www.adafruit.com/products/1480
 	enum
	{
		ILI9340_NOP			= 0x00,
		ILI9340_SWRESET		= 0x01,
		ILI9340_RDDID		= 0x04,
		ILI9340_RDDST		= 0x09,

		ILI9340_SLPIN		= 0x10,
		ILI9340_SLPOUT		= 0x11,
		ILI9340_PTLON		= 0x12,
		ILI9340_NORON		= 0x13,

		ILI9340_RDMODE		= 0x0A,
		ILI9340_RDMADCTL	= 0x0B,
		ILI9340_RDPIXFMT	= 0x0C,
		ILI9340_RDIMGFMT	= 0x0A,
		ILI9340_RDSELFDIAG	= 0x0F,

		ILI9340_INVOFF		= 0x20,
		ILI9340_INVON		= 0x21,
		ILI9340_GAMMASET	= 0x26,
		ILI9340_DISPOFF		= 0x28,
		ILI9340_DISPON		= 0x29,

		ILI9340_CASET		= 0x2A,
		ILI9340_PASET		= 0x2B,
		ILI9340_RAMWR		= 0x2C,
		ILI9340_RAMRD		= 0x2E,

		ILI9340_PTLAR		= 0x30,
		ILI9340_MADCTL		= 0x36,
		ILI9340_PIXFMT		= 0x3A,

		ILI9340_FRMCTR1		= 0xB1,
		ILI9340_FRMCTR2		= 0xB2,
		ILI9340_FRMCTR3		= 0xB3,
		ILI9340_INVCTR		= 0xB4,
		ILI9340_DFUNCTR		= 0xB6,

		ILI9340_PWCTR1		= 0xC0,
		ILI9340_PWCTR2		= 0xC1,
		ILI9340_PWCTR3		= 0xC2,
		ILI9340_PWCTR4		= 0xC3,
		ILI9340_PWCTR5		= 0xC4,
		ILI9340_VMCTR1		= 0xC5,
		ILI9340_VMCTR2		= 0xC7,

		ILI9340_RDID1		= 0xDA,
		ILI9340_RDID2		= 0xDB,
		ILI9340_RDID3		= 0xDC,
		ILI9340_RDID4		= 0xDD,

		ILI9340_GMCTRP1		= 0xE0,
		ILI9340_GMCTRN1		= 0xE1,

		// ILI9340_PWCTR6	= 0xFC,
	};

	// some other misc. constants
	enum
	{
		// screen dimensions
		ILI9340_TFTWIDTH	= 240,
		ILI9340_TFTHEIGHT	= 320,

		// MADCTL bits
		ILI9340_MADCTL_MH	= 0x04,	// bit 2 = 0 for refresh left -> right, 1 for refresh right -> left
		ILI9340_MADCTL_RGB	= 0x00,	// bit 3 = 0 for RGB color order
		ILI9340_MADCTL_BGR	= 0x08,	// bit 3 = 1 for BGR color order
		ILI9340_MADCTL_ML	= 0x10,	// bit 4 = 0 for refresh top -> bottom, 1 for bottom -> top
		ILI9340_MADCTL_MV	= 0x20,	// bit 5 = 0 for column, row order (portrait), 1 for row, column order (landscape)
		ILI9340_MADCTL_MX	= 0x40,	// bit 6 = 0 for left -> right, 1 for right -> left order
		ILI9340_MADCTL_MY	= 0x80,	// bit 7 = 0 for top -> bottom, 1 for bottom -> top

		// delay indicator bit for commandList()
		DELAY				= 0x80
	};

	// higher-level routines
	PDQ_ILI9340();
	static void inline begin(void);
	static void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
	static void pushColor(uint16_t color);
	static void pushColor(uint16_t color, int cnt);

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
	static void commandList(const uint8_t *addr);

	// NOTE: Make sure each spi_begin() is matched with a single spi_end() (and don't call either twice)
	// set CS back to low (LCD selected)
	static inline void spi_begin() __attribute__((always_inline))
	{
#if ILI9340_SAVE_SPCR && defined(AVR_HARDWARE_SPI)
		swapValue(save_SPCR, SPCR);	// swap initial/current SPCR settings
#endif
		FastPin<ILI9340_CS_PIN>::lo();		// CS <= LOW (selected)
	}

	// NOTE: Make sure each spi_begin() is matched with a single spi_end() (and don't call either twice)
	// reset CS back to high (LCD unselected)
	static inline void spi_end() __attribute__((always_inline))
	{
		FastPin<ILI9340_CS_PIN>::hi();		// CS <= HIGH (deselected)
#if ILI9340_SAVE_SPCR && defined(AVR_HARDWARE_SPI)
		swapValue(SPCR, save_SPCR);	// swap current/initial SPCR settings
#endif
	}

#if defined(AVR_HARDWARE_SPI)
	// 10 cycle delay (including "call")
	static void delay10() __attribute__((noinline)) __attribute__((naked))
	{
		__asm__ __volatile__
		(
									// +4 (call to get here)
#if !defined(__AVR_HAVE_RAMPD__)	
			"	adiw	r24,0\n"	// +2 (2-cycle NOP)
#else
			"	nop\n"				// +1 (1-cycle NOP)
#endif
			"	ret\n"				// +4 (or +5 on >64KB AVR with RAMPD reg)
									// = 10 cycles
			: : : 
		);
	}

	// 13 cycle delay (including "call")
	static void delay13() __attribute__((noinline)) __attribute__((naked))
	{
		__asm__ __volatile__
		(
									// +4 (call to get here)
			"	adiw	r24,0\n"	// +2 (2-cycle NOP)
			"	adiw	r24,0\n"	// +2 (2-cycle NOP)
#if !defined(__AVR_HAVE_RAMPD__)	
			"	nop\n"				// +1 (1-cycle NOP)
#endif
			"	ret\n"				// +4 (or +5 on >64KB AVR with RAMPD reg)
									// = 13 cycles
			: : : 
		);
	}

	// 15 cycle delay (including "call")
	static void delay15() __attribute__((noinline)) __attribute__((naked))
	{
		__asm__ __volatile__
		(
									// +4 (call to get here)
			"	adiw	r24,0\n"	// +2 (2-cycle NOP)
			"	adiw	r24,0\n"	// +2 (2-cycle NOP)
			"	adiw	r24,0\n"	// +2 (2-cycle NOP)
#if !defined(__AVR_HAVE_RAMPD__)	
			"	nop\n"				// +1 (1-cycle NOP)
#endif
			"	ret\n"				// +4 (or +5 on >64KB AVR with RAMPD reg)
									// = 15 cycles
			: : : 
		);
	}

	// 17 cycle delay (including "call")
	static void delay17() __attribute__((noinline)) __attribute__((naked))
	{
		__asm__ __volatile__
		(
									// +4 (call to get here)
			"	adiw	r24,0\n"	// +2 (2-cycle NOP)
			"	adiw	r24,0\n"	// +2 (2-cycle NOP)
			"	adiw	r24,0\n"	// +2 (2-cycle NOP)
			"	adiw	r24,0\n"	// +2 (2-cycle NOP)
#if !defined(__AVR_HAVE_RAMPD__)	
			"	nop\n"				// +1 (2-cycle NOP)
#endif
			"	ret\n"				// +4 (or +5 on >64KB AVR with RAMPD reg)
									// = 17 cycles
			: : : 
		);
	}
	
	// normal SPI write with minimal hand-tuned delay (assuming max DIV2 SPI rate)
	static INLINE void spiWrite(uint8_t data) INLINE_OPT
	{
		SPDR = data;
		__asm__ __volatile__
		(
			"	call	_ZN11PDQ_ILI93407delay17Ev\n"	// call mangled delay17 (compiler would needlessly save/restore regs)
			: : : 
		);
	}

	// special SPI write with minimal hand-tuned delay (assuming max DIV2 SPI rate) - minus 2 cycles for RS (etc.) change
	static INLINE void spiWrite_preCmd(uint8_t data) INLINE_OPT
	{
		SPDR = data;

		__asm__ __volatile__
		(
			"	call	_ZN11PDQ_ILI93407delay15Ev\n"	// call mangled delay15 (compiler would needlessly save/restore regs)
			: : : 
		);
	}

	// SPI 16-bit write with minimal hand-tuned delay (assuming max DIV2 SPI rate)
	static INLINE void spiWrite16(uint16_t data) INLINE_OPT
	{
		uint8_t temp;
		__asm__ __volatile__
		(
			"	out	%[spi],%[hi]\n"				// write SPI data (18 cycles until next write)
			"	call	_ZN11PDQ_ILI93407delay17Ev\n"	// call mangled delay17 (compiler would needlessly save/restore regs)
			"	out	%[spi],%[lo]\n"				// write SPI data (18 cycles until next write)
			"	call	_ZN11PDQ_ILI93407delay17Ev\n"	// call mangled delay17 (compiler would needlessly save/restore regs)

			: [temp] "=d" (temp)
			: [spi] "i" (_SFR_IO_ADDR(SPDR)), [lo] "r" ((uint8_t)data), [hi] "r" ((uint8_t)(data>>8))
			: 
		);
	}

	// SPI 16-bit write with minimal hand-tuned delay (assuming max DIV2 SPI rate) minus 2 cycles
	static INLINE void spiWrite16_preCmd(uint16_t data) INLINE_OPT
	{
		uint8_t temp;
		__asm__ __volatile__
		(
			"	out	%[spi],%[hi]\n"				// write SPI data (18 cycles until next write)
			"	call	_ZN11PDQ_ILI93407delay17Ev\n"	// call mangled delay17 (compiler would needlessly save/restore regs)
			"	out	%[spi],%[lo]\n"				// write SPI data (18 cycles until next write)
			"	call	_ZN11PDQ_ILI93407delay15Ev\n"	// call mangled delay15 (compiler would needlessly save/restore regs)

			: [temp] "=d" (temp)
			: [spi] "i" (_SFR_IO_ADDR(SPDR)), [lo] "r" ((uint8_t)data), [hi] "r" ((uint8_t)(data>>8))
			: 
		);
	}

	// SPI 16-bit write with minimal hand-tuned delay (assuming max DIV2 SPI rate) minus 4 cycles
	static INLINE void spiWrite16_lineDraw(uint16_t data) INLINE_OPT
	{
		uint8_t temp;
		__asm__ __volatile__
		(
			"	out	%[spi],%[hi]\n"				// write SPI data (18 cycles until next write)
			"	call	_ZN11PDQ_ILI93407delay17Ev\n"	// call mangled delay17 (compiler would needlessly save/restore regs)
			"	out	%[spi],%[lo]\n"				// write SPI data (18 cycles until next write)

			: [temp] "=d" (temp)
			: [spi] "i" (_SFR_IO_ADDR(SPDR)), [lo] "r" ((uint8_t)data), [hi] "r" ((uint8_t)(data>>8))
			: 
		);
	}

	// normal SPI write with minimal hand-tuned delay (assuming max DIV2 SPI rate)
	static INLINE void spiWrite16(uint16_t data, int count) INLINE_OPT
	{
		uint8_t temp;
		__asm__ __volatile__
		(
			"	sbiw	%[count],0\n"				// test count
			"	brmi	4f\n"					// if < 0 then done
			"	breq	4f\n"					// if == 0 then done
			"1:	out	%[spi],%[hi]\n"				// write SPI data (18 cycles until next write)
			"	call	_ZN11PDQ_ILI93407delay17Ev\n"	// call mangled delay17 (compiler would needlessly save/restore regs)
			"	out	%[spi],%[lo]\n"				// write SPI data (18 cycles until next write)
			"	call	_ZN11PDQ_ILI93407delay13Ev\n"	// call mangled delay13 (compiler would needlessly save/restore regs)
			"	sbiw	%[count],1\n"				// +2	decrement count
			"	brne	1b\n"					// +2/1	if != 0 then loop
										// = 13 + 2 + 2 (17 cycles)			
			"4:\n"

			: [temp] "=d" (temp), [count] "+w" (count)
			: [spi] "i" (_SFR_IO_ADDR(SPDR)), [lo] "r" ((uint8_t)data), [hi] "r" ((uint8_t)(data>>8))
			: 
		);
	}
	
#else	// bit-bang
#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny45__)
	// USI hardware assisted
	static void spiWrite(uint8_t data) __attribute__((noinline))
	{
		USIDR = data;
		__asm__ __volatile__
		(
			"	out %[spi],%[clkp0]\n"	// MSB
			"	out %[spi],%[clkp1]\n"
			"	out %[spi],%[clkp0]\n"
			"	out %[spi],%[clkp1]\n"
			"	out %[spi],%[clkp0]\n"
			"	out %[spi],%[clkp1]\n"
			"	out %[spi],%[clkp0]\n"
			"	out %[spi],%[clkp1]\n"
			"	out %[spi],%[clkp0]\n"
			"	out %[spi],%[clkp1]\n"
			"	out %[spi],%[clkp0]\n"
			"	out %[spi],%[clkp1]\n"
			"	out %[spi],%[clkp0]\n"
			"	out %[spi],%[clkp1]\n"
			"	out %[spi],%[clkp0]\n"	// LSB
			"	out %[spi],%[clkp1]\n"
			: 
			: [spi] "i" (_SFR_IO_ADDR(USICR)), [clkp0] "a" ((uint8_t)((1<<USIWM0)|(0<<USICS0)|(1<<USITC))), [clkp1] "a" ((uint8_t)((1<<USIWM0)|(0<<USICS0)|(1<<USITC)|(1<<USICLK)))
			:
		);
	}
	static void spiWrite16(uint16_t data) __attribute__((noinline))
	{
		USIDR = data>>8;
		__asm__ __volatile__
		(
			"	out %[spi],%[clkp0]\n"	// MSB
			"	out %[spi],%[clkp1]\n"
			"	out %[spi],%[clkp0]\n"
			"	out %[spi],%[clkp1]\n"
			"	out %[spi],%[clkp0]\n"
			"	out %[spi],%[clkp1]\n"
			"	out %[spi],%[clkp0]\n"
			"	out %[spi],%[clkp1]\n"
			"	out %[spi],%[clkp0]\n"
			"	out %[spi],%[clkp1]\n"
			"	out %[spi],%[clkp0]\n"
			"	out %[spi],%[clkp1]\n"
			"	out %[spi],%[clkp0]\n"
			"	out %[spi],%[clkp1]\n"
			"	out %[spi],%[clkp0]\n"	// LSB
			"	out %[spi],%[clkp1]\n"
			: 
			: [spi] "i" (_SFR_IO_ADDR(USICR)), [clkp0] "a" ((uint8_t)((1<<USIWM0)|(0<<USICS0)|(1<<USITC))), [clkp1] "a" ((uint8_t)((1<<USIWM0)|(0<<USICS0)|(1<<USITC)|(1<<USICLK)))
			:
		);

		USIDR = data&0xff;
		__asm__ __volatile__
		(
			"	out %[spi],%[clkp0]\n"	// MSB
			"	out %[spi],%[clkp1]\n"
			"	out %[spi],%[clkp0]\n"
			"	out %[spi],%[clkp1]\n"
			"	out %[spi],%[clkp0]\n"
			"	out %[spi],%[clkp1]\n"
			"	out %[spi],%[clkp0]\n"
			"	out %[spi],%[clkp1]\n"
			"	out %[spi],%[clkp0]\n"
			"	out %[spi],%[clkp1]\n"
			"	out %[spi],%[clkp0]\n"
			"	out %[spi],%[clkp1]\n"
			"	out %[spi],%[clkp0]\n"
			"	out %[spi],%[clkp1]\n"
			"	out %[spi],%[clkp0]\n"	// LSB
			"	out %[spi],%[clkp1]\n"
			: 
			: [spi] "i" (_SFR_IO_ADDR(USICR)), [clkp0] "a" ((uint8_t)((1<<USIWM0)|(0<<USICS0)|(1<<USITC))), [clkp1] "a" ((uint8_t)((1<<USIWM0)|(0<<USICS0)|(1<<USITC)|(1<<USICLK)))
			:
		);
	}
#else
	static void spiWrite(uint8_t data) __attribute__((noinline))
	{
		// Fast SPI bitbang swiped from LPD8806 library
		for(uint8_t bit = 0x80; bit; bit >>= 1)
		{
			if (data & bit)
				FastPin<ILI9340_MOSI_PIN>::hi();
			else
				FastPin<ILI9340_MOSI_PIN>::lo();

			FastPin<ILI9340_SCLK_PIN>::hi();
			FastPin<ILI9340_SCLK_PIN>::lo();
		}
	}
	static void spiWrite16(uint16_t data) __attribute__((noinline))
	{
		spiWrite(data >> 8);
		spiWrite(data & 0xff);
	}
#endif
	static INLINE void spiWrite_preCmd(uint8_t data) INLINE_OPT
	{
		spiWrite(data);
	}
	static INLINE void spiWrite16_preCmd(uint16_t data) INLINE_OPT
	{
		spiWrite16(data);
	}
	static INLINE void spiWrite16_lineDraw(uint16_t data) INLINE_OPT
	{
		spiWrite16(data);
	}
	static INLINE void spiWrite16(uint16_t data, int count) INLINE_OPT
	{
		while (count-- > 0)
			spiWrite16(data);
	}
	static inline void delay10()	{ }
	static inline void delay13()	{ }
	static inline void delay15()	{ }
	static inline void delay17()	{ }
#endif

	// write SPI byte with RS (aka D/C) pin set low to indicate a command byte (and then reset back to high when done)
	static INLINE void writeCommand(uint8_t data) INLINE_OPT
	{
		FastPin<ILI9340_DC_PIN>::lo();		// RS <= LOW indicate command byte
		spiWrite_preCmd(data);
		FastPin<ILI9340_DC_PIN>::hi();		// RS <= HIGH indicate data byte (always assumed left in data mode)
	}

	// write SPI byte with RS assumed low indicating a data byte
	static inline void writeData(uint8_t data) __attribute__((always_inline))
	{
		spiWrite(data);
	} 

	// internal version that does not spi_begin()/spi_end()
	static INLINE void setAddrWindow_(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) INLINE_OPT
	{
		writeCommand(ILI9340_CASET); 		// column address set
		spiWrite16(x0);				// XSTART
		spiWrite16_preCmd(x1);			// XEND
		writeCommand(ILI9340_PASET); 		// row address set
		spiWrite16(y0);				// YSTART
		spiWrite16_preCmd(y1);		 	// YEND
		writeCommand(ILI9340_RAMWR); 		// write to RAM
	}

#if ILI9340_SAVE_SPCR && defined(AVR_HARDWARE_SPI)
	static volatile uint8_t	save_SPCR;	// initial SPCR value/saved SPCR value (swapped in spi_begin/spi_end)
#endif
};

typedef PDQ_GFX_Button_<PDQ_ILI9340>	PDQ_GFX_Button;

/***************************************************
  This is an Arduino Library for the Adafruit 2.2" SPI display.
  This library works with the Adafruit 2.2" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/1480
 
  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#if ILI9340_SAVE_SPCR && defined(AVR_HARDWARE_SPI)
// static data needed by base class
volatile uint8_t PDQ_ILI9340::save_SPCR;
#endif

// Constructor when using hardware SPI.
PDQ_ILI9340::PDQ_ILI9340() : PDQ_GFX<PDQ_ILI9340>(ILI9340_TFTWIDTH, ILI9340_TFTHEIGHT)
{
#if defined(AVR_HARDWARE_SPI)
	// must reference these functions from C++ or they will be stripped by linker (called from inline asm)
	delay10();
	delay13();
	delay15();
	delay17();
#endif
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void PDQ_ILI9340::commandList(const uint8_t *addr)
{
	uint8_t	numCommands, numArgs;
	uint16_t ms;

	numCommands = pgm_read_byte(addr++);		// Number of commands to follow
	while (numCommands--)				// For each command...
	{
		writeCommand(pgm_read_byte(addr++));	// Read, issue command
		numArgs	= pgm_read_byte(addr++);	// Number of args to follow
		ms = numArgs & DELAY;			// If hibit set, delay follows args
		numArgs &= ~DELAY;			// Mask out delay bit
		while (numArgs--)			// For each argument...
		{
			writeData(pgm_read_byte(addr++)); // Read, issue argument
		}

		if (ms)
		{
			ms = pgm_read_byte(addr++);	// Read post-command delay time (ms)
			if(ms == 255)
				ms = 500;		// If 255, delay for 500 ms
			delay(ms);
		}
	}
}
      
void PDQ_ILI9340::begin(void)
{
	// set CS and RS pin directions to output
	FastPin<ILI9340_CS_PIN>::setOutput();
	FastPin<ILI9340_DC_PIN>::setOutput();
#if !defined(AVR_HARDWARE_SPI)
	#if defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
		USICR = (0<<USISIE)|(0<<USIOIE)|(0<<USIWM1)|(1<<USIWM0)|(0<<USICS1)|(1<<USICS0)|(1<<USICLK)|(0<<USITC);
	#endif
	#if defined (ILI9340_MISO_PIN)
		FastPin<ILI9340_MISO_PIN>::setInput();
	#endif
	FastPin<ILI9340_MOSI_PIN>::setOutput();
	FastPin<ILI9340_SCLK_PIN>::setOutput();
	FastPin<ILI9340_MOSI_PIN>::lo();
	FastPin<ILI9340_SCLK_PIN>::lo();
#endif
	
	FastPin<ILI9340_CS_PIN>::hi();		// CS <= HIGH (deselected, so no spurious data)
	FastPin<ILI9340_DC_PIN>::hi();		// RS <= HIGH (default data byte)

#if defined(AVR_HARDWARE_SPI)
	#if ILI9340_SAVE_SPCR
		uint8_t oldSPCR = SPCR;	// save initial SPCR settings
	#endif	
	SPI.begin();
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);
	SPI.setClockDivider(SPI_CLOCK_DIV2);	// 8 MHz (full! speed!) [1 byte every 18 cycles]
#endif
	
#if ILI9340_SAVE_SPCR && defined(AVR_HARDWARE_SPI)
	save_SPCR = SPCR;	// save SPCR settings
	SPCR = oldSPCR;		// restore previous SPCR settings (spi_begin/spi_end will switch between the two)
#endif	
	spi_begin();

	// Initialization commands for ILI9340 screens
	static const uint8_t ILI9340_cmds[] PROGMEM =
	{
		22,
		ILI9340_SWRESET, DELAY,		// 1
		5,
		0xEF, 3,			// 2
		0x03, 0x80, 0x02,
		0xCF, 3,			// 3
		0x00, 0xC1, 0x30,
		0xED, 4,			// 4
		0x64, 0x03, 0x12, 0x81,
		0xE8, 3,			// 5
		0x85, 0x00, 0x78,
		0xCB, 5,			// 6
		0x39, 0x2C, 0x00, 0x34, 0x02,
		0xF7, 1,			// 7
		0x20,
		0xEA, 2,			// 8
		0x00, 0x00,
		ILI9340_PWCTR1, 1,		// 9 power control 
		0x23,				// VRH[5:0] 
		ILI9340_PWCTR2, 1,		// 10 power control 
		0x10,				// SAP[2:0];BT[3:0]  
		ILI9340_VMCTR1, 2,		// 11 VCM control 
		0x3e, 0x28,
		ILI9340_VMCTR2, 1,		// 12 VCM control2 
		0x86,				// --
		ILI9340_MADCTL, 1,		// 13
		(ILI9340_MADCTL_MX | ILI9340_MADCTL_BGR),
		ILI9340_PIXFMT, 1,		// 14
		0x55,
		ILI9340_FRMCTR1, 2,		// 15
		0x00, 0x18,
		ILI9340_DFUNCTR, 3,		// 16
		0x08, 0x82, 0x27,
		0xF2, 1,			// 17 3Gamma Function Disable 
		0x00,
		ILI9340_GAMMASET, 1,		// 18 Gamma curve selected 
		0x01,
		ILI9340_GMCTRP1, 15,		// 19 Set Gamma 
		0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
		ILI9340_GMCTRN1, 15,		// 20
		0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
		ILI9340_SLPOUT, DELAY,		// 21
		120,
		ILI9340_DISPON, 0,		// 22
	};

	commandList(ILI9340_cmds);
	
	spi_end();
}

void PDQ_ILI9340::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	spi_begin();

	setAddrWindow_(x0, y0, x1, y1);

	spi_end();
}

void PDQ_ILI9340::pushColor(uint16_t color)
{
	spi_begin();

	spiWrite16_preCmd(color);

	spi_end();
}

void PDQ_ILI9340::pushColor(uint16_t color, int count)
{
	spi_begin();

	spiWrite16(color, count);

	spi_end();
}

void PDQ_ILI9340::drawPixel(int x, int y, uint16_t color)
{
	if ((x < 0) ||(x >= _width) || (y < 0) || (y >= _height))
		return;

	spi_begin();

	setAddrWindow_(x, y, x, y);

	spiWrite16_preCmd(color);

	spi_end();
}

void PDQ_ILI9340::drawFastVLine(int x, int y, int h, uint16_t color)
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

	spi_begin();

	setAddrWindow_(x, y, x, _height);
	spiWrite16(color, h);

	spi_end();
}


void PDQ_ILI9340::drawFastHLine(int x, int y, int w, uint16_t color)
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

	spi_begin();
	
	setAddrWindow_(x, y, _width, y);
	spiWrite16(color, w);

	spi_end();
}

void PDQ_ILI9340::fillRect(int x, int y, int w, int h, uint16_t color)
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

	spi_begin();

	setAddrWindow_(x, y, x+w-1, _height);

	for (; h > 0; h--)
	{
		spiWrite16(color, w);
	}

	spi_end();
}

// Bresenham's algorithm - thx Wikipedia
void PDQ_ILI9340::drawLine(int x0, int y0, int x1, int y1, uint16_t color)
{
#if 0 && defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny45__)
	drawLine_(x0, y0, x1, y1, color);
#else
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

#if 0 && defined(__AVR_ATtiny85__) && !defined(__AVR_ATtiny45__)
	int	end = steep ? _height-1 : _width-1;
	if (x1 > end)
		x1 = end;

	for (; x0 <= x1; x0++)
	{
		if ((x0 >= 0) && (y0 >= 0) && (y0 <= end))
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

	spi_begin();

	for (; x0 <= x1; x0++)
	{
		if (setaddr)
		{
			if (steep)
				setAddrWindow_(y0, x0, y0, end+1);
			else
				setAddrWindow_(x0, y0, end+1, y0);
			setaddr = 0;
		}
		spiWrite16_lineDraw(color);
		err -= dy;
		if (err < 0)
		{
			y0 += ystep;
			if ((y0 < 0) || (y0 > end))
				break;
			err += dx;
			setaddr = 1;
		}
	}
#else
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

		spi_begin();
	
		for (; x0 <= x1; x0++)
		{
			if (setaddr)
			{
				setAddrWindow_(y0, x0, y0, _height);
				setaddr = 0;
			}
			spiWrite16_lineDraw(color);
			err -= dy;
			if (err < 0)
			{
				y0 += ystep;
				if ((y0 < 0) || (y0 >= _width))
					break;
				err += dx;
				setaddr = 1;
			}
#if defined(AVR_HARDWARE_SPI)
			else
			{
				__asm__ __volatile__
				(
					"	call	_ZN11PDQ_ILI93407delay10Ev\n"
					: : :
				);
			}
#endif
		}
	}
	else	// x increments every iteration (x0 is x-axis, and y0 is y-axis)
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

		spi_begin();

		for (; x0 <= x1; x0++)
		{
			if (setaddr)
			{
				setAddrWindow_(x0, y0, _width, y0);
				setaddr = 0;
			}
			spiWrite16_lineDraw(color);
			err -= dy;
			if (err < 0)
			{
				y0 += ystep;
				if ((y0 < 0) || (y0 >= _height))
					break;
				err += dx;
				setaddr = 1;
			}
#if defined(AVR_HARDWARE_SPI)
			else
			{
				__asm__ __volatile__
				(
					"	call	_ZN11PDQ_ILI93407delay10Ev\n"
					: : :
				);
			}
#endif
		}
	}
#endif
	
	spi_end();
#endif
}

void PDQ_ILI9340::setRotation(uint8_t m)
{
	rotation = (m & 3); // can't be higher than 3

	spi_begin();

	writeCommand(ILI9340_MADCTL);

	switch (rotation)
	{
	default:
	case 0:
		writeData(ILI9340_MADCTL_MX | ILI9340_MADCTL_BGR);
		_width	= ILI9340_TFTWIDTH;
		_height = ILI9340_TFTHEIGHT;
		break;
	case 1:
		writeData(ILI9340_MADCTL_MV | ILI9340_MADCTL_BGR);
		_width	= ILI9340_TFTHEIGHT;
		_height = ILI9340_TFTWIDTH;
		break;
	case 2:
		writeData(ILI9340_MADCTL_MY | ILI9340_MADCTL_BGR);
		_width	= ILI9340_TFTWIDTH;
		_height = ILI9340_TFTHEIGHT;
		break;
	case 3:
		writeData(ILI9340_MADCTL_MV | ILI9340_MADCTL_MY | ILI9340_MADCTL_MX | ILI9340_MADCTL_BGR);
		_width	= ILI9340_TFTHEIGHT;
		_height = ILI9340_TFTWIDTH;
		break;
	}

	spi_end();
}

void PDQ_ILI9340::invertDisplay(boolean i)
{
	spi_begin();

	writeCommand(i ? ILI9340_INVON : ILI9340_INVOFF);

	spi_end();
}

#endif		// !defined(_PDQ_ILI9340H_)
