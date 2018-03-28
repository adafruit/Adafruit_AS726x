/*!
 * @file Adafruit_AS726x.h
 *
 * This is a library for the Adafruit AS726x breakout board
 * ----> https://www.adafruit.com/products/3779
 * 
 * Adafruit invests time and resources providing this open source code, 
 * please support Adafruit and open-source hardware by purchasing 
 * products from Adafruit!
 *
 * Written by Dean Miller for Adafruit Industries.
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */

#ifndef LIB_ADAFRUIT_AS276X
#define LIB_ADAFRUIT_AS276X

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Wire.h>

/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
    #define AS726x_ADDRESS                (0x49) ///< default I2C address
/*=========================================================================*/


/**************************************************************************/
/*! 
    @brief  virtual registers
*/
/**************************************************************************/
	enum
	{
		 AS726X_HW_VERSION	=	0x00,
		 AS726X_FW_VERSION	=	0x02,
		 AS726X_CONTROL_SETUP =	0x04,
		 AS726X_INT_T		=	0x05,
		 AS726X_DEVICE_TEMP	 =	0x06,
		 AS726X_LED_CONTROL	 =	0x07,
		
		//for reading sensor data
		AS7262_V_HIGH	=		0x08,
		 AS7262_V_LOW		=	0x09,
		 AS7262_B_HIGH		=	0x0A,
		 AS7262_B_LOW		=	0x0B,
		 AS7262_G_HIGH		=	0x0C,
		 AS7262_G_LOW		=	0x0D,
		 AS7262_Y_HIGH		=	0x0E,
		 AS7262_Y_LOW		=	0x0F,
		 AS7262_O_HIGH		=	0x10,
		 AS7262_O_LOW		=	0x11,
		 AS7262_R_HIGH		=	0x12,
		 AS7262_R_LOW		=	0x13,
		
		 AS7262_V_CAL		=	0x14,
		 AS7262_B_CAL		=	0x18,
		 AS7262_G_CAL		=	0x1C,
		 AS7262_Y_CAL		=	0x20,
		 AS7262_O_CAL		=	0x24,
		 AS7262_R_CAL		=	0x28
	};
	
/**************************************************************************/
/*! 
    @brief  hardware registers
*/
/**************************************************************************/
	enum
	{
		AS726X_SLAVE_STATUS_REG = 0x00,
		AS726X_SLAVE_WRITE_REG = 0x01,
		AS726X_SLAVE_READ_REG = 0x02,
		AS726X_SLAVE_TX_VALID = 0x02,
		AS726X_SLAVE_RX_VALID = 0x01,
	};
	
/**************************************************************************/
/*! 
    @brief  color registers
*/
/**************************************************************************/
	enum {
		AS7262_VIOLET = 0x08,
		AS7262_BLUE = 0x0A,
		AS7262_GREEN = 0x0C,
		AS7262_YELLOW = 0x0E,
		AS7262_ORANGE = 0x10,
		AS7262_RED = 0x12,
		AS7262_VIOLET_CALIBRATED = 0x14,
		AS7262_BLUE_CALIBRATED = 0x18,
		AS7262_GREEN_CALIBRATED = 0x1C,
		AS7262_YELLOW_CALIBRATED = 0x20,
		AS7262_ORANGE_CALIBRATED = 0x24,
		AS7262_RED_CALIBRATED = 0x28,
	};
	
/**************************************************************************/
/*! 
    @brief  conversion modes. Default is Mode 2
*/
/**************************************************************************/
	enum conversion_types{
		MODE_0 = 0b00,
		MODE_1 = 0b01,
		MODE_2 = 0b10, //default
		ONE_SHOT = 0b11,
	};
	
/**************************************************************************/
/*! 
    @brief gain settings. Default is 1x gain
*/
/**************************************************************************/
	enum channel_gain {
		GAIN_1X = 0b00, //default
		GAIN_3X7 = 0b01,
		GAIN_16X = 0b10,
		GAIN_64X = 0b11,
	};
	
/**************************************************************************/
/*! 
    @brief  indicator LED current limit settings. Default is 1mA
*/
/**************************************************************************/
	enum ind_led_current_limits {
		LIMIT_1MA = 0b00, //default
		LIMIT_2MA = 0b01,
		LIMIT_4MA = 0b10,
		LIMIT_8MA = 0b11,
	};
	
/**************************************************************************/
/*! 
    @brief  Driver LED current limit settings. Default is 12.5 mA
*/
/**************************************************************************/
	enum drv_led_current_limits {
		LIMIT_12MA5 = 0b00, //default
		LIMIT_25MA = 0b01,
		LIMIT_50MA = 0b10,
		LIMIT_100MA = 0b11,
	};
	
	
/*=========================================================================*/

#define AS726x_INTEGRATION_TIME_MULT 2.8 ///< multiplier for integration time
#define AS726x_NUM_CHANNELS 6 ///< number of sensor channels

/**************************************************************************/
/*! 
    @brief  Color definitions used by the library
*/
/**************************************************************************/
	enum {
		AS726x_VIOLET = 0,
		AS726x_BLUE,
		AS726x_GREEN,
		AS726x_YELLOW,
		AS726x_ORANGE,
		AS726x_RED,
	};

/**************************************************************************/
/*! 
    @brief  Class that stores state and functions for interacting with AS726x spectral sensors
*/
/**************************************************************************/
class Adafruit_AS726x {
	public:
		/*! 
		    @brief  Class constructor
		    @param addr Optional I2C address the sensor can be found on. Defaults to 0x49.
		*/
		Adafruit_AS726x(int8_t addr = AS726x_ADDRESS) { _i2caddr = addr; };
		~Adafruit_AS726x(void) {};
		
		bool begin(TwoWire *theWire = &Wire);
		
		/*========= LED STUFF =========*/
		
		// Set indicator LED current
		void setIndicateCurrent(uint8_t current);
		// turn on/off indicator
		void indicateLED(boolean on);

		//turn on the drv led
		void drvOn();
		//turn off the drv led
		void drvOff();
		
		//set current through drv led
		void setDrvCurrent(uint8_t current);
		
		/*===== END LED STUFF ======*/
		
		void setConversionType(uint8_t type);
		void setGain(uint8_t gain);
		void setIntegrationTime(uint8_t time);
		void enableInterrupt();
		void disableInterrupt();
		
		/*====== MEASUREMENTS ========*/
		
		//read sensor data
		void startMeasurement();

		/*! 
		    @brief  Check if the sensor is ready to return data
		    @return true if data is ready to be read, false otherwise.
		*/
		bool dataReady() { return virtualRead(AS726X_CONTROL_SETUP) & 0x02; }
		
		/*! 
		    @brief  Read the on-board temperature sensor
		    @return the temperature in Centigrade.
		*/
		uint8_t readTemperature() { return virtualRead(AS726X_DEVICE_TEMP); }
		uint16_t readChannel(uint8_t channel);
		
		/*! 
		    @brief  Read raw violet color value (AS7262 only)
		    @return the violet reading as an unsigned 16-bit integer
		*/
		uint16_t readViolet() { return(readChannel(AS7262_VIOLET)); }
		/*! 
		    @brief  Read raw blue color value (AS7262 only)
		    @return the blue reading as an unsigned 16-bit integer
		*/
		uint16_t readBlue() { return(readChannel(AS7262_BLUE)); }
		/*! 
		    @brief  Read raw green color value (AS7262 only)
		    @return the green reading as an unsigned 16-bit integer
		*/
		uint16_t readGreen() { return(readChannel(AS7262_GREEN)); }
		/*! 
		    @brief  Read raw yellow color value (AS7262 only)
		    @return the yellow reading as an unsigned 16-bit integer
		*/
		uint16_t readYellow() { return(readChannel(AS7262_YELLOW)); }
		/*! 
		    @brief  Read raw orange color value (AS7262 only)
		    @return the orange reading as an unsigned 16-bit integer
		*/
		uint16_t readOrange() { return(readChannel(AS7262_ORANGE)); }
		/*! 
		    @brief  Read raw red color value (AS7262 only)
		    @return the red reading as an unsigned 16-bit integer
		*/
		uint16_t readRed() { return(readChannel(AS7262_RED)); }
			
		void readRawValues(uint16_t *buf, uint8_t num = AS726x_NUM_CHANNELS);
		
		float readCalibratedValue(uint8_t channel);
		
		/*! 
		    @brief  Read calibrated violet color value (AS7262 only)
		    @return the violet reading as a 32-bit floating point number
		*/
		float readCalibratedViolet() { return(readCalibratedValue(AS7262_VIOLET_CALIBRATED)); }
		/*! 
		    @brief  Read calibrated blue color value (AS7262 only)
		    @return the blue reading as a 32-bit floating point number
		*/
		float readCalibratedBlue() { return(readCalibratedValue(AS7262_BLUE_CALIBRATED)); }
		/*! 
		    @brief  Read calibrated green color value (AS7262 only)
		    @return the green reading as a 32-bit floating point number
		*/
		float readCalibratedGreen() { return(readCalibratedValue(AS7262_GREEN_CALIBRATED)); }
		/*! 
		    @brief  Read calibrated yellow color value (AS7262 only)
		    @return the yellow reading as a 32-bit floating point number
		*/
		float readCalibratedYellow() { return(readCalibratedValue(AS7262_YELLOW_CALIBRATED)); }
		/*! 
		    @brief  Read calibrated orange color value (AS7262 only)
		    @return the orange reading as a 32-bit floating point number
		*/
		float readCalibratedOrange() { return(readCalibratedValue(AS7262_ORANGE_CALIBRATED)); }
		/*! 
		    @brief  Read calibrated red color value (AS7262 only)
		    @return the red reading as a 32-bit floating point number
		*/
		float readCalibratedRed() { return(readCalibratedValue(AS7262_RED_CALIBRATED)); }
			
		void readCalibratedValues(float *buf, uint8_t num = AS726x_NUM_CHANNELS);
		
		/*==== END MEASUREMENTS =====*/
	
	private:
		TwoWire *_i2c;
		uint8_t _i2caddr; ///< the I2C address of the sensor
			
			void      write8(byte reg, byte value);
			uint8_t   read8(byte reg);
			
			uint8_t virtualRead(uint8_t addr);
			void virtualWrite(uint8_t addr, uint8_t value);
			
			void read(uint8_t reg, uint8_t *buf, uint8_t num);
			void write(uint8_t reg, uint8_t *buf, uint8_t num);
			void _i2c_init();
		
		struct control_setup {
			
			uint8_t unused : 1;
			
			/* 1: data ready to be read, sets int active if int is enabled */
			uint8_t DATA_RDY : 1;
			
			/* conversion type
			*  0b00 = Mode 0
			*  0b01 = Mode 1
			*  0b10 = Mode 2
			*  0b11 = Mode 3 One shot
			*/
			uint8_t BANK : 2;
			
			/* Channel gain setting (all channels)
			*  0b00 = 1x
			*  0b01 = 3.7x
			*  0b10 = 16x
			*  0b11 = 64x
			*/
			uint8_t GAIN : 2;
			
			/* enable or disable interrupt */
			uint8_t INT : 1;
			uint8_t RST : 1;

			uint8_t get() {
				return ( (DATA_RDY << 1) | (BANK << 2) | (GAIN << 4) | (INT << 6) | (RST << 7) );
			};
		};
		control_setup _control_setup;
		
		struct int_time {
			
			//integration time (multiplied by INTEGRATION_TIME_MULT) in ms
			uint8_t INT_T : 8;
			
			uint8_t get() {
				return INT_T;
			};
		};
		int_time _int_time;
		
		struct led_control {
			// enable or disable indicator LED
			uint8_t LED_IND : 1;
			
			//indicator led current limit
			uint8_t ICL_IND : 2;
			
			//enable or disable led_drv
			uint8_t LED_DRV : 1;
			
			uint8_t ICL_DRV : 2;
			
			uint8_t get() {
				return ( LED_IND | (ICL_IND << 1) | (LED_DRV << 3) | (ICL_DRV << 4) );
			};
		};
		led_control _led_control;
	
};

#endif
