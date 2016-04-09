//
// PDQ_ILI9341 configuration
//
// You need to include this file above #include "PDQ_ILI9341.h" in your sketch.
// Check settings on lines marked with "<=".

#if !defined(__AVR_ATtiny85__) && !defined(__AVR_ATtiny45__)
#error Sorry, this example is designed for ATtiny parts
#endif

//
// NOTE: It is confusing, but USI SPI on ATtiny does not use the same pins as ISP SPI!
//       Specifically MOSI is really DO pin 1 (even though used as MISO for ISP)
//
// These are based on Arduino ATtiny pins as defined here http://highlowtech.org/?p=1695

// LCD PIN	ATtiny85
// -------	---------
// 1  VCC	  3.3V/5V  (depending on module)
// 2  GND	  GND
// 3* CS	   0
// 4* RESET   3.3V/5V  (depending on module, perhaps through 10K pull-up)
// 5* DC/RS	   3
// 6* SDI/MOSI 1
// 7* SCK	   2
// 8* LED	  3.3V/5V
// 9  SDO/MISO -
//
//  * = Typically only 3.3V safe logic-line (unless board has level converter [ala Adafruit]). Be careful with 5V!

#define	ILI9341_CS_PIN		0			// <= /CS pin (chip-select, LOW to get attention of ST7735, HIGH and it ignores SPI bus)
#define	ILI9341_DC_PIN		3			// <= DC pin (1=data or 0=command indicator line) also called RS
//#define	ILI9341_RST_PIN		RESET			// <= RST pin (optional)
#define	ILI9341_MOSI_PIN	1			// <= MOSI pin (if not hardware SPI)
//#define	ILI9341_MISO_PIN	0			// <= MISO pin (if not hardware SPI)
#define	ILI9341_SCLK_PIN	2			// <= SCLK pin (if not hardware SPI)
// (with hardware SPI some pins dictated by AVR PIN function - as shown above)

// other PDQ library options
#define	ILI9341_SAVE_SPCR	0			// <= 0/1 with 1 to save/restore AVR SPI control register (to "play nice" when other SPI use)
