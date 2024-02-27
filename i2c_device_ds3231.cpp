/*
 * ADXL345.cpp  Created on: 17 May 2014
 * Copyright (c) 2014 Derek Molloy (www.derekmolloy.ie)
 * Made available for the book "Exploring Raspberry Pi"
 * See: www.exploringrpi.com
 * Licensed under the EUPL V.1.1
 *
 * This Software is provided to You under the terms of the European 
 * Union Public License (the "EUPL") version 1.1 as published by the 
 * European Union. Any use of this Software, other than as authorized 
 * under this License is strictly prohibited (to the extent such use 
 * is covered by a right of the copyright holder of this Software).
 * 
 * This Software is provided under the License on an "AS IS" basis and 
 * without warranties of any kind concerning the Software, including 
 * without limitation merchantability, fitness for a particular purpose, 
 * absence of defects or errors, accuracy, and non-infringement of 
 * intellectual property rights other than copyright. This disclaimer 
 * of warranty is an essential part of the License and a condition for 
 * the grant of any rights to this Software.
 * 
 * For more details, see http://www.derekmolloy.ie/
 */

#include "i2c_device_ds3231.h"
#include <iostream>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <cstdint.h>

using namespace std;

namespace i2c {

//From fig 1. of the DS3231 Data sheet (page 11)
//Register address mapping
//Since the addresses are sequential, an enum can also be used.
#define SECONDS_REG        		0x00   //Device ID
#define MINUTES_REG    	   		0x01   //Tap Threshold
#define HOURS_REG          		0x02   //X-axis Offset
#define DAY_REG            		0x03   //Y-axis Offset
#define DATE_REG           		0x04   //Z-axis Offset
#define MONTH_CENT_REG     		0x05   //Tap duration
#define YEAR_REG           		0x06   //Tap latency
#define ALARM1_SEC_REG     		0x07   //Tap window
#define ALARM1_MIN_REG     		0x08   //Activity threshold
#define ALARM1_HR_REG           0x09   //Threshold inactivity
#define ALARM1_DAY_DATE_REG     0x0A   //Inactivity time
#define ALARM2_MIN_REG     		0x0B   //Activity threshold
#define ALARM2_HR_REG      		0x0C   //Threshold inactivity
#define ALARM2_DAY_DATE_REG    	0x0D   //Inactivity time
#define CTRL_REG           		0x0E   //Free-fall threshold
#define CTRL_STAT_REG      		0x0F   //Free-fall time
#define AGING_OFFSET_REG   		0x10   //Axis control for single tap/double tap
#define TEMP_MSB_REG       		0x11   //Source of single tap/double tap
#define TEMP_LSB_REG       		0x12   //Data rate and power mode control

/**
 * The constructor for the ADXL345 accelerometer object. It passes the bus number and the
 * device address (with is 0x53 by default) to the constructor of I2CDevice. All of the states
 * are initialized and the registers are updated.
 * @param I2CBus The bus number that the ADXL345 device is on - typically 0 or 1
 * @param I2CAddress The address of the ADLX345 device (default 0x53, but can be altered)
 */
 
/*Constructor to initialize the rtc module given the i2c bus and the address*/
/*Initialize the module to the default values
	Time and date:
	H_:M_:S_  D_ M_ Y___ 
	00:00:00  01/01/2000
	
	Control register set to -> (0x1C):
	EOSC	BBSWQ	VONC	RS2		RS1		INTC	A2IE 	A1IE
	1		0		0		0		1		0		1		0
	
	All other registers default to 0x00
 
*/
i2c_device_ds3231::i2c_device_ds3231(unsigned int I2CBus, unsigned int I2CAddress):
	I2CDevice(I2CBus, I2CAddress){   // this member initialisation list calls the parent constructor
	
	this->I2CAddress = I2CAddress;
	this->I2CBus = I2CBus;
	
	this->seconds = DS3231_REGISTER_SECONDS_DEFAULT;
	this->minutes = DS3231_REGISTER_MINUTES_DEFAULT;
	this->hours = DS3231_REGISTER_HOURS_DEFAULT;
	this->day = DS3231_REGISTER_DAY_OF_WEEK_DEFAULT;
	this->date = DS3231_REGISTER_DATE_DEFAULT;
	this->month = DS3231_REGISTER_MONTH_DEFAULT;
	this->year = DS3231_REGISTER_YEAR_DEFAULT;
	
	this->hr_mode = i2c_device_ds3231::TWENTYFOUR;
	this->wave = i2c_device_ds3231::WAVE_2;
	this->clk = i2c_device_ds3231::CLOCK_RUN;
		
	this->initUpdateAllRegisters();
}

//Update all registers (time and date)
int i2c_device_ds3231::initUpdateAllRegisters(){

   int check = 0;
   
   check += (this->writeRegister(SECONDS_REG, DS3231_REGISTER_SECONDS_DEFAULT)); //Seconds: 00
   check += (this->writeRegister(MINUTES_REG, DS3231_REGISTER_MINUTES_DEFAULT)); //Minutes: 00
   check += (this->writeRegister(HOURS_REG, DS3231_REGISTER_HOURS_DEFAULT)); //Hours: 00 | 24hr format
   check += (this->writeRegister(DAY_REG, DS3231_REGISTER_DAY_OF_WEEK_DEFAULT)); //User defined 01 Monday
   check += (this->writeRegister(DATE_REG, DS3231_REGISTER_DATE_DEFAULT)); //01
   check += (this->writeRegister(YEAR_REG, DS3231_REGISTER_YEAR_DEFAULT)); //2000
   
   return check;
}

i2c_device_ds3231::~i2c_device_ds3231() {}

/*********************************************************************************************/

/**
 * Useful debug method to display the pitch and roll values in degrees on a single standard output line
 * @param iterations The number of 0.1s iterations to take place.
 */
void i2c_device_ds3231::displayTimeAndDate(){

}

void i2c_device_ds3231::displayTemperature(){

}


} /* namespace exploringRPi */