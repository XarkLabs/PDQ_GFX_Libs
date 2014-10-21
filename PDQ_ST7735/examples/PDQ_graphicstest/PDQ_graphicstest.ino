// This is part of the PDQ re-mixed version of Adafruit's GFX library
// and associated chipset drivers.
// Here is the libraries original copyright notice:

/***************************************************
  This is an example sketch for the Adafruit 2.2" SPI display.
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

#include <SPI.h>			// must include this here (or else IDE can't find it)

#include <PDQ_GFX.h>			// PDQ: Core graphics library
#include "PDQ_ST7735_config.h"		// PDQ: ST7735 pins and other setup for this sketch
#include <PDQ_ST7735.h>			// PDQ: Hardware-specific driver library
PDQ_ST7735 tft = PDQ_ST7735();// PDQ: create LCD object (using pins in "PDQ_ST7735_config.h")

// NOTE: Changes to test with Adafruit libraries (comment out PDQ lines above and un-comment the AF: ones below)

// AF: #include <Adafruit_GFX.h>	// AF: Core graphics library
// AF: #include <Adafruit_ST7735.h>	// AF: Hardware-specific library
// AF: Adafruit_ST7735 tft = Adafruit_ST7735(10,  9, 0); // AF: create LCD object (HW SPI, CS=pin 10, D/C=pin 9, reset=Arduino reset)
// AF: #define color565 Color565	// AF: use consistent naming (same as other Adafruit drivers) needed for Adafruit libs

// These are used to get information about static SRAM and flash memory sizes
extern "C" char __data_start[];		// start of SRAM data
extern "C" char _end[];			// end of SRAM data (used to check amount of SRAM used)
extern "C" char __data_load_end[];	// end of FLASH (used to check amount of flash used)
void setup()
{
	Serial.begin(115200);
	while (!Serial);

	tft.begin();			// PDQ: initialize SPI/LCD
// AF:	tft.initR(INITR_18BLACKTAB);	// AF: initialize SPI/LCD
	
	Serial.print(F("PDQ re-mix "));

#if 0	
	if (ST7735_CHIPSET == ST7735_INITB)
		Serial.print(F("ST3775B 1.44"));
	else if (ST7735_CHIPSET == ST7735_INITR_18GREENTAB)
		Serial.print(F("ST3775R (green-tab) 1.8"));
	else if (ST7735_CHIPSET == ST7735_INITR_18REDTAB)
#endif
		Serial.print(F("ST3775R (red-tab) 1.8"));
#if 0
	else if (ST7735_CHIPSET == ST7735_INITR_18BLACKTAB)
		Serial.print(F("ST3775S (black-tab) 1.8"));
	else if (ST7735_CHIPSET == ST7735_INITR_144GREENTAB)
		Serial.print(F("ST3775R (green-tab) 1.44"));
#endif
	Serial.print(F("\" "));
	Serial.print(tft.width());
	Serial.print(F("x"));
	Serial.print(tft.height());
	Serial.println(F(" SPI TFT Test!")); 
}

void loop(void)
{
	Serial.print(F(__DATE__ " " __TIME__ " - Flash="));
	Serial.print((uint16_t)__data_load_end);
	Serial.print(F(" RAM="));
	Serial.println((uint16_t)_end - (uint16_t)__data_start);
	Serial.println(F("Benchmark                Time (microseconds)"));

#if 1
	Serial.print(F("HV Lines                 "));
	unsigned long t = testHV(0x5555, 0xAAAA);
	Serial.println(t);
	tft.setTextColor(ST7735_WHITE);
	tft.setTextSize(2);
	tft.setCursor((tft.width()-(6 * 2 * 8))/2, (tft.height()-(8*2))/2);
	tft.println(t);
	delay(2000);

#endif
	


	Serial.print(F("Screen fill              "));
	Serial.println(testFillScreen());
	delay(100);

	Serial.print(F("Text                     "));
	Serial.println(testText());
	delay(100);
#if 1
	Serial.print(F("Pixels                   "));
	Serial.println(testPixels());
	delay(100);
#endif	
	Serial.print(F("Lines                    "));
	Serial.println(testLines(ST7735_BLUE));
	delay(100);

	Serial.print(F("Horiz/Vert Lines         "));
	Serial.println(testFastLines(ST7735_RED, ST7735_BLUE));
	delay(100);

	Serial.print(F("Rectangles (outline)     "));
	Serial.println(testRects(ST7735_GREEN));
	delay(100);

	Serial.print(F("Rectangles (filled)      "));
	Serial.println(testFilledRects(ST7735_YELLOW, ST7735_MAGENTA));
	delay(100);

	Serial.print(F("Circles (filled)         "));
	Serial.println(testFilledCircles(10, ST7735_MAGENTA));
	delay(100);

	Serial.print(F("Circles (outline)        "));
	Serial.println(testCircles(10, ST7735_WHITE));
	delay(100);

	Serial.print(F("Triangles (outline)      "));
	Serial.println(testTriangles());
	delay(100);

	Serial.print(F("Triangles (filled)       "));
	Serial.println(testFilledTriangles());
	delay(100);

	Serial.print(F("Rounded rects (outline)  "));
	Serial.println(testRoundRects());
	delay(100);

	Serial.print(F("Rounded rects (filled)   "));
	Serial.println(testFilledRoundRects());

	Serial.println(F("Done!"));
	
	tft.setTextSize(3);
	uint16_t c = 0;
	for (int i = 0; i < 1000; i++)
	{
		tft.setTextColor(c);
		tft.setCursor((tft.width()-(6 * 3 * 5))/2, (tft.height()-(8*3))/2);
		tft.println(F("Done!"));
		c += 13;
		delay(1);
	}
	delay(100);
}

static inline unsigned long micros_start() __attribute__ ((always_inline));
static inline unsigned long micros_start()
{
	uint8_t oms = millis();
	while ((uint8_t)millis() == oms)
		;
	return micros();
}

unsigned long testFillScreen()
{
	unsigned long start = micros_start();

	tft.fillScreen(ST7735_BLACK);
	tft.fillScreen(ST7735_RED);
	tft.fillScreen(ST7735_GREEN);
	tft.fillScreen(ST7735_BLUE);
	tft.fillScreen(ST7735_BLACK);

	return micros() - start;
}

unsigned long testText() {
	tft.fillScreen(ST7735_BLACK);
	unsigned long start = micros_start();
	tft.setCursor(0, 0);
	tft.setTextColor(ST7735_WHITE);	tft.setTextSize(1);
	tft.println(F("Hello World!"));
	tft.setTextSize(2);
	tft.setTextColor(tft.color565(0xff, 0x00, 0x00));
	tft.print(F("RED "));
	tft.setTextColor(tft.color565(0x00, 0xff, 0x00));
	tft.print(F("GREEN "));
	tft.setTextColor(tft.color565(0x00, 0x00, 0xff));
	tft.println(F("BLUE"));
	tft.setTextColor(ST7735_YELLOW); tft.setTextSize(2);
	tft.println(1234.56);
	tft.setTextColor(ST7735_RED);		tft.setTextSize(3);
	tft.println(0xDEADBEEF, HEX);
	tft.println();
	tft.setTextColor(ST7735_GREEN);
	tft.setTextSize(5);
	tft.println(F("Groop"));
	tft.setTextSize(2);
	tft.println(F("I implore thee,"));
	tft.setTextSize(1);
	tft.println(F("my foonting turlingdromes."));
	tft.println(F("And hooptiously drangle me"));
	tft.println(F("with crinkly bindlewurdles,"));
	tft.println(F("Or I will rend thee"));
	tft.println(F("in the gobberwarts"));
	tft.println(F("with my blurglecruncheon,"));
	tft.println(F("see if I don't!"));
	unsigned long t = micros() - start;
	delay(5000);
	return t;
}

unsigned long testPixels()
{
	int	w = tft.width();
	int	h = tft.height();

	unsigned long start = micros_start();

	for (uint16_t y = 0; y < h; y++)
	{
		for (uint16_t x = 0; x < w; x++)
		{
			tft.drawPixel(x, y, tft.color565(x<<3, y<<3, x*y));
		}
	}
	
	return micros() - start;
}


unsigned long testLines(uint16_t color)
{
	unsigned long start, t;
	int	x1, y1, x2, y2;
	int	w = tft.width();
	int	h = tft.height();

	tft.fillScreen(ST7735_BLACK);

	x1 = y1 = 0;
	y2 = h - 1;

	start = micros_start();

	for (x2 = 0; x2 < w; x2 += 6)
	{
//		tft.drawLine_(x1, y1, x2, y2, ST7735_WHITE);
		tft.drawLine(x1, y1, x2, y2, color);
	}

	x2 = w - 1;

	for (y2 = 0; y2 < h; y2 += 6)
	{
//		tft.drawLine_(x1, y1, x2, y2, ST7735_WHITE);
		tft.drawLine(x1, y1, x2, y2, color);
	}

	t = micros() - start; // fillScreen doesn't count against timing

//	delay(2000);
	tft.fillScreen(ST7735_BLACK);

	x1 = w - 1;
	y1 = 0;
	y2 = h - 1;

	start = micros_start();

	for (x2 = 0; x2 < w; x2 += 6)
	{
//		tft.drawLine_(x1, y1, x2, y2, ST7735_WHITE);
		tft.drawLine(x1, y1, x2, y2, color);
	}

	x2 = 0;
	for (y2 = 0; y2 < h; y2 += 6)
	{
//		tft.drawLine_(x1, y1, x2, y2, ST7735_WHITE);
		tft.drawLine(x1, y1, x2, y2, color);
	}

	t += micros() - start;

//	delay(2000);
	tft.fillScreen(ST7735_BLACK);

	x1 = 0;
	y1 = h - 1;
	y2 = 0;

	start = micros_start();

	for (x2 = 0; x2 < w; x2 += 6)
	{
//		tft.drawLine_(x1, y1, x2, y2, ST7735_WHITE);
		tft.drawLine(x1, y1, x2, y2, color);
	}
	x2 = w - 1;
	for (y2 = 0; y2 < h; y2 += 6)
	{
//		tft.drawLine_(x1, y1, x2, y2, ST7735_WHITE);
		tft.drawLine(x1, y1, x2, y2, color);
	}
	t += micros() - start;

//	delay(2000);
	tft.fillScreen(ST7735_BLACK);

	x1 = w - 1;
	y1 = h - 1;
	y2 = 0;

	start = micros_start();

	for (x2 = 0; x2 < w; x2 += 6)
	{
//		tft.drawLine_(x1, y1, x2, y2, ST7735_WHITE);
		tft.drawLine(x1, y1, x2, y2, color);
	}

	x2 = 0;
	for (y2 = 0; y2 < h; y2 += 6)
	{
//		tft.drawLine_(x1, y1, x2, y2, ST7735_WHITE);
		tft.drawLine(x1, y1, x2, y2, color);
	}

	t += micros() - start;

//	delay(2000);

	return t;
}

unsigned long testHV(uint16_t color1, uint16_t color2)
{
	unsigned long start;
	int x, y;
	int w = tft.width();
	int h = tft.height();

	tft.fillScreen(ST7735_BLACK);

	start = micros_start();

	for (y = 0; y < h; y ++)
		tft.drawFastHLine(0, y, w, color1);
	for (y = 0; y < h; y ++)
		tft.drawFastHLine(0, y, w, color2);
	for (y = 0; y < h; y ++)
		tft.drawFastHLine(0, y, w, color1);
	for (y = 0; y < h; y ++)
		tft.drawFastHLine(0, y, w, color2);
	for (y = 0; y < h; y ++)
		tft.drawFastHLine(0, y, w, color1);
	for (y = 0; y < h; y ++)
		tft.drawFastHLine(0, y, w, color2);
	for (x = 0; x < w; x ++)
		tft.drawFastVLine(x, 0, h, color1);
	for (x = 0; x < w; x ++)
		tft.drawFastVLine(x, 0, h, color2);
	for (x = 0; x < w; x ++)
		tft.drawFastVLine(x, 0, h, color1);
	for (x = 0; x < w; x ++)
		tft.drawFastVLine(x, 0, h, color2);
	for (x = 0; x < w; x ++)
		tft.drawFastVLine(x, 0, h, color1);
	for (x = 0; x < w; x ++)
		tft.drawFastVLine(x, 0, h, color2);

	return micros() - start;
}


unsigned long testFastLines(uint16_t color1, uint16_t color2)
{
	unsigned long start;
	int x, y;
	int w = tft.width();
	int h = tft.height();

	tft.fillScreen(ST7735_BLACK);

	start = micros_start();

	for (y = 0; y < h; y += 5)
		tft.drawFastHLine(0, y, w, color1);
	for (x = 0; x < w; x += 5)
		tft.drawFastVLine(x, 0, h, color2);

	return micros() - start;
}

unsigned long testRects(uint16_t color)
{
	unsigned long start;
	int n, i, i2;
	int cx = tft.width() / 2;
	int cy = tft.height() / 2;

	tft.fillScreen(ST7735_BLACK);
	n = min(tft.width(), tft.height());
	start = micros_start();
	for (i = 2; i < n; i += 6)
	{
		i2 = i / 2;
		tft.drawRect(cx-i2, cy-i2, i, i, color);
	}

	return micros() - start;
}

unsigned long testFilledRects(uint16_t color1, uint16_t color2)
{
	unsigned long start, t = 0;
	int n, i, i2;
	int cx = tft.width() / 2 - 1;
	int cy = tft.height() / 2 - 1;

	tft.fillScreen(ST7735_BLACK);
	n = min(tft.width(), tft.height());
	for (i = n; i > 0; i -= 6)
	{
		i2 = i / 2;

		start = micros_start();

		tft.fillRect(cx-i2, cy-i2, i, i, color1);

		t += micros() - start;

		// Outlines are not included in timing results
		tft.drawRect(cx-i2, cy-i2, i, i, color2);
	}

	return t;
}

unsigned long testFilledCircles(uint8_t radius, uint16_t color)
{
	unsigned long start;
	int x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;

	tft.fillScreen(ST7735_BLACK);

	start = micros_start();

	for (x = radius; x < w; x += r2)
	{
		for (y = radius; y < h; y += r2)
		{
			tft.fillCircle(x, y, radius, color);
		}
	}

	return micros() - start;
}

unsigned long testCircles(uint8_t radius, uint16_t color)
{
	unsigned long start;
	int x, y, r2 = radius * 2;
	int w = tft.width() + radius;
	int h = tft.height() + radius;

	// Screen is not cleared for this one -- this is
	// intentional and does not affect the reported time.
	start = micros_start();

	for (x = 0; x < w; x += r2)
	{
		for (y = 0; y < h; y += r2)
		{
			tft.drawCircle(x, y, radius, color);
		}
	}

	return micros() - start;
}

unsigned long testTriangles()
{
	unsigned long start;
	int n, i;
	int cx = tft.width()/ 2 - 1;
	int cy = tft.height() / 2 - 1;

	tft.fillScreen(ST7735_BLACK);
	n = min(cx, cy);

	start = micros_start();

	for (i = 0; i < n; i += 5)
	{
		tft.drawTriangle(
			cx		, cy - i, // peak
			cx - i, cy + i, // bottom left
			cx + i, cy + i, // bottom right
			tft.color565(0, 0, i));
	}

	return micros() - start;
}

unsigned long testFilledTriangles()
{
	unsigned long start, t = 0;
	int i;
	int cx = tft.width() / 2 - 1;
	int cy = tft.height() / 2 - 1;

	tft.fillScreen(ST7735_BLACK);

	start = micros_start();

	for (i = min(cx,cy); i > 10; i -= 5) {
		start = micros_start();
		tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
			tft.color565(0, i, i));
		t += micros() - start;
		tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
			tft.color565(i, i, 0));
	}

	return t;
}

unsigned long testRoundRects()
 {
	unsigned long start;
	int w, i, i2;
	int cx = tft.width() / 2 - 1;
	int cy = tft.height() / 2 - 1;

	tft.fillScreen(ST7735_BLACK);
	
	w = min(tft.width(), tft.height());
	
	start = micros_start();

	for (i = 0; i < w; i += 6)
	{
		i2 = i / 2;
		tft.drawRoundRect(cx-i2, cy-i2, i, i, i/8, tft.color565(i, 0, 0));
	}

	return micros() - start;
}

unsigned long testFilledRoundRects()
{
	unsigned long start;
	int i, i2;
	int cx = tft.width() / 2 - 1;
	int cy = tft.height() / 2 - 1;

	tft.fillScreen(ST7735_BLACK);

	start = micros_start();

	for (i = min(tft.width(), tft.height()); i > 20; i -= 6)
	{
		i2 = i / 2;
		tft.fillRoundRect(cx-i2, cy-i2, i, i, i/8, tft.color565(0, i, 0));
	}

	return micros() - start;
}
