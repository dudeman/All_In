#include <Adafruit_NeoPixel.h>

const unsigned int BAUD_RATE = 9600;
const long DEBOUNCE_DELAY = 50;    // the debounce time; increase if the output flickers

const int LED_COUNT = 270;
const int STRIP_PIN = 6;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, STRIP_PIN, NEO_GRB + NEO_KHZ800);

namespace input {
  const uint32_t COLOR_GREEN = strip.Color(0, 255, 0);
  const uint32_t COLOR_RED = strip.Color(255, 0, 0);
  const uint32_t COLOR_BLUE = strip.Color(0, 0, 255);
  const uint32_t COLOR_WHITE = strip.Color(255, 255, 255);

  typedef struct {
    int inputPin;
    int ledPin;
    int state;
    int prevState;
    bool on;
    long lastDebounceTime;
    uint32_t color;
    int ledPosition;
  } Button;

  const int BUTTON_COUNT = 12;
  Button buttons[input::BUTTON_COUNT] = {
    {22, 23, LOW, LOW, false, 0, COLOR_BLUE, 250},
    {24, 25, LOW, LOW, false, 0, COLOR_GREEN, 2},
    {26, 27, LOW, LOW, false, 0, COLOR_RED, 20},
    {28, 29, LOW, LOW, false, 0, COLOR_WHITE, 46},
    {30, 31, LOW, LOW, false, 0, COLOR_BLUE, 73},
    {32, 33, LOW, LOW, false, 0, COLOR_GREEN, 98},
    {34, 35, LOW, LOW, false, 0, COLOR_RED, 115},
    {36, 37, LOW, LOW, false, 0, COLOR_WHITE, 138},
    {38, 39, LOW, LOW, false, 0, COLOR_BLUE, 154},
    {40, 41, LOW, LOW, false, 0, COLOR_GREEN, 181},
    {42, 43, LOW, LOW, false, 0, COLOR_RED, 208},
    {44, 45, LOW, LOW, false, 0, COLOR_WHITE, 233}
  };

  void setup(Button button) {
    pinMode(button.ledPin, OUTPUT);
    // pinMode(button.inputPin, INPUT);
    // Serial.print("button setup: ");
    // Serial.println(button.inputPin);
  }

  void read(Button *button) {
    int reading = digitalRead(button->inputPin);
    // Serial.print("button read: ");
    // Serial.println(button.inputPin);
    // Serial.print(reading);
    // check to see if you just pressed the button
    // (i.e. the input went from LOW to HIGH),  and you've waited
    // long enough since the last press to ignore any noise:

    // If the switch changed, due to noise or pressing:
    if (reading != button->prevState) {
      // reset the debouncing timer
      button->lastDebounceTime = millis();
    }
    if ((millis() - button->lastDebounceTime) > DEBOUNCE_DELAY) {
      // if the button state has changed:
      if (reading != button->state) {
        button->state = reading;
        // only toggle the button if the new button state is HIGH
        if (button->state == HIGH) {
          button->on = !button->on;
          if(button->on) {
            digitalWrite(button->ledPin, HIGH);
          } else {
            digitalWrite(button->ledPin, LOW);
          }
          // Serial.print("button toggle: ");
          // Serial.print(button->inputPin);
          // Serial.println(button->on ? "true" : "false");
        }
      }
    }
    button->prevState = reading;
  }

  void readButtons() {
    for(int i=0; i < input::BUTTON_COUNT; i++) {
      input::read(&buttons[i]);
    }
  }
}




void update() {
  static long lastDebounceTime = 0;
  static int prevState = 0;
  static bool resetState = false;
  static int ticks = 0;
  int buttonsOn = 0;
  // int framerate = map(analogRead(1), 0, 1023, 10, 100);
  int framerate = 6;

  //determine state
  for(int i=0; i < input::BUTTON_COUNT; i++) {
    if (input::buttons[i].on) {
      buttonsOn++;
      // if (framerate > 0) {
      //   framerate -= 1;
      // }
    }
  }

  // reset state and ticks when state changes
  if(buttonsOn != prevState) {
    resetState = (buttonsOn < prevState);
    prevState = buttonsOn;
    ticks = 0;
  }

  // framerate *= (input::BUTTON_COUNT - buttonsOn) / input::BUTTON_COUNT;
  // control the update framerate
  if ((millis() - lastDebounceTime) > framerate) {
    lastDebounceTime = millis();
    if(buttonsOn > 0 && !resetState) {
      allInAnimation(ticks, buttonsOn);
    } else if(buttonsOn > 0) {
      resetAnimation(ticks);
    } else {
      clearStrip();
    }
    ticks++;
  }
}

void allInAnimation(int ticks, int buttonsOn) {
  int color = ticks % 256;
  int px = ticks % LED_COUNT;
  int fadeInInterval = 32;
  clearStrip();

  if(ticks < fadeInInterval) {
    strip.setBrightness((256 / fadeInInterval) * ticks+1 % 256);
  }

  if(buttonsOn > 1 && ticks >= fadeInInterval) {
    for(uint16_t i=0; i<LED_COUNT; i++) {
      strip.setPixelColor(i, wheel((i+color) & 255));
    }
  }

  for(uint16_t i=0; i<LED_COUNT; i++) {
    for(int j=0; j < input::BUTTON_COUNT; j++) {
      if (input::buttons[j].on) {
        if(ticks < fadeInInterval && i % 2 == px % 2) {
          strip.setPixelColor(i, input::buttons[j].color);
        } else if(buttonsOn == 1) {
          if(i == px) {
            strip.setPixelColor(offsetPixelLocation(i+input::buttons[j].ledPosition-fadeInInterval), input::buttons[j].color);
          } else if(i == offsetPixelLocation(px-1)) {
            strip.setPixelColor(offsetPixelLocation(i+input::buttons[j].ledPosition-fadeInInterval), reduceBrightness(input::buttons[j].color, 2));
          } else if(i == offsetPixelLocation(px-2)) {
            strip.setPixelColor(offsetPixelLocation(i+input::buttons[j].ledPosition-fadeInInterval), reduceBrightness(input::buttons[j].color, 3));
          } else if(i == offsetPixelLocation(px-3)) {
            strip.setPixelColor(offsetPixelLocation(i+input::buttons[j].ledPosition-fadeInInterval), reduceBrightness(input::buttons[j].color, 4));
          } else if(i == offsetPixelLocation(px-4)) {
            strip.setPixelColor(offsetPixelLocation(i+input::buttons[j].ledPosition-fadeInInterval), reduceBrightness(input::buttons[j].color, 4));
          } else if(i == offsetPixelLocation(px-5)) {
            strip.setPixelColor(offsetPixelLocation(i+input::buttons[j].ledPosition-fadeInInterval), reduceBrightness(input::buttons[j].color, 5));
          } else if(i == offsetPixelLocation(px-6)) {
            strip.setPixelColor(offsetPixelLocation(i+input::buttons[j].ledPosition-fadeInInterval), reduceBrightness(input::buttons[j].color, 5));
          } else if(i == offsetPixelLocation(px-7)) {
            strip.setPixelColor(offsetPixelLocation(i+input::buttons[j].ledPosition-fadeInInterval), reduceBrightness(input::buttons[j].color, 5));
          }
        }
      }
    }
  }

  if(ticks >= fadeInInterval) {
    for(int j=0; j < input::BUTTON_COUNT; j++) {
      if (input::buttons[j].on) {
        if(ticks % 6 < 3) {
          strip.setPixelColor(input::buttons[j].ledPosition+2, input::buttons[j].color);
          strip.setPixelColor(input::buttons[j].ledPosition+3, input::buttons[j].color);
          strip.setPixelColor(input::buttons[j].ledPosition+4, input::buttons[j].color);
          strip.setPixelColor(input::buttons[j].ledPosition+5, input::buttons[j].color);
          strip.setPixelColor(input::buttons[j].ledPosition+6, input::buttons[j].color);
        } else {
          strip.setPixelColor(input::buttons[j].ledPosition+2, strip.Color(0, 0, 0));
          strip.setPixelColor(input::buttons[j].ledPosition+3, strip.Color(0, 0, 0));
          strip.setPixelColor(input::buttons[j].ledPosition+4, strip.Color(0, 0, 0));
          strip.setPixelColor(input::buttons[j].ledPosition+5, strip.Color(0, 0, 0));
          strip.setPixelColor(input::buttons[j].ledPosition+6, strip.Color(0, 0, 0));
        }
      }
    }
  }
}

void resetAnimation(int ticks) {
  int color = ticks % 256;
  clearStrip();
  for(uint16_t i=0; i<LED_COUNT; i++) {
    strip.setPixelColor(i, wheel((i+color) & 255));
  }

  for(int j=0; j < input::BUTTON_COUNT; j++) {
    if (input::buttons[j].on) {
      if(ticks % 6 < 3) {
        strip.setPixelColor(input::buttons[j].ledPosition+2, input::buttons[j].color);
        strip.setPixelColor(input::buttons[j].ledPosition+3, input::buttons[j].color);
        strip.setPixelColor(input::buttons[j].ledPosition+4, input::buttons[j].color);
        strip.setPixelColor(input::buttons[j].ledPosition+5, input::buttons[j].color);
        strip.setPixelColor(input::buttons[j].ledPosition+6, input::buttons[j].color);
      } else {
        strip.setPixelColor(input::buttons[j].ledPosition+2, strip.Color(0, 0, 0));
        strip.setPixelColor(input::buttons[j].ledPosition+3, strip.Color(0, 0, 0));
        strip.setPixelColor(input::buttons[j].ledPosition+4, strip.Color(0, 0, 0));
        strip.setPixelColor(input::buttons[j].ledPosition+5, strip.Color(0, 0, 0));
        strip.setPixelColor(input::buttons[j].ledPosition+6, strip.Color(0, 0, 0));
      }
    }
  }
}

void restAnimation(int ticks) {
  clearStrip();
}

int offsetPixelLocation(int offsetPosition) {
  if(offsetPosition > LED_COUNT) {
    return offsetPosition - LED_COUNT;
  } else if(offsetPosition < 0) {
    return LED_COUNT - offsetPosition;
  } else {
    return offsetPosition;
  }
}

uint32_t reduceBrightness(uint32_t c, int factor) {
  uint8_t
    r = (uint8_t)(c >> 16),
    g = (uint8_t)(c >>  8),
    b = (uint8_t)c;

  factor = 50 * factor;

  // Serial.print("before: ");
  // Serial.print("r: ");
  // Serial.print(r);
  // Serial.print("g: ");
  // Serial.print(g);
  // Serial.print("b: ");
  // Serial.print(b);
  // Serial.print("factor: ");
  // Serial.println(factor);

  r = (r - factor < 0) ? 0 : r - factor;
  g = (g - factor < 0) ? 0 : g - factor;
  b = (b - factor < 0) ? 0 : b - factor;

  // Serial.print("after: ");
  // Serial.print("r: ");
  // Serial.print(r);
  // Serial.print("g: ");
  // Serial.print(g);
  // Serial.print("b: ");
  // Serial.println(b);

  return strip.Color(r, g, b);
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void clearStrip() {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
}

void render() {
  strip.show();
  // delay(100);
}



/*****
 * Arduino Setup & Loop
 *****/

void setup() {
  // Serial.begin(BAUD_RATE);
  for(int i=0; i < input::BUTTON_COUNT; i++) {
      input::setup(input::buttons[i]);
  }
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  // Serial.println("Setup Complete");
}

void loop() {
  input::readButtons();
  update();
  render();
}





