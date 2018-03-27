/***************************************************************************
  This is a library for the Adafruit AS7262 6-Channel Visible Light Sensor

  This sketch reads the sensor and creates a color bar graph on a tiny TFT

  Designed specifically to work with the Adafruit AS7262 breakout and 160x18 tft
  ----> http://www.adafruit.com/products/3779
  ----> http://www.adafruit.com/product/3533
  
  These sensors use I2C to communicate. The device's I2C address is 0x49
  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!
  
  Written by Dean Miller for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include <Wire.h>
#include "Adafruit_AS726x.h"

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>


// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     10
#define TFT_RST    9  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to -1!
#define TFT_DC     8

#define SENSOR_MAX 5000

#define BLACK   0x0000
#define GRAY    0x8410
#define WHITE   0xFFFF
#define RED     0xF800
#define ORANGE  0xFA60
#define YELLOW  0xFFE0  
#define LIME    0x07FF
#define GREEN   0x07E0
#define CYAN    0x07FF
#define AQUA    0x04FF
#define BLUE    0x001F
#define MAGENTA 0xF81F
#define PINK    0xF8FF

uint16_t colors[] = {
  MAGENTA,
  BLUE,
  GREEN,
  YELLOW,
  ORANGE,
  RED
};

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

//create the object
Adafruit_AS726x ams;

//buffer to hold raw values (these aren't used by default in this example)
//uint16_t sensorValues[AS726x_NUM_CHANNELS];

//buffer to hold calibrated values
float calibratedValues[AS726x_NUM_CHANNELS];

uint16_t barWidth;

void setup() {
  
  Serial.begin(9600);

  tft.initR(INITR_MINI160x80);   // initialize a ST7735S chip, mini display
  tft.setRotation(3);

  tft.fillScreen(ST7735_BLACK);

  barWidth = tft.width() / AS726x_NUM_CHANNELS;
  
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  //begin and make sure we can talk to the sensor
  if(!ams.begin()){
    Serial.println("could not connect to sensor! Please check your wiring.");
    while(1);
  }
  
  ams.setConversionType(MODE_2);

  //uncomment this if you want to use the driver LED (off by default)
  //ams.drvOn();
}

void loop() {

  if(ams.dataReady()){
    
    //read the values!
    //ams.readRawValues(sensorValues);
    ams.readCalibratedValues(calibratedValues);
    
    for(int i=0; i<AS726x_NUM_CHANNELS; i++){
      uint16_t height = map(calibratedValues[i], 0, SENSOR_MAX, 0, tft.height());

      tft.fillRect(barWidth * i, 0, barWidth, tft.height() - height, ST7735_BLACK);
      tft.fillRect(barWidth * i, tft.height() - height, barWidth, height, colors[i]);
    }
  }
  
}

