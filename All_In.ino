const unsigned int BAUD_RATE = 9600;
const long DEBOUNCE_DELAY = 50;    // the debounce time; increase if the output flickers

namespace input {
  const int BUTTON_COUNT = 2;

  typedef struct {
    int inputPin;
    int ledPin;
    int state;
    int prevState;
    bool on;
    long lastDebounceTime;
  } Button;

  void setup(Button button) {
    pinMode(button.ledPin, OUTPUT);
  }

  void read(Button button) {
    int reading = digitalRead(button.inputPin);
    // check to see if you just pressed the button
    // (i.e. the input went from LOW to HIGH),  and you've waited
    // long enough since the last press to ignore any noise:

    // If the switch changed, due to noise or pressing:
    if (reading != button.lastDebounceTime) {
      // reset the debouncing timer
      button.lastDebounceTime = millis();
    }
    if ((millis() - button.lastDebounceTime) > DEBOUNCE_DELAY) {
      // if the button state has changed:
      if (reading != buttonState) {
        button.state = reading;
        // only toggle the button if the new button state is HIGH
        if (button.state == HIGH) {
          button.on = !button.on
          Serial.print("button toggle: ");
          Serial.print(button.inputPin);
          Serial.println(button.on ? "true" : "false");
        }
      }
    }
    lastButtonState = reading;
  }
}
input::Button buttons[2] = {
  {1, LOW, LOW, false, 0},
  {2, LOW, LOW, false, 0}
};

void setup() {
  Serial.begin(BAUD_RATE);
  for(int i; i < input::BUTTON_COUNT; i++) {
    input::setup(buttons[i]);
  }

}

void loop() {
  checkButtons();
  // update();
  // render();
}

void checkButtons() {
  for(int i; i < input::BUTTON_COUNT; i++) {
    input::read(buttons[i]);
  }
}



