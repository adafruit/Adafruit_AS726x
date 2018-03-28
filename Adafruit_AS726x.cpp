/*!
 * @file Adafruit_AS726x.cpp
 *
 * @mainpage Adafruit AS726x spectral sensor
 *
 * @section intro_sec Introduction
 *
 *  Driver for the AS726x family of spectral sensors
 *
 *  This is a library for the Adafruit AS726x breakout
 *  ----> https://www.adafruit.com/products/3779
 *    
 *  Adafruit invests time and resources providing this open source code, 
 *  please support Adafruit and open-source hardware by purchasing 
 *  products from Adafruit!
 *
 * @section author Author
 *
 * Written by Dean Miller for Adafruit Industries.
 *
 * @section license License
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */

#include "Adafruit_AS726x.h"

/**************************************************************************/
/*! 
    @brief  Set up hardware and begin communication with the sensor
    @param theWire a TwoWire object to use for I2C communication
    @return true on success, fale otherwise.
*/
/**************************************************************************/
bool Adafruit_AS726x::begin(TwoWire *theWire)
{
	_i2c = theWire;
	_i2c_init();
	
	_control_setup.RST = 1;
	virtualWrite(AS726X_CONTROL_SETUP, _control_setup.get());
	_control_setup.RST = 0;
	
	//wait for it to boot up
	delay(1000);
	
	//try to read the version reg to make sure we can connect
	uint8_t version = virtualRead(AS726X_HW_VERSION);
	
	//TODO: add support for other devices
	if(version != 0x40) return false;

	enableInterrupt();
	
	setDrvCurrent(LIMIT_12MA5);
	drvOff();
	
	setIntegrationTime(50);
	
	setGain(GAIN_64X);
	
	setConversionType(ONE_SHOT);
	
	return true;
}

/**************************************************************************/
/*! 
    @brief  turn on the driver LED
*/
/**************************************************************************/
void Adafruit_AS726x::drvOn()
{
	_led_control.LED_DRV = 1;
	virtualWrite(AS726X_LED_CONTROL, _led_control.get());
}

/**************************************************************************/
/*! 
    @brief  turn off the driver LED
*/
/**************************************************************************/
void Adafruit_AS726x::drvOff()
{
	_led_control.LED_DRV = 0;
	virtualWrite(AS726X_LED_CONTROL, _led_control.get());
}

/**************************************************************************/
/*! 
    @brief  set the current limit for the driver LED.
    @param current the current limit setting. Should be one of LIMIT_12MA5, LIMIT_25MA, LIMIT_50MA, or LIMIT_100MA = 0b11.
*/
/**************************************************************************/
void Adafruit_AS726x::setDrvCurrent(uint8_t current)
{
	_led_control.ICL_DRV = current;
	virtualWrite(AS726X_LED_CONTROL, _led_control.get());
}

/**************************************************************************/
/*! 
    @brief  turn on/off the indicator LED
    @param  on True if you want the LED on, False to turn off
*/
/**************************************************************************/
void Adafruit_AS726x::indicateLED(boolean on)
{
  _led_control.LED_IND = on;
  virtualWrite(AS726X_LED_CONTROL, _led_control.get());
}


/**************************************************************************/
/*! 
    @brief  set the current limit for the driver LED.
    @param current the current limit setting. Should be one of LIMIT_1MA, LIMIT_2MA, LIMIT_4MA, or LIMIT_8MA
*/
/**************************************************************************/
void Adafruit_AS726x::setIndicateCurrent(uint8_t current)
{
	_led_control.ICL_IND = current;
	virtualWrite(AS726X_LED_CONTROL, _led_control.get());
}

/**************************************************************************/
/*! 
    @brief  Set the conversion mode.
    @param type the mode to set the sensor to. Should be one of MODE_0, MODE_1, MODE_2, ONE_SHOT.
*/
/**************************************************************************/
void Adafruit_AS726x::setConversionType(uint8_t type)
{
	_control_setup.BANK = type;
	virtualWrite(AS726X_CONTROL_SETUP, _control_setup.get());
}

/**************************************************************************/
/*! 
    @brief  Set the sensor gain.
    @param gain the gain to set the sensor to. Should be one of GAIN_1X, GAIN_3X7, GAIN_16X, or GAIN_64X = 0b11.
*/
/**************************************************************************/
void Adafruit_AS726x::setGain(uint8_t gain)
{
	_control_setup.GAIN = gain;
	virtualWrite(AS726X_CONTROL_SETUP, _control_setup.get());
}

/**************************************************************************/
/*! 
    @brief  Set the integration time for the sensor.
    @param time the integration time to set. The actual integration time will be time*2.8ms
*/
/**************************************************************************/
void Adafruit_AS726x::setIntegrationTime(uint8_t time)
{
	_int_time.INT_T = time;
	virtualWrite(AS726X_INT_T, _int_time.get());
}

/**************************************************************************/
/*! 
    @brief  enable the device interrupt
*/
/**************************************************************************/
void Adafruit_AS726x::enableInterrupt()
{
	_control_setup.INT = 1;
	virtualWrite(AS726X_CONTROL_SETUP, _control_setup.get());
}

/**************************************************************************/
/*! 
    @brief  disable the device interrupt
*/
/**************************************************************************/
void Adafruit_AS726x::disableInterrupt()
{
	_control_setup.INT = 0;
	virtualWrite(AS726X_CONTROL_SETUP, _control_setup.get());
}

/**************************************************************************/
/*! 
    @brief  begin a measurement. This sets the conversion mode to ONE_SHOT.
*/
/**************************************************************************/
void Adafruit_AS726x::startMeasurement()
{
	_control_setup.DATA_RDY = 0;
	virtualWrite(AS726X_CONTROL_SETUP, _control_setup.get());
	
	setConversionType(ONE_SHOT);
}


/**************************************************************************/
/*! 
    @brief  read an individual raw spectral channel
    @param channel the channel to read
    @return the reading as a raw 16-bit integer
*/
/**************************************************************************/
uint16_t Adafruit_AS726x::readChannel(uint8_t channel)
{
	return (virtualRead(channel) << 8) | virtualRead(channel + 1);
}

/**************************************************************************/
/*! 
    @brief  read the raw channels
    @param buf the buffer to read the data into
    @param num Optional number of channels to read. Defaults to AS726x_NUM_CHANNELS
*/
/**************************************************************************/
void Adafruit_AS726x::readRawValues(uint16_t *buf, uint8_t num)
{
	for(int i = 0; i < num; i++){
		switch(i){
			case AS726x_VIOLET:
				buf[i] = readViolet();
				break;
			case AS726x_BLUE:
				buf[i] = readBlue();
				break;
			case AS726x_GREEN:
				buf[i] = readGreen();
				break;
			case AS726x_YELLOW:
				buf[i] = readYellow();
				break;
			case AS726x_ORANGE:
				buf[i] = readOrange();
				break;
			case AS726x_RED:
				buf[i] = readRed();
				break;
			default:
				break;
		}
	}
}

/**************************************************************************/
/*! 
    @brief  read the calibrated channels
    @param buf the buffer to read the data into
    @param num Optional number of channels to read. Defaults to AS726x_NUM_CHANNELS
*/
/**************************************************************************/
void Adafruit_AS726x::readCalibratedValues(float *buf, uint8_t num){
	for(int i = 0; i < num; i++){
		switch(i){
			case AS726x_VIOLET:
			buf[i] = readCalibratedViolet();
			break;
			case AS726x_BLUE:
			buf[i] = readCalibratedBlue();
			break;
			case AS726x_GREEN:
			buf[i] = readCalibratedGreen();
			break;
			case AS726x_YELLOW:
			buf[i] = readCalibratedYellow();
			break;
			case AS726x_ORANGE:
			buf[i] = readCalibratedOrange();
			break;
			case AS726x_RED:
			buf[i] = readCalibratedRed();
			break;
			default:
			break;
		}
	}
}


/**************************************************************************/
/*! 
    @brief  read an individual calibrated spectral channel
    @param channel the channel to read
    @return the reading as a raw 16-bit integer
*/
/**************************************************************************/
float Adafruit_AS726x::readCalibratedValue(uint8_t channel)
{
	uint32_t val = 0;
	val = ((uint32_t)virtualRead(channel) << 24) | ((uint32_t)virtualRead(channel + 1) << 16) | ((uint32_t)virtualRead(channel + 2) << 8) | (uint32_t)virtualRead(channel + 3);

	float ret;
	memcpy(&ret, &val, 4);
	return ret;
}

void Adafruit_AS726x::write8(byte reg, byte value)
{
	this->write(reg, &value, 1);
}

uint8_t Adafruit_AS726x::read8(byte reg)
{
	uint8_t ret;
	this->read(reg, &ret, 1);
	
	return ret;
}

uint8_t Adafruit_AS726x::virtualRead(uint8_t addr)
{
	volatile uint8_t status, d;
	while (1)
	{
	// Read slave I²C status to see if the read buffer is ready.
	status = read8(AS726X_SLAVE_STATUS_REG);
	if ((status & AS726X_SLAVE_TX_VALID) == 0)
	// No inbound TX pending at slave. Okay to write now.
	break;
	}
	// Send the virtual register address (setting bit 7 to indicate a pending write).
	write8(AS726X_SLAVE_WRITE_REG, addr);
	while (1)
	{
	// Read the slave I²C status to see if our read data is available.
	status = read8(AS726X_SLAVE_STATUS_REG);
	if ((status & AS726X_SLAVE_RX_VALID) != 0)
	// Read data is ready.
	break;
	}
	// Read the data to complete the operation.
	d = read8(AS726X_SLAVE_READ_REG);
	return d;
}

void Adafruit_AS726x::virtualWrite(uint8_t addr, uint8_t value)
{
	volatile uint8_t status;
	while (1)
	{
	// Read slave I²C status to see if the write buffer is ready.
	status = read8(AS726X_SLAVE_STATUS_REG);
	if ((status & AS726X_SLAVE_TX_VALID) == 0)
	// No inbound TX pending at slave. Okay to write now.
	break;
	}
	// Send the virtual register address (setting bit 7 to indicate a pending write).
	write8(AS726X_SLAVE_WRITE_REG, (addr | 0x80));
	//Serial.print("Address $"); Serial.print(addr, HEX);
	while (1)
	{
	// Read the slave I²C status to see if the write buffer is ready.
	status = read8(AS726X_SLAVE_STATUS_REG);
	if ((status & AS726X_SLAVE_TX_VALID) == 0)
	// No inbound TX pending at slave. Okay to write data now.
	break;
	}
	// Send the data to complete the operation.
	write8(AS726X_SLAVE_WRITE_REG, value);
	//Serial.print(" = 0x"); Serial.println(value, HEX);
}

void Adafruit_AS726x::read(uint8_t reg, uint8_t *buf, uint8_t num)
{
	uint8_t value;
	uint8_t pos = 0;
	
	//on arduino we need to read in 32 byte chunks
	while(pos < num){
		
		uint8_t read_now = min(32, num - pos);
		_i2c->beginTransmission((uint8_t)_i2caddr);
		_i2c->write((uint8_t)reg + pos);
		_i2c->endTransmission();
		_i2c->requestFrom((uint8_t)_i2caddr, read_now);
		
		for(int i=0; i<read_now; i++){
			buf[pos] = _i2c->read();
			pos++;
		}
	}
}
void Adafruit_AS726x::write(uint8_t reg, uint8_t *buf, uint8_t num)
{
	_i2c->beginTransmission((uint8_t)_i2caddr);
	_i2c->write((uint8_t)reg);
	_i2c->write((uint8_t *)buf, num);
	_i2c->endTransmission();
}
void Adafruit_AS726x::_i2c_init()
{
	_i2c->begin();
}
