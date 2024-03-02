

#ifndef I2C_DEVICE_DS3231_H_
#define I2C_DEVICE_DS3231_H_
#include"i2c_device.h"


//From fig 1. of the DS3231 Data sheet (page 11)
//Register address mapping
//Since the addresses are sequential, an enum can also be used.
#define SECONDS_REG        		0x00  
#define MINUTES_REG    	   		0x01  
#define HOURS_REG          		0x02  
#define DAY_REG            		0x03  
#define DATE_REG           		0x04  
#define MONTH_CENT_REG     		0x05  
#define YEAR_REG           		0x06  
#define ALARM1_SEC_REG     		0x07  
#define ALARM1_MIN_REG     		0x08  
#define ALARM1_HR_REG           0x09  
#define ALARM1_DAY_DATE_REG     0x0A  
#define ALARM2_MIN_REG     		0x0B  
#define ALARM2_HR_REG      		0x0C  
#define ALARM2_DAY_DATE_REG    	0x0D  
#define CTRL_REG           		0x0E  
#define CTRL_STAT_REG      		0x0F  
#define AGING_OFFSET_REG   		0x10  
#define TEMP_MSB_REG       		0x11  
#define TEMP_LSB_REG       		0x12 

#define SECONDS_DEFAULT                       0X00
#define MINUTES_DEFAULT                       0X00
#define HOURS_DEFAULT                         0X00
#define DAY_OF_WEEK_DEFAULT                   0X01
#define DATE_DEFAULT                          0X01
#define MONTH_DEFAULT                         0X01
#define YEAR_DEFAULT                          0X00
#define ALARM1_SECONDS_DEFAULT                0X00
#define ALARM1_MINUTES_DEFAULT                0X00            
#define ALARM1_HOURS_DEFAULT                  0X00
#define ALARM1_DAY_OF_WEEK_OR_DATE_DEFAULT    0X00
#define ALARM2_MINUTES_DEFAULT                0X00
#define ALARM2_HOURS_DEFAULT                  0X00    
#define ALARM2_DAY_OF_WEEK_OR_DATE_DEFAULT    0X00
#define CONTROL_DEFAULT                       0X1C
#define CONTROL_STATUS_DEFAULT                0X00
#define AGING_OFFSET_DEFAULT                  0X00

//alarm match modes
#define ONCE_PER_SECOND	0x0F
#define S_MATCH			0x0E
#define MS_MATCH		0x0C
#define HMS_MATCH		0x08
#define DHMS_MATCH		0x00

//setting register target
#define RTC_REGS			0x00
#define ALARM1_REGS		0x01
#define ALARM2_REGS		0x02

//Alarm no setting


namespace i2c {
	
class i2c_device_ds3231:protected i2c_device{
public:
	enum SQR_WAVES {
		WAVE_1 = 0, //1KHz
		WAVE_2,	//1.024kHz
		WAVE_3, //4.096kHz
		WAVE_4	//8.192kHz 
	};
	enum RUNCLK_STATE{CLOCK_HALT, CLOCK_RUN};
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
	//Alarm type
	enum ALARM_TYPE {
		DAY_OF_MONTH = 0, //DT*
		DAY_OF_WEEK	  //DY	 
	};
	
	enum ALARM_NO {
		ALARM1 = 1, 
		ALARM2	   
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
	
	//accepting the register as a parameter allows those function to be valid for all types of registers
	//this was an update while working on the alarms 
	virtual unsigned int setSeconds	(unsigned int seconds, unsigned int reg);
	virtual unsigned int setMinutes	(unsigned int minutes, unsigned int reg);
	virtual unsigned int setHours	(unsigned int hours, unsigned int reg);
	virtual unsigned int setDay		(unsigned int day, unsigned int reg);
	virtual unsigned int setDate	(unsigned int date, unsigned int reg);
	virtual unsigned int setMonth	(unsigned int month);
	virtual int			 setYear	(int year);
	
	virtual void enableAlarm1();
	//virtual void enableAlarm2();
	virtual void enableAlarm(ALARM_NO enableAlarmNo);
	virtual void disableAlarm(ALARM_NO disableAlarmNo);
	virtual void clearAlarmFlag(ALARM_NO clearFlagAlarmNo);
	virtual void setAlarm1Time(unsigned int A1_hours, unsigned int A1_minutes, unsigned int A1_seconds);
	virtual void setAlarm1DayDate(unsigned int A1_date, unsigned int A1_day, ALARM_TYPE A1_dom_dow);
	virtual void setAlarmTime(ALARM_NO setAlarmNo, unsigned int setAlarmHours, unsigned int setAlarmMinutes, unsigned int setAlarmSeconds);
	virtual void setAlarmDayDate(ALARM_NO setAlarmNo, unsigned int setAlarmDate, unsigned int setAlarmDay, ALARM_TYPE setAlarmDayOrDate);
	virtual void setAlarmMaskBits(ALARM_NO maskAlarmNo, ALARM_TYPE maskDayOrDate, unsigned int maskAlarmMatchMode);
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
	
	void dumpRegisters(unsigned int number = 0xff) {
        debugDumpRegisters(number); // Calls the public function from the base class

    }
	
	static unsigned char decimalToBCD(int decimal);



	virtual void displayTimeAndDate();
	virtual int displayTemperature();
	
	virtual void changeHrMode(unsigned int mode, unsigned int reg);
	virtual void setTimeAndDate(unsigned int hours, unsigned int minutes, unsigned int seconds, unsigned int date, unsigned int month, int year);
	//time is only set by user in 24 format but it will retain the current format for time
	virtual void setTime(unsigned int hours, unsigned int minutes, unsigned int seconds);
	virtual void setDate(unsigned int date, unsigned int month, int year);
	virtual void startSquareWave(SQR_WAVES wave);
 	virtual void stopSquareWave(); 
	
	virtual void setAlarm1(ALARM_TYPE A1_dom_dow, unsigned int A1_match_mode, unsigned int A1_date, unsigned int A1_day, unsigned int A1_hours, unsigned int A1_minutes, unsigned int A1_seconds);
	virtual void setAlarm(ALARM_NO alarmNo, ALARM_TYPE dayOrDate, unsigned int alarmMatchMode, unsigned int alarmDate, unsigned int alarmDay, unsigned int alarmHours, unsigned int alarmMinutes, unsigned int alarmSeconds);
	
	virtual ~i2c_device_ds3231();
};

} /* namespace i2c */

#endif /* I2C_DEVICE_DS3231_H_ */