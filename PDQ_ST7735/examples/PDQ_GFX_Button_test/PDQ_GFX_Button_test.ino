// Simple test of PDQ_GFX_Button (based on Adafruit http://github.com/adafruit/Adafruit-GFX-Library)
// This is designed to use Arduino Uno (or equivilent)
// with Adafruit 2.8" TFT Touch Shield for Arduino w/Capacitive Touch (http://www.adafruit.com/products/1947)

#include <SPI.h>						// must include this here (or else IDE can't find it)
#include <Wire.h>						// must include this here (or else IDE can't find it)
#include <PDQ_GFX.h>
#include "PDQ_ST7735_config.h"			// PDQ: ST7735 pins and other setup for this sketch
#include <PDQ_ST7735.h>
#include <Adafruit_FT6206.h>			// I2C capacitive touch library from Adafruit

Adafruit_FT6206 ts = Adafruit_FT6206();	// touch screen library object
PDQ_ST7735 tft = PDQ_ST7735();		// PDQ_GFX lcd library object

#define NUM_BUTTONS	2
PDQ_GFX_Button	button[NUM_BUTTONS];	// array of buttons

int16_t	current_count = 0;				// test count

void setup(void)
{
	Serial.begin(9600);

	tft.begin();
	
	if (!ts.begin())
	{ 
		Serial.println(F("Unable to start touchscreen."));
	} 
	else
	{ 
		Serial.println(F("Touchscreen started.")); 
	}

	button[0].initButton(&tft, 60, 100, 72, 72, ST7735_RED, ST7735_BLACK, ST7735_WHITE, "Decrement", 1);
	button[1].initButton(&tft, 180, 100, 72, 72, ST7735_GREEN, ST7735_BLACK, ST7735_WHITE, "Increment", 1);
	
	tft.fillScreen(ST7735_BLUE);

	tft.setCursor(54, 10);
	tft.setTextColor(ST7735_WHITE, ST7735_BLUE);
	tft.setTextSize(1);
	tft.print(F("Simple GFX_Button Test"));
	
	for (uint8_t i = 0; i < NUM_BUTTONS; i++)
		button[i].drawButton(false);
}

void loop()
{
	// See if screen is being touched
	if (ts.touched())
	{	 
		// Retrieve touch point	
		TS_Point p = ts.getPoint(); 
	
		// reverse coordinates to match orientation of LCD
		int x = tft.width() - p.x;
		int y = tft.height() - p.y;
		
//		tft.drawPixel(x, y, 0xffff);	// touch debugging
		
		// loop through buttons and process touch coordinate
		for (uint8_t i = 0; i < NUM_BUTTONS; i++)
		{
			if (button[i].contains(x, y))
			{
				button[i].press(true);
			}
			else
			{
				// if still touched, but outside of button area
				if (button[i].isPressed())
					button[i].drawButton(false);		// unhighlight
				button[i].press(false);					// set not pressed
				button[i].press(false);					// twice, because we don't want "justReleased" to return true
			}
		}
	}	
	else
	{
		// screen not touched, so all buttons not pressed
		for (uint8_t i = 0; i < NUM_BUTTONS; i++)
			button[i].press(false);
	}
	
	// draw button and perform action if button was just released
	for (uint8_t i = 0; i < NUM_BUTTONS; i++)
	{
		if (button[i].justPressed())
		{
			button[i].drawButton(true);
		}
		else if (button[i].justReleased())
		{
			button[i].drawButton(false);

			// perform button action (decrement or increment in this simple test)
			if (i == 0)
				current_count -= 1;
			else if (i == 1)
				current_count += 1;
		}
	}
	
	// draw the current count
	tft.setCursor(60, 200);
	tft.setTextColor(ST7735_WHITE, ST7735_BLUE);
	tft.setTextSize(2);
	tft.print("Count = ");
	tft.print(current_count);
	tft.print("  ");
	
	// make sure we don't go too fast for touch controller
	delay(1);
}
