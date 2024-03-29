

#ifndef I2C_DEVICE_DS3231_H_
#define I2C_DEVICE_DS3231_H_
#include"i2c_device.h"



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
public:
	enum MAP {SECOND, MINUTE, HOUR, DAY_OF_WEEK, DATE, MONTH, YEAR, CONTROL, CONTROL_STATUS, AGING_OFFSET, ALARM1, ALARM2, ALARMS, TEMPERATURE, TIME, ALL};
	enum SQR_WAVES {
		WAVE_1, //1KHz
		WAVE_2,	//1.024kHz
		WAVE_3, //4.096kHz
		WAVE_4	//8.192kHz 
		};
	enum RUNCLK_STATE{CLOCK_HALT, CLOCK_RUN};
	/*This enum is redundant, the register map in the cpp file is used instead*/
	
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
	
	enum STATUS { 
		OK,
		ERROR
	};
private:
	/*private function*/
	unsigned int I2CBus, I2CAddress;
	unsigned char *registers;
	i2c_device_ds3231::HOUR_MODE hr_mode;
	i2c_device_ds3231::AFTER_BEFORE_NOON am_pm;
	i2c_device_ds3231::SQR_WAVES wave;
	i2c_device_ds3231::RUNCLK_STATE clk;
	
	//they are always in decimal format
	unsigned int seconds, minutes, hours, day, date, month; // raw 2's complement values
	int year;
	bool isLeapYear;
	float temperature;

/* 	virtual int updateAllRegisters();
	virtual int resetAllRegisters(); */

	virtual unsigned int getSeconds();
	virtual unsigned int getMinutes();
	virtual unsigned int getHours();
	virtual unsigned int getDay();
	virtual unsigned int getDate();
	virtual unsigned int getMonth();
	virtual int			 getYear();
	
	virtual unsigned int setSeconds	(unsigned int seconds);
	virtual unsigned int setMinutes	(unsigned int minutes);
	virtual unsigned int setHours	(unsigned int hours);
	virtual unsigned int setDay		(unsigned int day);
	virtual unsigned int setDate	(unsigned int date);
	virtual unsigned int setMonth	(unsigned int month);
	virtual int			 setYear	(int year);
	
	
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
	
	static unsigned char decimalToBCD(int decimal);



	virtual void displayTimeAndDate();
	virtual int displayTemperature();
	
	virtual void changeHrMode(unsigned int mode);
	virtual void setTimeAndDate(unsigned int hours, unsigned int minutes, unsigned int seconds, unsigned int date, unsigned int month, int year);
	//time is only set by user in 24 format but it will retain the current format for time
	virtual void setTime(unsigned int hours, unsigned int minutes, unsigned int seconds);
	virtual void setDate(unsigned int date, unsigned int month, int year);
	
	virtual ~i2c_device_ds3231();
};

} /* namespace i2c */

#endif /* I2C_DEVICE_DS3231_H_ */