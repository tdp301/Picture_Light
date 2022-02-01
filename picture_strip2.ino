/* tdp
    picture strip code

    Places a random color onto the strip, and cycles every 30 minutes
    Also listens on serial port for these commands followed by \n (enter):
      "!" led off
      "%" rainbow cycle
      "$,rrr,ggg,bbb," change to specific color
*/

#include <Adafruit_NeoPixel.h>

#define PIN 6
#define LED_COUNT 24

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_GRB + NEO_KHZ800);

//Serial buffer
char buffer[100];
int bufferIndex = 0;

void setup() {
  //setup color strip
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  //Setup serial
  Serial.begin(9600);

  randomSeed(analogRead(0));

  colorWipe(strip.Color(random(0,200), random(0,200), random(0,200)), 10);
}

int serialIdleCtr=0;

//Main Loop
void loop() {
  delay(500); //main loop delay

  Serial.print('A');  //keep alive signal

  while (Serial.available() > 0) 
  {
    serialIdleCtr=0;
    buffer[bufferIndex] = Serial.read();
    if (buffer[bufferIndex] == '\n')
    {
      //end of buffer
      if (buffer[0] == '$')
      {
        char *cptr;

        int red(-1);
        int green(-1);
        int blue(-1);

        cptr = strtok(buffer + 1, ",");
        if (cptr != NULL)
        {
          red = atoi(cptr);
          cptr = strtok(NULL, ",");
        }
        if (cptr != NULL)
        {
          green = atoi(cptr);
          cptr = strtok(NULL, ",");
        }
        if (cptr != NULL)
        {
          blue = atoi(cptr);
        }

        //Assign color if valid
        if ((red != -1) && (blue != -1) && (green != -1) )
        {
          colorWipe(strip.Color(red, green, blue), 10);
        }
      }

      if (buffer[0] == '!')
      {
        //off
        colorWipe(strip.Color(0, 0, 0), 10);
      }

      if (buffer[0] == '%')
      {
        rainbowCycle(100);
      }

      //Clear buffer
      bufferIndex = 0;
      buffer[0] = 0;
    }
    else
    {
      bufferIndex++;
      if (bufferIndex > 99)
        bufferIndex = 0;
    }
  } //end while (serial avail)

  serialIdleCtr++;

  if (serialIdleCtr >= (2*60*30))
  {
    serialIdleCtr = 0;
    //random color
    colorWipe(strip.Color(random(0,200), random(0,200), random(0,200)), 10);
  }
}

// From AdaFruit
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

// From AdaFruit
// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// From AdaFruit
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
    WheelPos -= 170;
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}
