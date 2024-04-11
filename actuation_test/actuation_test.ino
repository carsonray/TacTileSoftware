#import "Button.h"

#define SS 2
#define MOSI 3
#define CLK 4

Button trigger = Button(5);

int clockSpeed = 1000;

bool state = false;

int buffer[5] = {1, 1, 0, 0, 0};

int count = 0;

unsigned long timerStart = 0;

void writePins(bool ss, bool mosi, bool clk) {
  digitalWrite(SS, ss);
  digitalWrite(MOSI, mosi);
  digitalWrite(CLK, clk);
}

void setup() {
  Serial.begin(9600);

  pinMode(SS, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(CLK, OUTPUT);

  writePins(1, 0, 1);
}

void loop() {
  if (trigger.changeTo(HIGH)) {
    state = !state;
    Serial.println(state);
    if (!state) {
      writePins(1, 0, 1);
      count = 0;
      timerStart = millis();
    }
  }
  if ((millis()-timerStart) > 1000/clockSpeed) {
    if (state && count/2 < 5) {
      timerStart = millis();
      if (count % 2 == 0) {
        writePins(0, buffer[count/2], 1);
      } else {
        writePins(0, 0, 0);
      }
      count++;
    } else if (!state) {
      writePins(0, 0, 0);
    }
  }
  trigger.update();
}
