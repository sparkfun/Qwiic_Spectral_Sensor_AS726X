#include "AS726X.h"

void setup() {
}

void loop() {
  AS726X sensor;
  while (1 == 1) {
    sensor.takeMeasurements();
    //sensor.takeMeasurementsWithBulb(); //Uncomment to illuminate the onboard Bulb for taking measurements
    sensor.printMeasurements();//Prints out all measurements
  }
}
