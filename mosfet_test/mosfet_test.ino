#import "Button.h"
#import <math.h>

Button trigger = Button(2);

uint8_t ser[2] = {3, 4};
#define CLK 5
#define RCLK 6
#define PWM 9

uint32_t clockInterval = 1000;

class ControlParams {
  public:
    ControlParams(uint8_t index, bool phase, bool polarity) {
      this->index = index;
      this->phase = phase;
      this->polarity = polarity;
    };

    uint8_t getIndex() {
      return index;
    }

    bool getPhase() {
      return phase;
    }

    bool getPolarity() {
      return polarity;
    }

    void setIndex(uint8_t index) {
      this->index = index;
    }

    void setPhase(bool phase) {
      this->phase = phase;
    }

    void setPolarity(bool polarity) {
      this->polarity = polarity;
    }
  private:
    uint8_t index;
    bool phase;
    bool polarity;
};

bool state = false;
bool enabled = false;
int dir = 1;

ControlParams params[2] = {ControlParams(0, false, false), ControlParams(0, false, true)};
bool phaseCycles[2][8] = {{false, false, true, true, false, false, true, true}, {true, false, false, true, true, false, false, true}};
uint8_t cycleStep = 0;

uint8_t serBuffers[2] = {0, 0};

uint8_t serRemaining = 0;

uint32_t timerStart = 0;

char strBuffer[32];

void resetTimer() {
  timerStart = micros();
  state = false;
}

void selectOutput(ControlParams* params) {
  for (uint8_t i = 0; i < 2; i++) {
    ControlParams param = *(params + i);
    serBuffers[i] = param.getPolarity();
    serBuffers[i] += param.getPhase() << 1;
    serBuffers[i] += param.getIndex() << 2;
  }

  serRemaining = 5;

  resetTimer();
}

void disableOutput() {
  digitalWrite(PWM, 0);
  serBuffers[0] = 0;
  serBuffers[1] = 0;
  serRemaining = 5;
  resetTimer();
}

void sendBit() {
  if (serRemaining > 0) {
    for (int i = 0; i < 2; i++) {
      digitalWrite(ser[i], serBuffers[i] & 1);
      serBuffers[i] = serBuffers[i] >> 1;
    }
    digitalWrite(CLK, 1);
    serRemaining--;
    if (serRemaining == 0) {
      digitalWrite(RCLK, 1);
    }
  }
}

void updateData() {
  uint32_t currTime = micros();
  if (currTime >= timerStart) {
    timerStart = currTime + clockInterval;
    state = !state;
    if (state) {
      sendBit();
    } else {
      digitalWrite(CLK, 0);
      digitalWrite(RCLK, 0);

      for (int i = 0; i < 2; i++) {
        digitalWrite(ser[i], 0);
      }
    }
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(CLK, OUTPUT);
  pinMode(RCLK, OUTPUT);
  pinMode(PWM, OUTPUT);
  pinMode(A0, INPUT);

  for (int i = 0; i < 2; i++) {
    pinMode(ser[i], OUTPUT);
    params[i].setPhase(phaseCycles[i][cycleStep]);
  }

  disableOutput();
}

void loop() {
  Serial.println(analogRead(A0));
  if (trigger.changeTo(true) && serRemaining == 0) {
    enabled = !enabled;
    digitalWrite(PWM, enabled);
    if (enabled) {
      selectOutput(params);

      /*for (int i = 0; i < 2; i++) {
        sprintf(strBuffer, "Index%d: %d; ", i, params[i].getIndex());
        Serial.print(strBuffer);
        sprintf(strBuffer, "Phase%d: %d; ", i, params[i].getPhase());
        Serial.print(strBuffer);
        sprintf(strBuffer, "Polarity%d: %d; ", i, params[i].getPolarity());
        Serial.println(strBuffer);
      }

      Serial.print("Cycle step: ");
      Serial.println(cycleStep);*/

      params[1].setIndex(params[1].getIndex()+1);

      if (params[1].getIndex() > 7) {
        params[1].setIndex(0);

        if ((cycleStep == 0) && (dir < 0)) {
          params[0].setIndex(params[0].getIndex()+1);
          if (params[0].getIndex() > 7) {
            params[1].setIndex(0);
          }
          dir = -dir;
        } else if ((cycleStep == 7) && (dir > 0)) {
          dir = -dir;
        } else {
          cycleStep+=dir;
        }

        for (int i = 0; i < 2; i++) {
          params[i].setPhase(phaseCycles[i][cycleStep]);
          params[i].setPolarity(!params[i].getPolarity());
        }
      }
    } else {
      //Serial.println("Disabled");
      disableOutput();
    }
  }
  updateData();
  trigger.update();
}
