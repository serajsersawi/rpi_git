/*
 * ADXL345.h  Created on: 17 May 2014
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

#ifndef I2C_DEVICE_DS3231_H_
#define I2C_DEVICE_DS3231_H_
#include"i2c_device.h"

/// The ADXL345 has 0x40 registers (0x01 to 0x1C are reserved and should not be accessed)
#define BUFFER_SIZE 0x40

namespace i2c {

/**
 * @class ADXL345
 * @brief Specific class for the ADXL345 Accelerometer that is a child of the I2CDevice class
 * Protected inheritance means that the public I2CDevice methods are not publicly accessible
 * by an object of the ADXL345 class.
 */
 
#define DS3231_REGISTER_SECONDS_DEFAULT                       0X00
#define DS3231_REGISTER_MINUTES_DEFAULT                       0X00
#define DS3231_REGISTER_HOURS_DEFAULT                         0X00
#define DS3231_REGISTER_DAY_OF_WEEK_DEFAULT                   0X01
#define DS3231_REGISTER_DATE_DEFAULT                          0X01
#define DS3231_REGISTER_MONTH_DEFAULT                         0X01
#define DS3231_REGISTER_YEAR_DEFAULT                          0X00
#define DS3231_REGISTER_ALARM1_SECONDS_DEFAULT                0X00
#define DS3231_REGISTER_ALARM1_MINUTES_DEFAULT                0X00            
#define DS3231_REGISTER_ALARM1_HOURS_DEFAULT                  0X00
#define DS3231_REGISTER_ALARM1_DAY_OF_WEEK_OR_DATE_DEFAULT    0X00
#define DS3231_REGISTER_ALARM2_MINUTES_DEFAULT                0X00
#define DS3231_REGISTER_ALARM2_HOURS_DEFAULT                  0X00    
#define DS3231_REGISTER_ALARM2_DAY_OF_WEEK_OR_DATE_DEFAULT    0X00
#define DS3231_REGISTER_CONTROL_DEFAULT                       0X1C
#define DS3231_REGISTER_CONTROL_STATUS_DEFAULT                0X00
#define DS3231_REGISTER_AGING_OFFSET_DEFAULT                  0X00

class i2c_device_ds3231:protected i2c_device{
	
	enum MAP {SECOND, MINUTE, HOUR, DAY_OF_WEEK, DATE, MONTH, YEAR, CONTROL, CONTROL_STATUS, AGING_OFFSET, ALARM1, ALARM2, ALARMS, TEMPERATURE, TIME, ALL};
	enum SQR_WAVES {
		WAVE_1, //1KHz
		WAVE_2,	//1.024kHz
		WAVE_3, //4.096kHz
		WAVE_4	//8.192kHz 
		};
	enum RUNCLK_STATE{CLOCK_HALT, CLOCK_RUN};
	/*This enum is redundant, the register map in the cpp file is used instead*/
	enum DS3231_REG {
		DS3231_REGISTER_SECONDS, 
		DS3231_REGISTER_MINUTES, 
		DS3231_REGISTER_HOURS, 
		DS3231_REGISTER_DAY_OF_WEEK, 
		DS3231_REGISTER_DATE, 
		DS3231_REGISTER_MONTH, 
		DS3231_REGISTER_YEAR, 
		DS3231_REGISTER_ALARM1_SECONDS,
		DS3231_REGISTER_ALARM1_MINUTES,
		DS3231_REGISTER_ALARM1_HOURS,
		DS3231_REGISTER_ALARM1_DAY_OF_WEEK_OR_DATE,
		DS3231_REGISTER_ALARM2_MINUTES,
		DS3231_REGISTER_ALARM2_HOURS,
		DS3231_REGISTER_ALARM2_DAY_OF_WEEK_OR_DATE,
		DS3231_REGISTER_CONTROL,
		DS3231_REGISTER_CONTROL_STATUS,
		DS3231_REGISTER_AGING_OFFSET,
		DS3231_REGISTER_ALARM2_TEMP_MSB,
		DS3231_REGISTER_ALARM2_TEMP_LSB
	};
	// bit 6 of the hours register
	enum HOUR_MODE { 
		TWENTYFOUR,
		TWELVE
	};
	//User defined sequence for days of the week
	//Could be changed but must be sequential
	enum DAY_OF_WEEK { 
		MONDAY = 1,
		TUESDAY,
		WEDNESDAY,
		THURSDAY,
		FRIDAY,
		SATURDAY,
		SUNDAY
	};
	
private:
	/*private function*/
	unsigned int I2CBus, I2CAddress;
	unsigned char *registers;
	i2c_device_ds3231::HOUR_MODE hr_mode;
	i2c_device_ds3231::SQR_WAVES wave;
	i2c_device_ds3231::RUNCLK_STATE clk;
	
	unsigned int seconds, minutes, hours, day, date, month, year; // raw 2's complement values
	
	unsigned int temperature;

	static unsigned int register_current_value;        /*used to read current values of ds3231 registers*/
	static unsigned int register_new_value;        /*used to write new values to ds3231 registers*/
	virtual int updateAllRegisters();
	virtual int resetAllRegisters();

public:
	/*public functions APIs*/
	i2c_device_ds3231(unsigned int I2CBus, unsigned int I2CAddress=0x68);
	virtual int initUpdateAllRegisters();


	
	static int decimalToBCD(int decimalNumber) {
        int tens = decimalNumber / 10;
        int ones = decimalNumber % 10;
        return (tens << 4) | ones;
    }

	// Debugging method to display and update the pitch/roll on the one line
	virtual void displayTimeAndDate();
	virtual void displayTemperature();
	virtual ~i2c_device_ds3231();
};

} /* namespace i2c */

#endif /* I2C_DEVICE_DS3231_H_ */