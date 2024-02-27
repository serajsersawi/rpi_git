

#ifndef I2C_DEVICE_DS3231_H_
#define I2C_DEVICE_DS3231_H_
#include"i2c_device.h"

/// The ADXL345 has 0x40 registers (0x01 to 0x1C are reserved and should not be accessed)
#define BUFFER_SIZE 0x40

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

namespace i2c {


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
	
	enum AFTER_BEFORE_NOON { 
		AM,
		PM
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
	i2c_device_ds3231::AFTER_BEFORE_NOON am_pm;
	i2c_device_ds3231::SQR_WAVES wave;
	i2c_device_ds3231::RUNCLK_STATE clk;
	
	unsigned int seconds, minutes, hours, day, date, month; // raw 2's complement values
	int year;
	unsigned int temperature;

/* 	virtual int updateAllRegisters();
	virtual int resetAllRegisters(); */
	virtual unsigned int getSeconds();
	virtual unsigned int getMinutes();
	virtual unsigned int getHours();
	virtual unsigned int getDay();
	virtual unsigned int getDate();
	virtual unsigned int getMonth();
	virtual int			 getYear();
	
	virtual void changeHrMode(HOUR_MODE mode);

public:
	/*public functions APIs*/
	i2c_device_ds3231(unsigned int I2CBus, unsigned int I2CAddress=0x68);
	virtual int initUpdateAllRegisters();
	//those might be moved to private

	static unsigned int bcdToDec(unsigned char bcdValue) {
    unsigned int tens = (bcdValue >> 4) * 10;
    unsigned int ones = bcdValue & 0x0F;
    return tens + ones;
	}

	virtual void displayTimeAndDate();
	
	//virtual void displayTemperature();
	virtual ~i2c_device_ds3231();
};

} /* namespace i2c */

#endif /* I2C_DEVICE_DS3231_H_ */