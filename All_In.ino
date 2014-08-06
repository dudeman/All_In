#include <Adafruit_NeoPixel.h>

const unsigned int BAUD_RATE = 9600;
const long DEBOUNCE_DELAY = 50;    // the debounce time; increase if the output flickers

namespace input {
  const int BUTTON_COUNT = 1;
  const int COLOR_GREEN = 1;
  const int COLOR_RED = 2;
  const int COLOR_BLUE = 3;
  const int COLOR_WHITE = 4;

  typedef struct {
    int inputPin;
    int ledPin;
    int state;
    int prevState;
    bool on;
    long lastDebounceTime;
    int color;
  } Button;

  Button buttons[input::BUTTON_COUNT] = {
    {12, 5, LOW, LOW, false, 0, COLOR_WHITE}//,
    // {3, 4, LOW, LOW, false, 0, COLOR_BLUE}
  };

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
      Serial.print("prev state ");
      Serial.println(button->prevState);

      button->lastDebounceTime = millis();
    }
    if ((millis() - button->lastDebounceTime) > DEBOUNCE_DELAY) {
      // Serial.println("button debounce ");
      // if the button state has changed:
      if (reading != button->state) {
        Serial.println("button change ");
        button->state = reading;
        // only toggle the button if the new button state is HIGH
        if (button->state == HIGH) {
          button->on = !button->on;
          if(button->on) {
            digitalWrite(button->ledPin, HIGH);
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

const int LED_COUNT = 16;
const int STRIP_PIN = 6;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, STRIP_PIN, NEO_GRB + NEO_KHZ800);


void update() {
  static int oldState;
  static int state;
  static int ticks = 0;
  //determine state
  int buttonsOn = 0;
  for(int i=0; i < input::BUTTON_COUNT; i++) {
    if (input::buttons[i].on) {
      buttonsOn++;
    }
  }
  if(buttonsOn > 0) {
    int px = ticks % LED_COUNT;
    for(uint16_t i=0; i<strip.numPixels(); i++) {
      // Serial.println('rendering px');
      if(i == px) {
        strip.setPixelColor(i, strip.Color(0, 0, 100));
      } else {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
      }
    }
  }
  ticks++;
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





