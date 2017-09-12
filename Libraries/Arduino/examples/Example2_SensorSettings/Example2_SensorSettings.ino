/*
  This is a library written for the AS726X Spectral Sensor (Visible or IR) with I2C firmware
  specially loaded. SparkFun sells these at its website: www.sparkfun.com

  Written by Nathan Seidle & Andrew England @ SparkFun Electronics, July 12th, 2017

  https://github.com/sparkfun/Qwiic_Spectral_Sensor_AS726X

  Do you like this library? Help support SparkFun. Buy a board!

  Development environment specifics:
  Arduino IDE 1.8.1

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.


  ---AVAILABLE FUNCTIONS---
  AS726X(TwoWire &wirePort = Wire, byte gain = 3, byte measurementMode = 3);
  void takeMeasurements();
  void takeMeasurementsWithBulb();
  void printMeasurements();
  void printUncalibratedMeasurements();
  byte getTemperature();
  float getTemperatureF();
  void setMeasurementMode(byte mode);
  boolean dataAvailable();
  void enableIndicator();
  void disableIndicator();
  void setIndicatorCurrent(byte current);
  void enableBulb();
  void disableBulb();
  void setBulbCurrent(byte current);
  void softReset();
  void setGain(byte gain);
  void setIntegrationTime(byte integrationValue);
  void enableInterrupt();
  void disableInterrupt();


  //Get the various color readings
  int getViolet();
  int getBlue();
  int getGreen();
  int getYellow();
  int getOrange();
  int getRed();

  //Get the various NIR readings
  int getR();
  int getS();
  int getT();
  int getU();
  int getV();
  int getW();

  //Returns the various calibration data
  float getCalibratedViolet();
  float getCalibratedBlue();
  float getCalibratedGreen();
  float getCalibratedYellow();
  float getCalibratedOrange();
  float getCalibratedRed();

  float getCalibratedR();
  float getCalibratedS();
  float getCalibratedT();
  float getCalibratedU();
  float getCalibratedV();
  float getCalibratedW();
*/
#include "AS726X.h"
AS726X sensor;
byte GAIN = 0;
byte MEASUREMENT_MODE = 0;

void setup() {
  sensor.begin(Wire, GAIN, MEASUREMENT_MODE);
/*//You can also create an instance that runs on a different port (For instance, if you are using a teensy). The Gain and Measurement mode can also be changed.
  //Uncomment the below code and comment the above call of "sensor.begin()" to customize these settings.
  byte GAIN = 3;
  //Sets the gain
  //Sets the gain value
  //Gain 0: 1x (power-on default)
  //Gain 1: 3.7x
  //Gain 2: 16x
  //Gain 3: 64x
  byte MEASUREMENT_MODE = 3;
  //Sets the measurement mode
  //Mode 0: Continuous reading of VBGY (7262) / STUV (7263)
  //Mode 1: Continuous reading of GYOR (7262) / RTUX (7263)
  //Mode 2: Continuous reading of all channels (power-on default)
  //Mode 3: One-shot reading of all channels
  sensor.begin(Wire, GAIN, MEASUREMENT_MODE);
  //New declaration of sensor with different values
*/
}

void loop() {
  sensor.takeMeasurements();
  sensor.printMeasurements();//Prints out all measurements (calibrated)
}
