#import "Button.h"

String names[3] = {"SS", "MOSI", "CLK"};
uint8_t pins[3] = {2,3,4};
Button buttons[3] = {Button(5), Button(6), Button(7)};

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 3; i++) {
    pinMode(pins[i], OUTPUT);
  }
}

void loop() {
  for (int i = 0; i < 3; i++) {
    if (buttons[i].change()) {
      digitalWrite(pins[i], buttons[i].state());
      Serial.println(names[i] + ": " + (String) buttons[i].state());
    }
    buttons[i].update();
  }
}
