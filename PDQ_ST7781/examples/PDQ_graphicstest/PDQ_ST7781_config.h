//
// PDQ_ST7781 configuration (using 8-bit "parallel" interface with 16-bit data)
//
// You need to include this file above #include "PDQ_ST7781.h" in your sketch.

#if 0	// set to 1 for SeeedStudio Touch Shield V1.0 PORT optimized version

// Seeed Studio Touch Shield V1.0 pin usage:
// D2 - LCD data bit 0
// D3 - LCD data bit 1
// D4 - LCD data bit 2
// D5 - LCD data bit 3
// D6 - LCD data bit 4
// D7 - LCD data bit 5
// D8 - LCD data bit 6
// D9 - LCD data bit 7
// D10 - LCD CS pin, active low.
// D11 - LCD RS pin.
// D12 - LCD WR pin.
// D13 - LCD RD pin.
//
// LCD Data Bit :    7    6    5    4    3    2    1    0
// Uno port/pin :  PB1  PB0  PB7  PD6  PD5  PD4  PD3  PD2

#define	USE_PORT_ACCESS

#define PORT_DATALO6	PORTD	// PD7=D5 PD6=D4 PD5=D3 PD4=D2 PD3=D1 PD2=D0 x x
#define DDR_DATALO6		DDRD
#define DATALO6_MASK	0xfc

#define PORT_DATAHI2	PORTB	// x x PB5=RD PB4=WR PB3=RS PB2=CS PB1=D7 PB0=D6
#define DDR_DATAHI2		DDRB
#define DATAHI2_MASK	0x03
#define CS_BIT			0x04
#define RS_BIT			0x08
#define WR_BIT			0x10
#define RD_BIT			0x20	// read functionality is not used by this library

#else	// else, individually assignable pin version (but significantly slower)

// set these below for your shield/breakout pins (Arduino "digitalWrite" numbering)
#define LCD_D0			2
#define LCD_D1			3
#define LCD_D2			4
#define LCD_D3			5
#define LCD_D4			6
#define LCD_D5			7
#define LCD_D6			8
#define LCD_D7			9
#define LCD_CS			10
#define LCD_RS			11		// sometimes called "DC"
#define	LCD_WR			12
#define	LCD_RD			13		// read functionality is not used by this library (comment this out if you don't have this pin)
	
#endif

#define	FIXED_ROTATION	0		// 0 to 3 or comment-out for run-time rotation switching (slightly slower and bigger)
