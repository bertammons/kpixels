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
 
#define BUTTON_PIN   2    // Digital IO pin connected to the button.  This will be
                          // driven with a pull-up resistor so the switch should
                          // pull the pin to ground momentarily.  On a high -> low
#define BUTTON_PIN2   3    // transition the button press logic will execute.
//#define POT_PIN A3
#define PIXEL_PIN 6    // Digital IO pin connected to the NeoPixels.
#define PIXEL_COUNT 60

CRGB leds[PIXEL_COUNT];

bool oldState = HIGH;
int showType = 0;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUTTON_PIN2, INPUT_PULLUP);
  FastLED.addLeds<NEOPIXEL, PIXEL_PIN>(leds, PIXEL_COUNT);
  attachInterrupt(digitalPinToInterrupt(2), increment, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), decrement, FALLING);
}

void loop() {
  int val = analogRead(3);
  int brightVal = map(val, 0, 1023, 0, 130);
    
  FastLED.setBrightness(brightVal);
  
  FastLED.clear();

  startShow(showType);

  FastLED.show(); 
}

void increment() {
	static unsigned long last_interrupt_time = 0;
 	unsigned long interrupt_time = millis();
 	// If interrupts come faster than 200ms, assume it's a bounce and ignore
 	if (interrupt_time - last_interrupt_time > 100) {
 		showType++;
    	if (showType > 5)
    		showType=0;
 	}
 	last_interrupt_time = interrupt_time;
}

void decrement() {
	static unsigned long last_interrupt_time = 0;
 	unsigned long interrupt_time = millis();
 	// If interrupts come faster than 200ms, assume it's a bounce and ignore
 	if (interrupt_time - last_interrupt_time > 100) {
 		showType--;
    	if (showType < 0)
    		showType=5;
 	}
 	last_interrupt_time = interrupt_time;
}

void startShow(int i) {
  switch(i){
    case 0: for (int led = 0; led < PIXEL_COUNT; led++) {
              leds[led]= CRGB::Black;
            }
            break;
    case 1: for (int led = 0; led < PIXEL_COUNT; led++) {
              if (led < PIXEL_COUNT/2) {
                leds[led]= CRGB::Red;
              }
              else if (led > PIXEL_COUNT/2) {
                leds[led]= CRGB::Green;
              }
    }
            break;
    case 2: for (int led = 0; led < PIXEL_COUNT; led++) {
              leds[led]= CRGB::Purple;  // Purple
    }
            break;
    case 3: for (int led = 0; led < PIXEL_COUNT; led++) {
              leds[led]= CRGB::Blue;  // Blue
    }
            break;
    case 4: theaterChase(0,0xff,0,100);
            break;
    case 5: rainbowCycle(5);
            break;
  }
}

//Theatre-style crawling lights.
void theaterChase(byte red, byte green, byte blue, int SpeedDelay) {
  for (int j=0; j<1; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < PIXEL_COUNT; i=i+3) {
        leds[i+q]= CRGB( red, green, blue);    //turn every third pixel on
      }
      FastLED.show(); 
     
      delay(SpeedDelay);
     
      for (int i=0; i < PIXEL_COUNT; i=i+3) {
        leds[i+q]= CRGB( 0, 0, 0);        //turn every third pixel off
      }
    }
  }
}

void rainbowCycle(int SpeedDelay) {
  byte *c;
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< PIXEL_COUNT; i++) {
      c=Wheel(((i * 256 / PIXEL_COUNT) + j) & 255);
      leds[i]= CRGB( *c, *(c+1), *(c+2));
    }
    FastLED.show();
    delay(SpeedDelay);
  }
}

byte * Wheel(byte WheelPos) {
  static byte c[3];
  
  if(WheelPos < 85) {
    c[0]=WheelPos * 3;
    c[1]=255 - WheelPos * 3;
    c[2]=0;
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    c[0]=255 - WheelPos * 3;
    c[1]=0;
    c[2]=WheelPos * 3;
  } 
  else {
    WheelPos -= 170;
    c[0]=0;
    c[1]=WheelPos * 3;
    c[2]=255 - WheelPos * 3;
  }
  return c;
}
