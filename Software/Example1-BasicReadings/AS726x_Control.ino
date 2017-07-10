#define AS726X_ADDR 0x49 //7-bit unshifted default I2C Address

//Register addresses
#define AS726x_DEVICE_TYPE 0x00
#define AS726x_HW_VERSION 0x01
#define AS726x_CONTROL_SETUP 0x04
#define AS726x_INT_T 0x05
#define AS726x_DEVICE_TEMP 0x06
#define AS726x_LED_CONTROL 0x07

#define AS72XX_SLAVE_STATUS_REG 0x00
#define AS72XX_SLAVE_WRITE_REG 0x01
#define AS72XX_SLAVE_READ_REG 0x02

//The same register locations are shared between the AS7262 and AS7263, they're just called something different
//AS7262 Registers
#define AS7262_V 0x08
#define AS7262_B 0x0A
#define AS7262_G 0x0C
#define AS7262_Y 0x0E
#define AS7262_O 0x10
#define AS7262_R 0x12
#define AS7262_V_CAL 0x14
#define AS7262_B_CAL 0x18
#define AS7262_G_CAL 0x1C
#define AS7262_Y_CAL 0x20
#define AS7262_O_CAL 0x24
#define AS7262_R_CAL 0x28

//AS7263 Registers
#define AS7263_R 0x08
#define AS7263_S 0x0A
#define AS7263_T 0x0C
#define AS7263_U 0x0E
#define AS7263_V 0x10
#define AS7263_W 0x12
#define AS7263_R_CAL 0x14
#define AS7263_S_CAL 0x18
#define AS7263_T_CAL 0x1C
#define AS7263_U_CAL 0x20
#define AS7263_V_CAL 0x24
#define AS7263_W_CAL 0x28

#define AS72XX_SLAVE_TX_VALID 0x02
#define AS72XX_SLAVE_RX_VALID 0x01

#define SENSORTYPE_AS7262 0x3E
#define SENSORTYPE_AS7263 0x3F

#define POLLING_DELAY 5 //Amount of ms to wait between checking for virtual register changes

//Sets up the sensor for constant read
//Returns the sensor version (AS7262 or AS7263)
byte as726xSetup(void)
{
  byte sensorVersion = virtualReadRegister(AS726x_HW_VERSION);
  if (sensorVersion != 0x3E && sensorVersion != 0x3F) //HW version for AS7262 and AS7263
  {
    Serial.print("ID (should be 0x3E or 0x3F): 0x");
    Serial.println(sensorVersion, HEX);
    return (false); //Device ID should be 0x3E
  }

  setBulbCurrent(0b00); //Set to 12.5mA (minimum)
  disableBulb(); //Turn off to avoid heating the sensor

  setIndicatorCurrent(0b11); //Set to 8mA (maximum)
  disableIndicator(); //Turn off lights to save power
  
  setIntegrationTime(50); //50 * 2.8ms = 140ms. 0 to 255 is valid.
  //If you use Mode 2 or 3 (all the colors) then integration time is double. 140*2 = 280ms between readings.

  setGain(3); //Set gain to 64x

  setMeasurementMode(3); //One-shot reading of VBGYOR

  return (sensorVersion);
}

//Sets the measurement mode
//Mode 0: Continuous reading of VBGY (7262) / STUV (7263)
//Mode 1: Continuous reading of GYOR (7262) / RTUX (7263)
//Mode 2: Continuous reading of all channels (power-on default)
//Mode 3: One-shot reading of all channels
void setMeasurementMode(byte mode)
{
  if(mode > 0b11) mode = 0b11;

  //Read, mask/set, write
  byte value = virtualReadRegister(AS726x_CONTROL_SETUP); //Read
  value &= 0b11110011; //Clear BANK bits
  value |= (mode << 2); //Set BANK bits with user's choice
  virtualWriteRegister(AS726x_CONTROL_SETUP, value); //Write
}

//Sets the gain value
//Gain 0: 1x (power-on default)
//Gain 1: 3.7x
//Gain 2: 16x
//Gain 3: 64x
void setGain(byte gain)
{
  if(gain > 0b11) gain = 0b11;

  //Read, mask/set, write
  byte value = virtualReadRegister(AS726x_CONTROL_SETUP); //Read
  value &= 0b11001111; //Clear GAIN bits
  value |= (gain << 4); //Set GAIN bits with user's choice
  virtualWriteRegister(AS726x_CONTROL_SETUP, value); //Write
}

//Sets the integration value
//Give this function a byte from 0 to 255.
//Time will be 2.8ms * [integration value]
void setIntegrationTime(byte integrationValue)
{
  virtualWriteRegister(AS726x_INT_T, integrationValue); //Write
}

//Enables the interrupt pin
void enableInterrupt()
{
  //Read, mask/set, write
  byte value = virtualReadRegister(AS726x_CONTROL_SETUP); //Read
  value |= 0b01000000; //Set INT bit
  virtualWriteRegister(AS726x_CONTROL_SETUP, value); //Write
}

//Disables the interrupt pin
void disableInterrupt()
{
  //Read, mask/set, write
  byte value = virtualReadRegister(AS726x_CONTROL_SETUP); //Read
  value &= 0b10111111; //Clear INT bit
  virtualWriteRegister(AS726x_CONTROL_SETUP, value); //Write
}

//Tells IC to take measurements and polls for data ready flag
void takeMeasurements()
{
  clearDataAvailable(); //Clear DATA_RDY flag when using Mode 3

  //Goto mode 3 for one shot measurement of all channels
  setMeasurementMode(3);

  //Wait for data to be ready
  while(dataAvailable() == false) delay(POLLING_DELAY);

  //Readings can now be accessed via getViolet(), getBlue(), etc
}

//Turns on bulb, takes measurements, turns off bulb
void takeMeasurementsWithBulb()
{
  //enableIndicator(); //Tell the world we are taking a reading. 
  //The indicator LED is red and may corrupt the readings

  enableBulb(); //Turn on bulb to take measurement

  takeMeasurements();
  
  disableBulb(); //Turn off bulb to avoid heating sensor
  //disableIndicator();
}

//Get the various color readings
int getViolet() { return(getChannel(AS7262_V)); }
int getBlue() { return(getChannel(AS7262_B)); }
int getGreen() { return(getChannel(AS7262_G)); }
int getYellow() { return(getChannel(AS7262_Y)); }
int getOrange() { return(getChannel(AS7262_O)); }
int getRed() { return(getChannel(AS7262_R)); }

//Get the various NIR readings
int getR() { return(getChannel(AS7263_R)); }
int getS() { return(getChannel(AS7263_S)); }
int getT() { return(getChannel(AS7263_T)); }
int getU() { return(getChannel(AS7263_U)); }
int getV() { return(getChannel(AS7263_V)); }
int getW() { return(getChannel(AS7263_W)); }

//A the 16-bit value stored in a given channel registerReturns 
int getChannel(byte channelRegister)
{
  int colorData = virtualReadRegister(channelRegister) << 8; //High byte
  colorData |= virtualReadRegister(channelRegister + 1); //Low byte
  return(colorData);
}

//Returns the various calibration data
float getCalibratedViolet() { return(getCalibratedValue(AS7262_V_CAL)); }
float getCalibratedBlue() { return(getCalibratedValue(AS7262_B_CAL)); }
float getCalibratedGreen() { return(getCalibratedValue(AS7262_G_CAL)); }
float getCalibratedYellow() { return(getCalibratedValue(AS7262_Y_CAL)); }
float getCalibratedOrange() { return(getCalibratedValue(AS7262_O_CAL)); }
float getCalibratedRed() { return(getCalibratedValue(AS7262_R_CAL)); }

float getCalibratedR() { return(getCalibratedValue(AS7263_R_CAL)); }
float getCalibratedS() { return(getCalibratedValue(AS7263_S_CAL)); }
float getCalibratedT() { return(getCalibratedValue(AS7263_T_CAL)); }
float getCalibratedU() { return(getCalibratedValue(AS7263_U_CAL)); }
float getCalibratedV() { return(getCalibratedValue(AS7263_V_CAL)); }
float getCalibratedW() { return(getCalibratedValue(AS7263_W_CAL)); }

//Given an address, read four bytes and return the floating point calibrated value
float getCalibratedValue(byte calAddress)
{
  byte b0, b1, b2, b3;
  b0 = virtualReadRegister(calAddress + 0);
  b1 = virtualReadRegister(calAddress + 1);
  b2 = virtualReadRegister(calAddress + 2);
  b3 = virtualReadRegister(calAddress + 3);

  //Channel calibrated values are stored big-endian
  uint32_t calBytes = 0;
  calBytes |= ((uint32_t)b0 << (8 * 3));
  calBytes |= ((uint32_t)b1 << (8 * 2));
  calBytes |= ((uint32_t)b2 << (8 * 1));
  calBytes |= ((uint32_t)b3 << (8 * 0));

  return (convertBytesToFloat(calBytes));
}

//Given 4 bytes returns the floating point value
float convertBytesToFloat(uint32_t myLong)
{
  float myFloat;
  memcpy(&myFloat, &myLong, 4); //Copy bytes into a float
  return (myFloat);
}

//Checks to see if DRDY flag is set in the control setup register
boolean dataAvailable()
{
  byte value = virtualReadRegister(AS726x_CONTROL_SETUP);
  return (value & (1 << 1)); //Bit 1 is DATA_RDY
}

//Clears the DRDY flag
//Normally this should clear when data registers are read
boolean clearDataAvailable()
{
  byte value = virtualReadRegister(AS726x_CONTROL_SETUP);
  value &= ~(1 << 1); //Set the DATA_RDY bit
  virtualWriteRegister(AS726x_CONTROL_SETUP, value);
}

//Enable the onboard indicator LED
void enableIndicator()
{
  //Read, mask/set, write
  byte value = virtualReadRegister(AS726x_LED_CONTROL);
  value |= (1 << 0); //Set the bit
  virtualWriteRegister(AS726x_LED_CONTROL, value);
}

//Disable the onboard indicator LED
void disableIndicator()
{
  //Read, mask/set, write
  byte value = virtualReadRegister(AS726x_LED_CONTROL);
  value &= ~(1 << 0); //Clear the bit
  virtualWriteRegister(AS726x_LED_CONTROL, value);
}

//Set the current limit of onboard LED. Default is max 8mA = 0b11.
void setIndicatorCurrent(byte current)
{
  if (current > 0b11) current = 0b11;
  //Read, mask/set, write
  byte value = virtualReadRegister(AS726x_LED_CONTROL); //Read
  value &= 0b11111001; //Clear ICL_IND bits
  value |= (current << 1); //Set ICL_IND bits with user's choice
  virtualWriteRegister(AS726x_LED_CONTROL, value); //Write
}

//Enable the onboard 5700k or external incandescent bulb
void enableBulb()
{
  //Read, mask/set, write
  byte value = virtualReadRegister(AS726x_LED_CONTROL);
  value |= (1 << 3); //Set the bit
  virtualWriteRegister(AS726x_LED_CONTROL, value);
}

//Disable the onboard 5700k or external incandescent bulb
void disableBulb()
{
  //Read, mask/set, write
  byte value = virtualReadRegister(AS726x_LED_CONTROL);
  value &= ~(1 << 3); //Clear the bit
  virtualWriteRegister(AS726x_LED_CONTROL, value);
}

//Set the current limit of bulb/LED.
//Current 0: 12.5mA
//Current 1: 25mA
//Current 2: 50mA
//Current 3: 100mA
void setBulbCurrent(byte current)
{
  if (current > 0b11) current = 0b11; //Limit to two bits

  //Read, mask/set, write
  byte value = virtualReadRegister(AS726x_LED_CONTROL); //Read
  value &= 0b11001111; //Clear ICL_DRV bits
  value |= (current << 4); //Set ICL_DRV bits with user's choice
  virtualWriteRegister(AS726x_LED_CONTROL, value); //Write
}

//Returns the temperature in C
//Pretty inaccurate: +/-8.5C
byte getTemperature()
{
  return (virtualReadRegister(AS726x_DEVICE_TEMP));
}
//Convert to F if needed
float getTemperatureF()
{
  float temperatureF = getTemperature();
  temperatureF = temperatureF * 1.8 + 32.0;
  return (temperatureF);
}

//Does a soft reset
//Give sensor at least 1000ms to reset
void softReset()
{
  //Read, mask/set, write
  byte value = virtualReadRegister(AS726x_CONTROL_SETUP); //Read
  value |= (1<<7); //Set RST bit
  virtualWriteRegister(AS726x_CONTROL_SETUP, value); //Write
}

//Read a virtual register from the AS726x
byte virtualReadRegister(byte virtualAddr)
{
  byte status;

  //Do a prelim check of the read register
  status = readRegister(AS72XX_SLAVE_STATUS_REG);
  if ((status & AS72XX_SLAVE_RX_VALID) != 0) //There is data to be read
  {
    //Serial.println("Premptive read");
    byte incoming = readRegister(AS72XX_SLAVE_READ_REG); //Read the byte but do nothing with it
  }

  //Wait for WRITE flag to clear
  while (1)
  {
    status = readRegister(AS72XX_SLAVE_STATUS_REG);
    if ((status & AS72XX_SLAVE_TX_VALID) == 0) break; // If TX bit is clear, it is ok to write
    delay(POLLING_DELAY);
  }

  // Send the virtual register address (bit 7 should be 0 to indicate we are reading a register).
  writeRegister(AS72XX_SLAVE_WRITE_REG, virtualAddr);

  //Wait for READ flag to be set
  while (1)
  {
    status = readRegister(AS72XX_SLAVE_STATUS_REG);
    if ((status & AS72XX_SLAVE_RX_VALID) != 0) break; // Read data is ready.
    delay(POLLING_DELAY);
  }

  byte incoming = readRegister(AS72XX_SLAVE_READ_REG);
  return (incoming);
}

//Write to a virtual register in the AS726x
void virtualWriteRegister(byte virtualAddr, byte dataToWrite)
{
  byte status;

  //Wait for WRITE register to be empty
  while (1)
  {
    status = readRegister(AS72XX_SLAVE_STATUS_REG);
    if ((status & AS72XX_SLAVE_TX_VALID) == 0) break; // No inbound TX pending at slave. Okay to write now.
    delay(POLLING_DELAY);
  }

  // Send the virtual register address (setting bit 7 to indicate we are writing to a register).
  writeRegister(AS72XX_SLAVE_WRITE_REG, (virtualAddr | 0x80));

  //Wait for WRITE register to be empty
  while (1)
  {
    status = readRegister(AS72XX_SLAVE_STATUS_REG);
    if ((status & AS72XX_SLAVE_TX_VALID) == 0) break; // No inbound TX pending at slave. Okay to write now.
    delay(POLLING_DELAY);
  }

  // Send the data to complete the operation.
  writeRegister(AS72XX_SLAVE_WRITE_REG, dataToWrite);
}

//Reads from a give location from the AS726x
byte readRegister(byte addr)
{
  Wire.beginTransmission(AS726X_ADDR);
  Wire.write(addr);
  Wire.endTransmission();

  Wire.requestFrom(AS726X_ADDR, 1);
  if (Wire.available()) return (Wire.read());

  Serial.println("I2C Error");
  return (0xFF); //Error
}

//Write a value to a spot in the AS726x
void writeRegister(byte addr, byte val)
{
  Wire.beginTransmission(AS726X_ADDR);
  Wire.write(addr);
  Wire.write(val);
  Wire.endTransmission();
}

