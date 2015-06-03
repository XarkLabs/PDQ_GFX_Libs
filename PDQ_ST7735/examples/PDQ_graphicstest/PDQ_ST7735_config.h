//
// PDQ_ST7735 configuration
//
// You need to include this file above #include "PDQ_ILI9340.h" in your sketch.
// Check settings on lines marked with "<=".

// ST7735 has several variations, set your version based on this list (using the color of the "tab" on the screen cover).
// NOTE: The tab colors refer to Adafruit versions, other suppliers may vary (you may have to experiment to find the right one).
enum
{
	ST7735_INITB			= 0,				// 1.8" (128x160) ST7735B chipset (only one type)
	ST7735_INITR_GREENTAB		= 1,				// 1.8" (128x160) ST7735R chipset with green tab (same as ST7735_INITR_18GREENTAB)
	ST7735_INITR_REDTAB		= 2,				// 1.8" (128x160) ST7735R chipset with red tab (same as ST7735_INITR_18REDTAB)
	ST7735_INITR_BLACKTAB		= 3,				// 1.8" (128x160) ST7735S chipset with black tab (same as ST7735_INITR_18BLACKTAB)
	ST7735_INITR_144GREENTAB   	= 4,				// 1.4" (128x128) ST7735R chipset with green tab
	ST7735_INITR_18GREENTAB		= ST7735_INITR_GREENTAB,	// 1.8" (128x160) ST7735R chipset with green tab
	ST7735_INITR_18REDTAB		= ST7735_INITR_REDTAB,		// 1.8" (128x160) ST7735R chipset with red tab
	ST7735_INITR_18BLACKTAB		= ST7735_INITR_BLACKTAB,	// 1.8" (128x160) ST7735S chipset with black tab
};

#define ST7735_CHIPSET		ST7735_INITR_BLACKTAB	// <= Set ST7735 LCD chipset/variation here (from above list)

// NOTE: These are typical hookups individual boards will vary, please check your documentation.
// CAUTION: While Adafruit boards generally always come with needed level-converters, I find many
//          other LCD displays advertised as supporting 5V only support 5V power (with a regulator).
//          They still only have 3.3V safe logic (CS, DC, RESET, MOSI, SCK marked with * below).
//          If this is the case you will need a voltage level-converter (e.g., HC4050, divider circuit etc.).
//
// LCD PIN	Uno (328)	Leo (32u4)  644/1284
// -------	---------	---------- --------
// 1  VCC	  3.3V/5V	 3.3V/5V	3.3V/5V	// +3.3V or 5V with on-board regulator
// 2  GND	    GND		   GND		  GND
// 3* CS	    10		   10		   4	// Could be any GPIO pin, but then need to make sure SS isn't a LOW input (or slave SPI mode)
// 4* RESET	  3.3V/5V	 3.3V/5V	3.3V/5V	// This relies on soft-reset. You can also use Arduino reset pin (if correct voltage).
// 5* DC/RS		9  		   9		   3	// Could be any GPIO pin
// 6* SDI/MOSI	11		  ICSP4		   5	// HW SPI pin (can't change)
// 7* SCK	    13		  ICSP3		   7	// HW SPI pin (can't change) NOTE: On Uno this causes on-board LED to flicker during SPI use
// 8* LED	 3.3V/5V	 3.3V/5V	3.3V/5V	// LCD screen blanked when LOW (could use GPIO for PWM dimming)
// 9  SDO/MISO      -		    -	   -	// (not used if present, LCD code is currently "write only")
//
//  * = Typically only 3.3V safe logic-line (unless board has level converter [ala Adafruit]). Be careful with 5V!

#define	ST7735_CS_PIN		10			// <= /CS pin (chip-select, LOW to get attention of ST7735, HIGH and it ignores SPI bus)
#define	ST7735_DC_PIN		9			// <= DC pin (1=data or 0=command indicator line) also called RS
// 644/1284 #define	ST7735_CS_PIN		4			// <= /CS pin (chip-select, LOW to get attention of ST7735, HIGH and it ignores SPI bus)
// 644/1284	#define	ST7735_DC_PIN		3			// <= DC pin (1=data or 0=command indicator line) also called RS
// (other pins used are dictated by AVR HW SPI used as shown above)

// other PDQ library options
#define	ST7735_SAVE_SPCR	0			// <= 0/1 with 1 to save/restore AVR SPI control register (to "play nice" when other SPI use)
