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
#define DATA_PIN 6    // Digital IO pin connected to the NeoPixels.
#define NUM_LEDS 60

int showCount = 10;

CRGB leds[NUM_LEDS];

bool oldState = HIGH;
int showType = 0;

void setup() {
  pinMode(13, OUTPUT); //debug blink
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUTTON_PIN2, INPUT_PULLUP);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  attachInterrupt(digitalPinToInterrupt(2), increment, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), decrement, FALLING);
}

void loop() {
  FastLED.clear();
  startShow(showType);
  FastLED.show(); 
}

void brightness() {
  int val = analogRead(3);
  int brightVal = map(val, 0, 1023, 0, 130);
  FastLED.setBrightness(brightVal);
}

void increment() {
	static unsigned long last_interrupt_time = 0;
 	unsigned long interrupt_time = millis();
 	// If interrupts come faster than 200ms, assume it's a bounce and ignore
 	if (interrupt_time - last_interrupt_time > 150) {
 		showType++;
    	if (showType > showCount)
    		showType=0;
 	}
 	last_interrupt_time = interrupt_time;
}

void decrement() {
	static unsigned long last_interrupt_time = 0;
 	unsigned long interrupt_time = millis();
 	// If interrupts come faster than 200ms, assume it's a bounce and ignore
 	if (interrupt_time - last_interrupt_time > 150) {
 		showType--;
    	if (showType < 0)
    		showType=showCount;
 	}
 	last_interrupt_time = interrupt_time;
}

void startShow(int i) {
  switch(i){
    case 0: for (int led = 0; led < NUM_LEDS; led++) {
              leds[led]= CRGB::Black;
            }
            break;
    case 1: setSplit(255, 0, 0, 0, 255, 0);
            break;
    case 2: setAll(255, 0, 0);
            break;
    case 3: setAll(0, 255, 0);
            break;
    case 4: setAll(0, 0, 255);
            break;
    case 5: setAll(255, 0, 255);
            break;
    case 6: setAll(255, 255, 0);
            break;
    case 7: setAll(0, 255, 255);
            break;
    case 8: theaterChase(0,0xff,0,50);
            break;
    case 9: rainbowCycle(5);
            break;
    case 10: breath(5, 255, 0, 0, 50, 5000);
            break;
  }
}

void setPixel(int pixel, byte red, byte green, byte blue) {
  leds[pixel].r = red;
  leds[pixel].g = green;
  leds[pixel].b = blue;
}

void setSplit(byte leftR, byte leftG, byte leftB, byte rightR, byte rightG, byte rightB) {
  for (int led = 0; led < NUM_LEDS; led++) {
    if (led < NUM_LEDS/2) {
      leds[led]= CRGB(leftR, leftG, leftB);
    }
    else if (led > NUM_LEDS/2) {
      leds[led]= CRGB(rightR, rightG, rightB);
    }
  brightness();
  //FastLED.show();
  }
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue); 
  }
  brightness();
  FastLED.show();
}

void fadeIn (int spd, byte red, byte green, byte blue) {
  for (int k = 0; k < 255; k++) {
    setAll(red*k, green*k, blue*k);
    delay(spd);
  }
}

void fadeOut (int spd, byte red, byte green, byte blue) {
  for (int k = 255; k >= 0; k--) {
    setAll(red*k, green*k, blue*k);
    delay(spd);
  }
}

void breath (int spd, byte red, byte green, byte blue, int onHold, int offHold) {
  fadeIn(spd, red, green, blue);
  delay(onHold);
  fadeOut(spd, red, green, blue);
  delay(offHold);
}

//Theatre-style crawling lights.
void theaterChase(byte red, byte green, byte blue, int SpeedDelay) {
  for (int j=0; j<1; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < NUM_LEDS; i=i+3) {
        leds[i+q]= CRGB( red, green, blue);    //turn every third pixel on
      }
      brightness();
      FastLED.show(); 
      delay(SpeedDelay);
     
      for (int i=0; i < NUM_LEDS; i=i+3) {
        leds[i+q]= CRGB( 0, 0, 0);        //turn every third pixel off
      }
    }
  }
}

void rainbowCycle(int SpeedDelay) {
  byte *c;
  uint16_t i, j;

  for(j=0; j<256*1; j++) { // 1 cycles of all colors on wheel
    for(i=0; i< NUM_LEDS; i++) {
      c=Wheel(((i * 256 / NUM_LEDS) + j) & 255);
      leds[i]= CRGB( *c, *(c+1), *(c+2));
    }
    brightness();
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
