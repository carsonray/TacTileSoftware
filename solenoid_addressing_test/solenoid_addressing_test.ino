#import <Button.h>

#define CPC 2
#define CPR 3
#define resetPin 4
#define MRC 5

#define Q0 6
#define Q1 7
#define RCLK 8
#define SER 9

uint16_t halfPulse = 50;



bool clockVal = false;

uint8_t count = 0;
uint8_t bit = 0;

Button reset = Button(resetPin);
Button count0 = Button(Q0);
Button count1 = Button(Q1);

void setup() {
  pinMode(CPC, OUTPUT);
  pinMode(CPR, OUTPUT);
  pinMode(MRC, OUTPUT);
  pinMode(RCLK, OUTPUT);
  pinMode(SER, OUTPUT);
}

void setClock(bool value) {
  digitalWrite(CPC, value);
  digitalWrite(CPR, !value);
}

void toggleClock() {
  clockVal = !clockVal;
  setClock(clockVal);
}

void checkReset() {
  digitalWrite(MRC, !reset.state());
  if (reset.state()) {
    clockVal = false;
    count = 0;
    bit = 0;
  }
}

void checkLatch() {
  if (count0.state() && count1.state()) {
    digitalWrite(RCLK, HIGH);
  } else {
    digitalWrite(RCLK, LOW);
  }
}

void nextBit() {
  digitalWrite(SER, bitRead(count, bit));
  bit++;
  if (bit == 4) {
    bit = 0;
    count++;
    count %= 16;
  }
}

void updateDelay(uint32_t ms)
{
  uint32_t start = millis();
  do
  {
    checkReset();
    checkLatch();
    reset.update();
    count0.update();
    count1.update();
  } while (millis() - start < ms);
}

void loop() {
  if (clockVal == false) {
    nextBit();
  }
  toggleClock();
  updateDelay(halfPulse);
}
