/*
  Using the AS726x Spectral Sensor
  By: Nathan Seidle
  SparkFun Electronics
  Date: March 8th, 2017
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  Feel like supporting our work? Buy a board from SparkFun!

  Read the calibrated sensor values. These are more accurate but you have to deal with floating point numbers.
  Calibrated Violet, Blue, Green, Yellow, Orange, and Red data from the AS7262 (Visible)
  Calibrated R, S, T, U, V, W data from the AS7263 (NIR)

  The AS726x Qwiic board can be configured to communicate over I2C (default) or serial. This example
  assumes we are communicating over I2C. See schematic for jumpers to change to serial and datasheet
  for the AT command interface.

  Hardware Connections:
  Attach the Qwiic Shield to your Arduino/Photon/ESP32 or other
  Plug the AS726x onto the shield
  Serial.print it out at 115200 baud to serial monitor.

  Available:
  void takeMeasurements()
  void takeMeasurementsWithBulb()
  int getViolet() Blue() Green() Yellow() Orange() Red()
  float getCalibratedViolet(), Blue, Green, Yellow, Orange, Red
  void setMeasurementMode(byte mode)
  boolean dataAvailable()
  boolean as726xSetup()
  byte getTemperature()
  byte getTemperatureF()
  void setIndicatorCurrent(byte)
  void enableIndicator()
  void disableIndicator()
  void setBulbCurrent(byte)
  void enableBulb()
  void disableBulb()
  void softReset()
  setGain(byte gain)
  setIntegrationTime(byte integrationValue)
  enableInterrupt()
  disableInterrupt()

  To Write:

*/

#include <Wire.h>

byte sensorVersion = 0; //Keeps track of whether we have an AS7262 or AS7263
#define SENSORTYPE_AS7262 0x3E
#define SENSORTYPE_AS7263 0x3F

void setup()
{
  Serial.begin(115200);
  while (!Serial);
  Serial.println("AS726x Read Example");

  Wire.begin();

  sensorVersion = as726xSetup();
  if (sensorVersion == 0)
  {
    Serial.println("Sensor failed to respond. Check wiring.");
    while (1); //Freeze!
  }
  Serial.println("AS726x online!");
}

void loop()
{
  takeMeasurements(); //No LED - easier on your eyes
  //takeMeasurementsWithBulb(); //Use LED - bright white LED

  float tempF = getTemperatureF();
  
  if(sensorVersion == SENSORTYPE_AS7262)
  {
    //Visible readings
    Serial.print(" Reading: V[");
    Serial.print(getCalibratedViolet(), 2);
    Serial.print("] B[");
    Serial.print(getCalibratedBlue(), 2);
    Serial.print("] G[");
    Serial.print(getCalibratedGreen(), 2);
    Serial.print("] Y[");
    Serial.print(getCalibratedYellow(), 2);
    Serial.print("] O[");
    Serial.print(getCalibratedOrange(), 2);
    Serial.print("] R[");
    Serial.print(getCalibratedRed(), 2);
  }
  else if(sensorVersion == SENSORTYPE_AS7263)
  {
    //Near IR readings
    Serial.print(" Reading: R[");
    Serial.print(getCalibratedR(), 2);
    Serial.print("] S[");
    Serial.print(getCalibratedS(), 2);
    Serial.print("] T[");
    Serial.print(getCalibratedT(), 2);
    Serial.print("] U[");
    Serial.print(getCalibratedU(), 2);
    Serial.print("] V[");
    Serial.print(getCalibratedV(), 2);
    Serial.print("] W[");
    Serial.print(getCalibratedW(), 2);
  }

  Serial.print("] tempF[");
  Serial.print(tempF, 1);
  Serial.print("]");

  Serial.println();

  //delay(500);
}
