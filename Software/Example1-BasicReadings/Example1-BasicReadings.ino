/*
  Using the AS726x Spectral Sensor
  By: Nathan Seidle
  SparkFun Electronics
  Date: March 8th, 2017
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  Feel like supporting our work? Buy a board from SparkFun!

  Read the raw integer values from the sensor.
  Violet, Blue, Green, Yellow, Orange, and Red data from the AS7262 (Visible)
  R, S, T, U, V, W data from the AS7263 (NIR)

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

  if(sensorVersion == SENSORTYPE_AS7262) Serial.println("AS7262 online!");
  if(sensorVersion == SENSORTYPE_AS7263) Serial.println("AS7263 online!");
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
    Serial.print(getViolet());
    Serial.print("] B[");
    Serial.print(getBlue());
    Serial.print("] G[");
    Serial.print(getGreen());
    Serial.print("] Y[");
    Serial.print(getYellow());
    Serial.print("] O[");
    Serial.print(getOrange());
    Serial.print("] R[");
    Serial.print(getRed());
  }
  else if(sensorVersion == SENSORTYPE_AS7263)
  {
    //Near IR readings
    Serial.print(" Reading: R[");
    Serial.print(getR());
    Serial.print("] S[");
    Serial.print(getS());
    Serial.print("] T[");
    Serial.print(getT());
    Serial.print("] U[");
    Serial.print(getU());
    Serial.print("] V[");
    Serial.print(getV());
    Serial.print("] W[");
    Serial.print(getW());
  }

  Serial.print("] tempF[");
  Serial.print(tempF, 1);
  Serial.print("]");

  Serial.println();

  //delay(500);
}
