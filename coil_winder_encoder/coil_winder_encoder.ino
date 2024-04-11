#import "Button.h"

//Optical encoder pulses
int count = 0;
//Encoder ticks per revolution
int ticksPerRev = 15;

//Button to handle sensor events
Button sensor = Button(3);

void setup()
{
  Serial.begin(9600);
}
void loop()
{
  //Counts sensor ticks and prints conversion to revolutions
  if (sensor.changeTo(LOW)) {
    count++;
    Serial.println((float) count/ticksPerRev, 2);
  }
  sensor.update();
}

