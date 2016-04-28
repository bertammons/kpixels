#include <bitswap.h>
#include <chipsets.h>
#include <color.h>
#include <colorpalettes.h>
#include <colorutils.h>
#include <controller.h>
#include <cpp_compat.h>
#include <dmx.h>
#include <FastLED.h>
#include <fastled_config.h>
#include <fastled_delay.h>
#include <fastled_progmem.h>
#include <fastpin.h>
#include <fastspi.h>
#include <fastspi_bitbang.h>
#include <fastspi_dma.h>
#include <fastspi_nop.h>
#include <fastspi_ref.h>
#include <fastspi_types.h>
#include <hsv2rgb.h>
#include <led_sysdefs.h>
#include <lib8tion.h>
#include <noise.h>
#include <pixelset.h>
#include <pixeltypes.h>
#include <platforms.h>
#include <power_mgt.h>
 
#define NUM_LEDS 299
#define DATA_PIN 3
//#define POT_PIN A3
#define BTN_PIN 21

CRGB leds[NUM_LEDS];

void setup() {
	FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}

void loop() { 
  int val = analogRead(3);
	int brightVal = map(val, 0, 1023, 0, 130);

	FastLED.setBrightness(brightVal);
	
	FastLED.clear();
	for (int led = 0; led < NUM_LEDS; led++) {
		if (led < NUM_LEDS/2) {
		  leds[led]= CRGB::Red;
		}
		else if (led > NUM_LEDS/2) {
		  leds[led]= CRGB::Green;
		}
	}

    FastLED.show(); 
    delay(3); 
}
