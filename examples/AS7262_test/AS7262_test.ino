//TODO: write header

#include <Wire.h>
#include "Adafruit_AS726x.h"

//create the object
Adafruit_AS726x ams;

//buffer to hold raw values
uint16_t sensorValues[AS726x_NUM_CHANNELS];
//buffer to hold calibrated values
float calibratedValues[AS726x_NUM_CHANNELS];

void setup() {
  Serial.begin(9600);
  while(!Serial);
  
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  //begin and make sure we can talk to the sensor
  if(!ams.begin()){
    Serial.println("could not connect to sensor! Please check your wiring.");
    while(1);
  }
}

void loop() {

  //flash the indicator LED
  ams.indicatorOn();
  delay(1000);
  ams.indicatorOff();
  delay(1000);

  //read the device temperature
  uint8_t temp = ams.readTemperature();
  
  //ams.drvOn(); //uncomment this if you want to use the driver LED for readings
  ams.startMeasurement(); //begin a measurement
  
  //wait till data is available
  bool rdy = false;
  while(!rdy){
    delay(5);
    rdy = ams.dataAvailable();
  }
  //ams.drvOff(); //uncomment this if you want to use the driver LED for readings

  //read the values!
  ams.readRawValues(sensorValues);
  ams.readCalibratedValues(calibratedValues);

  Serial.print("Temp: "); Serial.print(temp);
  Serial.print(" Violet: "); Serial.print(sensorValues[VIOLET]);
  Serial.print(" Blue: "); Serial.print(sensorValues[BLUE]);
  Serial.print(" Green: "); Serial.print(sensorValues[GREEN]);
  Serial.print(" Yellow: "); Serial.print(sensorValues[YELLOW]);
  Serial.print(" Orange: "); Serial.print(sensorValues[ORANGE]);
  Serial.print(" Red: "); Serial.print(sensorValues[RED]);
  Serial.println();
  Serial.println();
}
