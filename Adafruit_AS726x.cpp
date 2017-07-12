#include "Adafruit_AS726x.h"

bool Adafruit_AS726x::begin(uint8_t addr)
{
	_i2caddr = addr;
	
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
	
	setDrvCurrent(LIMIT_12MA5);
	drvOff();
	
	setIntegrationTime(50);
	
	setGain(GAIN_64X);
	
	setConversionType(ONE_SHOT);
	
	return true;
}
		
/*========= LED STUFF =========*/

//turn on the drv led
void Adafruit_AS726x::drvOn()
{
	_led_control.LED_DRV = 1;
	virtualWrite(AS726X_LED_CONTROL, _led_control.get());
}

//turn off the drv led
void Adafruit_AS726x::drvOff()
{
	_led_control.LED_DRV = 0;
	virtualWrite(AS726X_LED_CONTROL, _led_control.get());
}

//set current through drv led
void Adafruit_AS726x::setDrvCurrent(uint8_t current)
{
	_led_control.ICL_DRV = current;
	virtualWrite(AS726X_LED_CONTROL, _led_control.get());
}

/*===== END LED STUFF ======*/

void Adafruit_AS726x::setConversionType(uint8_t type)
{
	_control_setup.BANK = type;
	virtualWrite(AS726X_CONTROL_SETUP, _control_setup.get());
}
void Adafruit_AS726x::setGain(uint8_t gain)
{
	_control_setup.GAIN = gain;
	virtualWrite(AS726X_CONTROL_SETUP, _control_setup.get());
}
void Adafruit_AS726x::setIntegrationTime(uint8_t time)
{
	_int_time.INT_T = time;
	virtualWrite(AS726X_INT_T, _int_time.get());
}
void Adafruit_AS726x::enableInterrupt()
{
	_control_setup.INT = 1;
	virtualWrite(AS726X_CONTROL_SETUP, _control_setup.get());
}
void Adafruit_AS726x::disableInterrupt()
{
	_control_setup.INT = 0;
	virtualWrite(AS726X_CONTROL_SETUP, _control_setup.get());
}

/*====== MEASUREMENTS ========*/

//read sensor data
void Adafruit_AS726x::startMeasurement()
{
	_control_setup.DATA_RDY = 0;
	virtualWrite(AS726X_CONTROL_SETUP, _control_setup.get());
	
	setConversionType(ONE_SHOT);
}

uint16_t Adafruit_AS726x::readChannel(uint8_t channel)
{
	return (virtualRead(channel) << 8) | virtualRead(channel + 1);
}

void Adafruit_AS726x::readRawValues(uint16_t *buf, uint8_t num)
{
	for(int i = 0; i < num; i++){
		switch(i){
			case VIOLET:
				buf[i] = readViolet();
				break;
			case BLUE:
				buf[i] = readBlue();
				break;
			case GREEN:
				buf[i] = readGreen();
				break;
			case YELLOW:
				buf[i] = readYellow();
				break;
			case ORANGE:
				buf[i] = readOrange();
				break;
			case RED:
				buf[i] = readRed();
				break;
			default:
				break;
		}
	}
}

void Adafruit_AS726x::readCalibratedValues(float *buf, uint8_t num){
	for(int i = 0; i < num; i++){
		switch(i){
			case VIOLET:
			buf[i] = readCalibratedViolet();
			break;
			case BLUE:
			buf[i] = readCalibratedBlue();
			break;
			case GREEN:
			buf[i] = readCalibratedGreen();
			break;
			case YELLOW:
			buf[i] = readCalibratedYellow();
			break;
			case ORANGE:
			buf[i] = readCalibratedOrange();
			break;
			case RED:
			buf[i] = readCalibratedRed();
			break;
			default:
			break;
		}
	}
}

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
}

void Adafruit_AS726x::read(uint8_t reg, uint8_t *buf, uint8_t num)
{
	uint8_t value;
	uint8_t pos = 0;
	
	//on arduino we need to read in 32 byte chunks
	while(pos < num){
		
		uint8_t read_now = min(32, num - pos);
		Wire.beginTransmission((uint8_t)_i2caddr);
		Wire.write((uint8_t)reg + pos);
		Wire.endTransmission();
		Wire.requestFrom((uint8_t)_i2caddr, read_now);
		
		for(int i=0; i<read_now; i++){
			buf[pos] = Wire.read();
			pos++;
		}
	}
}
void Adafruit_AS726x::write(uint8_t reg, uint8_t *buf, uint8_t num)
{
	Wire.beginTransmission((uint8_t)_i2caddr);
	Wire.write((uint8_t)reg);
	Wire.write((uint8_t *)buf, num);
	Wire.endTransmission();
}
void Adafruit_AS726x::_i2c_init()
{
	Wire.begin();
}