#include <Adafruit_NeoPixel.h>

const unsigned int BAUD_RATE = 9600;
const long DEBOUNCE_DELAY = 50;    // the debounce time; increase if the output flickers

const int LED_COUNT = 16;
const int STRIP_PIN = 6;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, STRIP_PIN, NEO_GRB + NEO_KHZ800);

namespace input {
  const int BUTTON_COUNT = 2;
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

  Button buttons[input::BUTTON_COUNT] = {
    {12, 5, LOW, LOW, false, 0, COLOR_GREEN, 3},
    {3, 4, LOW, LOW, false, 0, COLOR_WHITE, 7}
  };
  
  Button lastButtonPressed;

  void setup(Button button) {
    pinMode(button.ledPin, OUTPUT);
    // pinMode(button.inputPin, INPUT);
    Serial.print("button setup: ");
    Serial.println(button.inputPin);
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
            lastButtonPressed = *button;
          } else {
            digitalWrite(button->ledPin, LOW);
          }
          Serial.print("button toggle: ");
          Serial.print(button->inputPin);
          Serial.println(button->on ? "true" : "false");
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
  static int ticks = 0;
  int buttonsOn = 0;
  int framerate = map(analogRead(1), 0, 1023, 10, 100);

  //determine state
  for(int i=0; i < input::BUTTON_COUNT; i++) {
    if (input::buttons[i].on) {
      buttonsOn++;
    }
  }

  // reset state and ticks when state changes
  if(buttonsOn != prevState) {
    prevState = buttonsOn;
    ticks = 0;
  }

  // framerate *= (input::BUTTON_COUNT - buttonsOn) / input::BUTTON_COUNT;
  // control the update framerate
  if ((millis() - lastDebounceTime) > framerate) {
    lastDebounceTime = millis();
    if(buttonsOn > 0) {
      updateStrip(ticks);
    } else {
      clearStrip();
    }
    ticks++;
  }
}

void updateStrip(int ticks) {
  int px = ticks % LED_COUNT;
  for(uint16_t i=0; i<LED_COUNT; i++) {
    if(i == px) {
      strip.setPixelColor(offsetPixelLocation(i+input::lastButtonPressed.ledPosition), input::lastButtonPressed.color);
    } else {
      strip.setPixelColor(offsetPixelLocation(i+input::lastButtonPressed.ledPosition), strip.Color(0, 0, 0));
    }
  }
}

int offsetPixelLocation(int offsetPosition) {
  if(offsetPosition > LED_COUNT) {
    return offsetPosition - LED_COUNT;
  } else {
    return offsetPosition; 
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
  Serial.begin(BAUD_RATE);
  for(int i=0; i < input::BUTTON_COUNT; i++) {
      input::setup(input::buttons[i]);
  }
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.println("Setup Complete");
}

void loop() {
  input::readButtons();
  update();
  render();
}





