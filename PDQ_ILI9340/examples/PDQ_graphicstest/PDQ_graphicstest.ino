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
 
#include "SPI.h"				// must include this here (or else IDE can't find it)
                                           
//#include <PDQ_GFX.h>				// PDQ: Core graphics library
//#include "PDQ_ILI9340_config.h"			// PDQ: ST7735 pins and other setup for this sketch
//#include <PDQ_ILI9340.h>			// PDQ: Hardware-specific driver library
//PDQ_ILI9340 tft;				// PDQ: create LCD object (using pins in "PDQ_ST7735_config.h")

// NOTE: Changes to test with Adafruit libraries (comment out PDQ lines above and un-comment the AF: ones below)

// AF:
 #include <Adafruit_GFX.h>		// AF: Core graphics library
// AF:
 #include <Adafruit_ILI9341.h>		// AF: Hardware-specific library
// AF:
 Adafruit_ILI9341 tft(10,  8, 9); // AF: create LCD object (HW SPI, CS=pin 10, D/C=pin 8, reset=9)
// AF:
 #define color565 Color565

extern "C" char __data_start[];		// start of SRAM data
extern "C" char _end[];			// end of SRAM data (used to check amount of SRAM this program's variables uses)
extern "C" char __data_load_end[];	// end of FLASH (used to check amount of Flash this program and static data uses)

void setup()
{
	Serial.begin(115200);
	while (!Serial);
	
//	Serial.println("PDQ ILI9340 2.2\" SPI TFT Test!"); 
// AF:
 	Serial.println("Adafruit ILI9340 2.2\" SPI TFT Test!"); 
 
#if defined(ILI9340_RST_PIN)
	FastPin<ILI9340_RST_PIN>::setOutput();
	FastPin<ILI9340_RST_PIN>::hi();
	FastPin<ILI9340_RST_PIN>::lo();
	delay(1);
	FastPin<ILI9340_RST_PIN>::hi();
#endif

	tft.begin();
	SPI.setClockDivider(SPI_CLOCK_DIV2);	// 8 MHz (full! speed!) [1 byte every 18 cycles]
}

void loop(void)
{
	Serial.print(F(__DATE__ " " __TIME__ " - Flash=0x"));
	Serial.print((uint16_t)__data_load_end, HEX);
	Serial.print(F(" RAM=0x"));
	Serial.println((uint16_t)_end - (uint16_t)__data_start, HEX);
	Serial.println(F("Benchmark                Time (microseconds)"));

	uint32_t usecHV = testHV(0x0000, 0xffff);
	Serial.print(F("HV Lines                 "));
	Serial.println(usecHV);
	delay(100);

	uint32_t usecFillScreen = testFillScreen();
	Serial.print(F("Screen fill              "));
	Serial.println(usecFillScreen);
	delay(100);

	uint32_t usecText = testText();
	Serial.print(F("Text                     "));
	Serial.println(usecText);
	delay(100);

	uint32_t usecPixels = testPixels();
	Serial.print(F("Pixels                   "));
	Serial.println(usecPixels);
	delay(100);

	uint32_t usecLines = testLines(ILI9340_BLUE);
	Serial.print(F("Lines                    "));
	Serial.println(usecLines);
	delay(100);

	uint32_t usecFastLines = testFastLines(ILI9340_RED, ILI9340_BLUE);
	Serial.print(F("Horiz/Vert Lines         "));
	Serial.println(usecFastLines);
	delay(100);

	uint32_t usecRects = testRects(ILI9340_GREEN);
	Serial.print(F("Rectangles (outline)     "));
	Serial.println(usecRects);
	delay(100);

	uint32_t usecFilledRects = testFilledRects(ILI9340_YELLOW, ILI9340_MAGENTA);
	Serial.print(F("Rectangles (filled)      "));
	Serial.println(usecFilledRects);
	delay(100);

	uint32_t usecFilledCircles = testFilledCircles(10, ILI9340_MAGENTA);
	Serial.print(F("Circles (filled)         "));
	Serial.println(usecFilledCircles);
	delay(100);

	uint32_t usecCircles = testCircles(10, ILI9340_WHITE);
	Serial.print(F("Circles (outline)        "));
	Serial.println(usecCircles);
	delay(100);

	uint32_t usecTriangles = testTriangles();
	Serial.print(F("Triangles (outline)      "));
	Serial.println(usecTriangles);
	delay(100);

	uint32_t usecFilledTrangles = testFilledTriangles();
	Serial.print(F("Triangles (filled)       "));
	Serial.println(usecFilledTrangles);
	delay(100);

	uint32_t usecRoundRects = testRoundRects();
	Serial.print(F("Rounded rects (outline)  "));
	Serial.println(usecRoundRects);
	delay(100);

	uint32_t usedFilledRoundRects = testFilledRoundRects();
	Serial.print(F("Rounded rects (filled)   "));
	Serial.println(usedFilledRoundRects);
	delay(100);

	Serial.println(F("Done!"));
	
	uint16_t c = 0;
	for (int32_t i = 0; i < tft.height(); i++)
	{
		tft.drawLine(0, i, tft.width()-1, i, c);
		c += 1;
		c &= tft.color565(0x3f, 0x3f, 0x3f);
	}
	
	tft.setCursor(0, 0);
	tft.setTextColor(ILI9340_MAGENTA);
	tft.setTextSize(2);

//	tft.println(F("PDQ GFX+PDQ ILI934x"));
// AF:
 	tft.print(F("Adafruit GFX+ILI934x"));
	tft.setTextSize(1);
	tft.setTextColor(ILI9340_WHITE);
	tft.println(F(" SPI LCD Benchmark (Atmega328p @ 16MHz)"));
	tft.setTextSize(1);
	tft.println(F(""));
	tft.print(F("  Memory Used:  Flash=0x"));
	tft.print((uint16_t)__data_load_end, HEX);
	tft.print(F(" RAM=0x"));
	tft.println((uint16_t)_end - (uint16_t)__data_start, HEX);
	tft.println(F(""));

	tft.setTextColor(ILI9340_GREEN);
	tft.println(F("Benchmark                   usec"));
	tft.println(F(""));
	tft.setTextColor(ILI9340_YELLOW);

	tft.setTextColor(ILI9340_CYAN); tft.setTextSize(1);
	tft.print(F("HV Lines              "));
	tft.setTextColor(ILI9340_YELLOW); tft.setTextSize(2);
	printnice(usecHV);

	tft.setTextColor(ILI9340_CYAN); tft.setTextSize(1);
	tft.print(F("Screen fill           "));
	tft.setTextColor(ILI9340_YELLOW); tft.setTextSize(2);
	printnice(usecFillScreen);

	tft.setTextColor(ILI9340_CYAN); tft.setTextSize(1);
	tft.print(F("Text                  "));
	tft.setTextColor(ILI9340_YELLOW); tft.setTextSize(2);
	printnice(usecText);

	tft.setTextColor(ILI9340_CYAN); tft.setTextSize(1);
	tft.print(F("Pixels                "));
	tft.setTextColor(ILI9340_YELLOW); tft.setTextSize(2);
	printnice(usecPixels);

	tft.setTextColor(ILI9340_CYAN); tft.setTextSize(1);
	tft.print(F("Lines                 "));
	tft.setTextColor(ILI9340_YELLOW); tft.setTextSize(2);
	printnice(usecLines);

	tft.setTextColor(ILI9340_CYAN); tft.setTextSize(1);
	tft.print(F("Horiz/Vert Lines      "));
	tft.setTextColor(ILI9340_YELLOW); tft.setTextSize(2);
	printnice(usecFastLines);

	tft.setTextColor(ILI9340_CYAN); tft.setTextSize(1);
	tft.print(F("Rectangles            "));
	tft.setTextColor(ILI9340_YELLOW); tft.setTextSize(2);
	printnice(usecRects);

	tft.setTextColor(ILI9340_CYAN); tft.setTextSize(1);
	tft.print(F("Rectangles (filled)   "));
	tft.setTextColor(ILI9340_YELLOW); tft.setTextSize(2);
	printnice(usecFilledRects);

	tft.setTextColor(ILI9340_CYAN); tft.setTextSize(1);
	tft.print(F("Circles               "));
	tft.setTextColor(ILI9340_YELLOW); tft.setTextSize(2);
	printnice(usecCircles);

	tft.setTextColor(ILI9340_CYAN); tft.setTextSize(1);
	tft.print(F("Circles (filled)      "));
	tft.setTextColor(ILI9340_YELLOW); tft.setTextSize(2);
	printnice(usecFilledCircles);

	tft.setTextColor(ILI9340_CYAN); tft.setTextSize(1);
	tft.print(F("Triangles             "));
	tft.setTextColor(ILI9340_YELLOW); tft.setTextSize(2);
	printnice(usecTriangles);

	tft.setTextColor(ILI9340_CYAN); tft.setTextSize(1);
	tft.print(F("Triangles (filled)    "));
	tft.setTextColor(ILI9340_YELLOW); tft.setTextSize(2);
	printnice(usecFilledTrangles);

	tft.setTextColor(ILI9340_CYAN); tft.setTextSize(1);
	tft.print(F("Rounded rects         "));
	tft.setTextColor(ILI9340_YELLOW); tft.setTextSize(2);
	printnice(usecRoundRects);

	tft.setTextColor(ILI9340_CYAN); tft.setTextSize(1);
	tft.print(F("Rounded rects (filled)"));
	tft.setTextColor(ILI9340_YELLOW); tft.setTextSize(2);
	printnice(usedFilledRoundRects);

	tft.setTextColor(ILI9340_GREEN); tft.setTextSize(2);
	tft.println(F(""));
	tft.print(F("Benchmark Completed!"));

	delay(60 * 1000);
}

void printnice(int32_t v)
{
	char	str[32];
	sprintf(str, "%8ld", v);
	tft.println(str);
}

static inline uint32_t micros_start() __attribute__ ((always_inline));
static inline uint32_t micros_start()
{
	uint8_t oms = millis();
	while ((uint8_t)millis() == oms)
		;
	return micros();
}

uint32_t testFillScreen()
{
	uint32_t start = micros_start();

	tft.fillScreen(ILI9340_BLACK);
	tft.fillScreen(ILI9340_RED);
	tft.fillScreen(ILI9340_GREEN);
	tft.fillScreen(ILI9340_BLUE);
	tft.fillScreen(ILI9340_BLACK);

	return micros() - start;
}

uint32_t testText() {
	tft.fillScreen(ILI9340_BLACK);
	uint32_t start = micros_start();
	tft.setCursor(0, 0);
	tft.setTextColor(ILI9340_WHITE);	tft.setTextSize(1);
	tft.println(F("Hello World!"));
	tft.setTextSize(2);
	tft.setTextColor(tft.color565(0xff, 0x00, 0x00));
	tft.print(F("RED "));
	tft.setTextColor(tft.color565(0x00, 0xff, 0x00));
	tft.print(F("GREEN "));
	tft.setTextColor(tft.color565(0x00, 0x00, 0xff));
	tft.println(F("BLUE"));
	tft.setTextColor(ILI9340_YELLOW); tft.setTextSize(2);
	tft.println(1234.56);
	tft.setTextColor(ILI9340_RED);		tft.setTextSize(3);
	tft.println(0xDEADBEEF, HEX);
	tft.println();
	tft.setTextColor(ILI9340_GREEN);
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
	tft.println(F(""));
	tft.println(F(""));
	tft.setTextColor(ILI9340_MAGENTA);
	tft.println(F("Happy 10th anniversary Hackaday! Woot!"));
	uint32_t t = micros() - start;
	delay(1000);
	return t;
}

uint32_t testPixels()
{
	int32_t	w = tft.width();
	int32_t	h = tft.height();

	uint32_t start = micros_start();

	for (uint16_t y = 0; y < h; y++)
	{
		for (uint16_t x = 0; x < w; x++)
		{
			tft.drawPixel(x, y, tft.color565(x<<3, y<<3, x*y));
		}
	}
	
	return micros() - start;
}


uint32_t testLines(uint16_t color)
{
	uint32_t start, t;
	int32_t	x1, y1, x2, y2;
	int32_t	w = tft.width();
	int32_t	h = tft.height();

	tft.fillScreen(ILI9340_BLACK);

	x1 = y1 = 0;
	y2 = h - 1;

	start = micros_start();

	for (x2 = 0; x2 < w; x2 += 6)
	{
//		tft.drawLine_(x1, y1, x2, y2, ILI9340_WHITE);
		tft.drawLine(x1, y1, x2, y2, color);
	}

	x2 = w - 1;

	for (y2 = 0; y2 < h; y2 += 6)
	{
//		tft.drawLine_(x1, y1, x2, y2, ILI9340_WHITE);
		tft.drawLine(x1, y1, x2, y2, color);
	}

	t = micros() - start; // fillScreen doesn't count against timing

//	delay(2000);
	tft.fillScreen(ILI9340_BLACK);

	x1 = w - 1;
	y1 = 0;
	y2 = h - 1;

	start = micros_start();

	for (x2 = 0; x2 < w; x2 += 6)
	{
//		tft.drawLine_(x1, y1, x2, y2, ILI9340_WHITE);
		tft.drawLine(x1, y1, x2, y2, color);
	}

	x2 = 0;
	for (y2 = 0; y2 < h; y2 += 6)
	{
//		tft.drawLine_(x1, y1, x2, y2, ILI9340_WHITE);
		tft.drawLine(x1, y1, x2, y2, color);
	}

	t += micros() - start;

//	delay(2000);
	tft.fillScreen(ILI9340_BLACK);

	x1 = 0;
	y1 = h - 1;
	y2 = 0;

	start = micros_start();

	for (x2 = 0; x2 < w; x2 += 6)
	{
//		tft.drawLine_(x1, y1, x2, y2, ILI9340_WHITE);
		tft.drawLine(x1, y1, x2, y2, color);
	}
	x2 = w - 1;
	for (y2 = 0; y2 < h; y2 += 6)
	{
//		tft.drawLine_(x1, y1, x2, y2, ILI9340_WHITE);
		tft.drawLine(x1, y1, x2, y2, color);
	}
	t += micros() - start;

//	delay(2000);
	tft.fillScreen(ILI9340_BLACK);

	x1 = w - 1;
	y1 = h - 1;
	y2 = 0;

	start = micros_start();

	for (x2 = 0; x2 < w; x2 += 6)
	{
//		tft.drawLine_(x1, y1, x2, y2, ILI9340_WHITE);
		tft.drawLine(x1, y1, x2, y2, color);
	}

	x2 = 0;
	for (y2 = 0; y2 < h; y2 += 6)
	{
//		tft.drawLine_(x1, y1, x2, y2, ILI9340_WHITE);
		tft.drawLine(x1, y1, x2, y2, color);
	}

	t += micros() - start;

//	delay(2000);

	return t;
}

uint32_t testHV(uint16_t color1, uint16_t color2)
{
	uint32_t start;
	int32_t x, y;
	int32_t w = tft.width();
	int32_t h = tft.height();

	tft.fillScreen(ILI9340_BLACK);

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


uint32_t testFastLines(uint16_t color1, uint16_t color2)
{
	uint32_t start;
	int32_t x, y;
	int32_t w = tft.width();
	int32_t h = tft.height();

	tft.fillScreen(ILI9340_BLACK);

	start = micros_start();

	for (y = 0; y < h; y += 5)
		tft.drawFastHLine(0, y, w, color1);
	for (x = 0; x < w; x += 5)
		tft.drawFastVLine(x, 0, h, color2);

	return micros() - start;
}

uint32_t testRects(uint16_t color)
{
	uint32_t start;
	int32_t n, i, i2;
	int32_t cx = tft.width() / 2;
	int32_t cy = tft.height() / 2;

	tft.fillScreen(ILI9340_BLACK);
	n = min(tft.width(), tft.height());
	start = micros_start();
	for (i = 2; i < n; i += 6)
	{
		i2 = i / 2;
		tft.drawRect(cx-i2, cy-i2, i, i, color);
	}

	return micros() - start;
}

uint32_t testFilledRects(uint16_t color1, uint16_t color2)
{
	uint32_t start, t = 0;
	int32_t n, i, i2;
	int32_t cx = tft.width() / 2 - 1;
	int32_t cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9340_BLACK);
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

uint32_t testFilledCircles(uint8_t radius, uint16_t color)
{
	uint32_t start;
	int32_t x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;

	tft.fillScreen(ILI9340_BLACK);

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

uint32_t testCircles(uint8_t radius, uint16_t color)
{
	uint32_t start;
	int32_t x, y, r2 = radius * 2;
	int32_t w = tft.width() + radius;
	int32_t h = tft.height() + radius;

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

uint32_t testTriangles()
{
	uint32_t start;
	int32_t n, i;
	int32_t cx = tft.width()/ 2 - 1;
	int32_t cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9340_BLACK);
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

uint32_t testFilledTriangles()
{
	uint32_t start, t = 0;
	int32_t i;
	int32_t cx = tft.width() / 2 - 1;
	int32_t cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9340_BLACK);

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

uint32_t testRoundRects()
 {
	uint32_t start;
	int32_t w, i, i2;
	int32_t cx = tft.width() / 2 - 1;
	int32_t cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9340_BLACK);
	
	w = min(tft.width(), tft.height());
	
	start = micros_start();

	for (i = 0; i < w; i += 6)
	{
		i2 = i / 2;
		tft.drawRoundRect(cx-i2, cy-i2, i, i, i/8, tft.color565(i, 0, 0));
	}

	return micros() - start;
}

uint32_t testFilledRoundRects()
{
	uint32_t start;
	int32_t i, i2;
	int32_t cx = tft.width() / 2 - 1;
	int32_t cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9340_BLACK);

	start = micros_start();

	for (i = min(tft.width(), tft.height()); i > 20; i -= 6)
	{
		i2 = i / 2;
		tft.fillRoundRect(cx-i2, cy-i2, i, i, i/8, tft.color565(0, i, 0));
	}

	return micros() - start;
}
