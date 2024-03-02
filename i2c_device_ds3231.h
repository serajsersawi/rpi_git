/**
 * @file i2c_device_ds3231.h
 * This file is part of a library developed for interfacing with the DS3231 real-time clock on a Raspberry Pi.
 * The library provides functionalities to manage date, time, alarms, and other features of the DS3231 module.
 *
 * Author: Sarraj Alsersawi
 * Repository: https://github.com/serajsersawi/rpi_git.git
 * 
 * This work is inspired by and adapted from Derek Molloy's Exploring Raspberry Pi repository:
 * https://github.com/derekmolloy/exploringrpi.git
 * 
 * The I2C and LED libraries have been adapted from Derek Molloy's work to provide a comprehensive interface for the DS3231 RTC module,
 * incorporating advanced functionalities such as alarm management and square wave generation.
 *
 * Functions included in this library:
 * - Constructor: i2c_device_ds3231(unsigned int I2CBus, unsigned int I2CAddress=0x68)
 * - Destructor: ~i2c_device_ds3231()
 * - void displayTimeAndDate()
 * - int displayTemperature()
 * - void changeHrMode(unsigned int mode, unsigned int reg)
 * - void setTimeAndDate(unsigned int hours, unsigned int minutes, unsigned int seconds, unsigned int date, unsigned int month, int year)
 * - void setTime(unsigned int hours, unsigned int minutes, unsigned int seconds)
 * - void setDate(unsigned int date, unsigned int month, int year)
 * - void startSquareWave(SQR_WAVES wave)
 * - void stopSquareWave()
 * - void start32kHz()
 * - void stop32kHz()
 * - void setAlarm1(ALARM_TYPE A1_dom_dow, unsigned int A1_match_mode, unsigned int A1_date, unsigned int A1_day, unsigned int A1_hours, unsigned int A1_minutes, unsigned int A1_seconds)
 * - void setAlarm(ALARM_NO alarmNo, ALARM_TYPE dayOrDate, unsigned int alarmMatchMode, unsigned int alarmDate, unsigned int alarmDay, unsigned int alarmHours, unsigned int alarmMinutes, unsigned int alarmSeconds)
 * - unsigned int readAlarm(ALARM_NO readAlarmNo)
 * - void clearAlarmFlag(ALARM_NO clearFlagAlarmNo)
 * - void alarmSnooze(ALARM_NO snoozeAlarmNo)
 * - static unsigned int bcdToDec(unsigned char bcdValue)
 * - void dumpRegisters(unsigned int number = 0xff)
 * - static unsigned char decimalToBCD(int decimal)
 */
 

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
	
	//Always in decimal format
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
	virtual void setAlarm1Time(unsigned int A1_hours, unsigned int A1_minutes, unsigned int A1_seconds);
	virtual void setAlarm1DayDate(unsigned int A1_date, unsigned int A1_day, ALARM_TYPE A1_dom_dow);
	virtual void setAlarmTime(ALARM_NO setAlarmNo, unsigned int setAlarmHours, unsigned int setAlarmMinutes, unsigned int setAlarmSeconds);
	virtual void setAlarmDayDate(ALARM_NO setAlarmNo, unsigned int setAlarmDate, unsigned int setAlarmDay, ALARM_TYPE setAlarmDayOrDate);
	virtual void setAlarmMaskBits(ALARM_NO maskAlarmNo, ALARM_TYPE maskDayOrDate, unsigned int maskAlarmMatchMode);


public:

	/////////////////////////// User APIs ////////////////////////////
	/** i2c_device_ds3231
	* Constructs an i2c_device_ds3231 object that represents a DS3231 RTC module.
	* This constructor initializes the RTC module by setting up the I2C communication
	* parameters, including the bus number and device address. It sets the initial
	* state of the RTC module to the default time and date (00:00:00 01/01/2000)
	* and configures the control register with default settings.
	*
	* @param I2CBus The I2C bus number to which the DS3231 is connected. It is typically
	*               0 or 1 on systems that support multiple I2C buses.
	* @param I2CAddress The I2C address of the DS3231 device. The default address is 0x68,
	*                   but it can be changed if the module is configured differently.
	*/
	i2c_device_ds3231(unsigned int I2CBus, unsigned int I2CAddress=0x68);
	
	/** initUpdateAllRegisters
	* Initializes and updates all registers of the DS3231 device to default values,
	* including time, date, and control registers. This method sets the time and date
	* to 00:00:00 on 01/01/2000, and configures the control register with default settings.
	* It ensures the device starts with a known state. Returns a status code indicating
	* the success or failure of the initialization.
	* 
	* @return An integer indicating the success (0) or failure (non-zero) of the operation.
	*/
	virtual int initUpdateAllRegisters();

	/** displayTimeAndDate
	* @brief Displays the current time and date.
	* 
	* This method displays the current time and date in a formatted string. The format depends on the hour mode
	* (either 12-hour or 24-hour format). For a 12-hour format, the AM/PM suffix is added. It first updates
	* the internal state with the latest time and date from the DS3231 registers before displaying them.
	* 
	* @note No parameters are required and there is no return value for this void method.
	*/
	virtual void displayTimeAndDate();
	
	/** displayTemperature
	* @brief Displays the current temperature read from the DS3231's temperature register.
	* 
	* This function initiates a temperature conversion and waits until the conversion is complete.
	* It then reads the temperature registers of the DS3231, combines the MSB and LSB into a
	* 10-bit value, and converts this raw value into a temperature in degrees Celsius. The
	* temperature is printed to the standard output.
	* 
	* @return int Returns 0 on success.
	*/
	virtual int displayTemperature();
	
	/** changeHrMode
	* @brief Changes the hour mode (12-hour or 24-hour format) of the DS3231 RTC.
	* 
	* This method allows changing the time representation mode of the RTC. It can switch between
	* 12-hour format with AM/PM indication and 24-hour format. This setting affects how time is
	* displayed and set within the RTC. The function modifies the appropriate bit in the hours
	* register to reflect the selected mode.
	* 
	* @param mode Specifies the hour mode to set. Use HOUR_MODE::TWENTYFOUR for 24-hour format or HOUR_MODE::TWELVE for 12-hour format.
	* @param reg Specifies the target register for the mode change. This parameter is included for compatibility and future use but is not currently used in the function body.
	*/
	virtual void changeHrMode(unsigned int mode, unsigned int reg);
	
	/** setTimeAndDate
	* @brief Sets the current time and date on the DS3231 RTC.
	* 
	* This method updates the DS3231's internal registers to set the current time and date. It handles
	* the conversion from human-readable format to the format required by the RTC. This method is useful
	* for initializing the RTC with a specific date and time, such as during startup or when synchronizing
	* with an external time source.
	* 
	* @param hours The hour to set (0-23 for 24-hour format, 1-12 for 12-hour format depending on the current hour mode of the RTC).
	* @param minutes The minutes to set (0-59).
	* @param seconds The seconds to set (0-59).
	* @param date The date to set (1-31, depending on the month).
	* @param month The month to set (1-12).
	* @param year The year to set. The year should be provided in full format (e.g., 2024).
	*/
	virtual void setTimeAndDate(unsigned int hours, unsigned int minutes, unsigned int seconds, unsigned int date, unsigned int month, int year);
	
	/** setTime
	* @brief Sets the current time on the DS3231 RTC.
	* 
	* This method updates the DS3231's internal registers to set the current time, while retaining
	* the current date and hour format (24-hour or 12-hour mode). It is designed to update the time
	* without affecting the current date settings or switching the hour mode of the RTC.
	* 
	* Note: The time is set in 24-hour format regardless of the current hour mode setting on the DS3231.
	* If the DS3231 is in 12-hour mode, the time will still be correctly interpreted according to the
	* 24-hour format provided.
	* 
	* @param hours The hour to set in 24-hour format (0-23).
	* @param minutes The minutes to set (0-59).
	* @param seconds The seconds to set (0-59).
	*/
	virtual void setTime(unsigned int hours, unsigned int minutes, unsigned int seconds);
	
	/** setDate
	* Sets the date of the DS3231 RTC.
	* This method allows setting the day of the month, the month, and the year. It validates
	* the input to ensure the date is within valid ranges and adjusts the date to the default
	* (01/01/2000) if invalid data is provided.
	* 
	* @param date The day of the month (1-31).
	* @param month The month (1-12).
	* @param year The year (2000-2099).
	*/
	virtual void setDate(unsigned int date, unsigned int month, int year);
	
	/** startSquareWave
	* Starts the square wave output based on the specified frequency. 
	* This method configures the DS3231 to output a square wave signal on the SQW pin.
	* 
	* @param wave The frequency of the square wave output. 
	*             Acceptable values are defined by the SQR_WAVES enum, 
	*             including WAVE_1 for 1Hz,
	*						WAVE_2 for 1.024kHz,
	*						WAVE_3 for 4.096kHz,
	*						and WAVE_4 for 8.192kHz.
	*/
	virtual void startSquareWave(SQR_WAVES wave);
	
	/** stopSquareWave
	* Stops the square wave output that was previously started with `startSquareWave`.
	* 
	* @brief Stops square wave generation on the SQW pin of the DS3231.
	*/
 	virtual void stopSquareWave(); 
	
	/** start32kHz
	* Starts the 32kHz output on the dedicated pin of the DS3231.
	* 
	* @brief Activates the 32kHz output signal. This function enables the continuous 
	*        32kHz square wave signal output, which can be used for external devices 
	*        requiring a precise clock signal.
	* @note This function modifies the control status register to enable the 32kHz output.
	*       Ensure that the SQW pin is not used for square wave output when this mode is active.
	*/
	virtual void start32kHz();
	
	/** stop32kHz
	* Stops the 32kHz output on the dedicated pin of the DS3231.
	* 
	* @brief Deactivates the 32kHz output signal. This function stops the 32kHz square 
	*        wave signal output to conserve power or to use the SQW pin for square wave output.
	* @note This function modifies the control status register to disable the 32kHz output.
	*/
	virtual void stop32kHz(); 
	
	//Deprecated function was only used during the development phase :D
	virtual void setAlarm1(ALARM_TYPE A1_dom_dow, unsigned int A1_match_mode, unsigned int A1_date, unsigned int A1_day, unsigned int A1_hours, unsigned int A1_minutes, unsigned int A1_seconds);
	//
	
	/**setAlarm
	* Sets an alarm with specified parameters, including the type of alarm, match mode, date, day, hours, minutes, and seconds.
	* This method configures the alarm based on the day of month or day of week, with various match modes to specify the alarm trigger conditions.
	* 
	* @param alarmNo The alarm number (ALARM1 or ALARM2).
	* @param dayOrDate Indicates if the alarm is set by day of month (DAY_OF_MONTH) or day of week (DAY_OF_WEEK).
	* @param alarmMatchMode Specifies how the alarm matches the current time for it to trigger. Options include:
	*                       - ONCE_PER_SECOND (0x0F): Alarm triggers once per second.
	*                       - S_MATCH (0x0E): Alarm triggers when seconds match.
	*                       - MS_MATCH (0x0C): Alarm triggers when minutes and seconds match.
	*                       - HMS_MATCH (0x08): Alarm triggers when hours, minutes, and seconds match.
	*                       - DHMS_MATCH (0x00): Alarm triggers when day (or date), hours, minutes, and seconds match.
	* @param alarmDate The date of the month to trigger the alarm (1-31). Ignored if dayOrDate is set to DAY_OF_WEEK.
	* @param alarmDay The day of the week to trigger the alarm (MONDAY-SUNDAY). Ignored if dayOrDate is set to DAY_OF_MONTH.
	* @param alarmHours The hour to trigger the alarm (0-23).
	* @param alarmMinutes The minute to trigger the alarm (0-59).
	* @param alarmSeconds The second to trigger the alarm (0-59). Ignored for ALARM2.
	*/
	virtual void setAlarm(ALARM_NO alarmNo, ALARM_TYPE dayOrDate, unsigned int alarmMatchMode, unsigned int alarmDate, unsigned int alarmDay, unsigned int alarmHours, unsigned int alarmMinutes, unsigned int alarmSeconds);
	
	//FUTURE WORK: setAlarmInterruptGPIOPin
	/** readAlarm
	* Reads the current status of the specified alarm.
	* 
	* @brief Retrieves the status (triggered or not) of an alarm.
	* @param readAlarmNo The alarm number to check (ALARM1 or ALARM2).
	* @return An unsigned integer indicating the alarm status: 1 if the alarm has been triggered, 0 otherwise.
	* 
	* @note This function allows for monitoring of alarm triggers, useful for applications requiring 
	*       actions based on alarm conditions.
	* @todo Implement functionality to link alarm triggers to specific GPIO pins for external notifications.
	*/
	virtual unsigned int readAlarm(ALARM_NO readAlarmNo);
	
	/** clearAlarmFlag
	* Clears the alarm flag for the specified alarm.
	* 
	* @brief Resets the flag indicating whether a specific alarm has been triggered.
	* @param clearFlagAlarmNo The alarm number whose flag is to be cleared (ALARM1 or ALARM2).
	* 
	* @note This function is essential for resetting the alarm status, allowing it to be triggered again in future cycles.
	*       It should be called after handling an alarm trigger to ensure the system is ready for subsequent alarms.
	*/
	virtual void clearAlarmFlag(ALARM_NO clearFlagAlarmNo);
	
	//Novel future used to advance the alarm set off time by 10 mins
	/**alarmSnooze
	* Snoozes the specified alarm for 10 minutes.
	* 
	* @brief Temporarily postpones the alarm trigger time by 10 minutes.
	* @param snoozeAlarmNo The alarm number to snooze (ALARM1 or ALARM2).
	* 
	* @note This feature provides a convenient way to delay alarm notifications, 
	*       offering users additional flexibility in managing alarm triggers. 
	*       It automatically recalculates and updates the alarm time, extending it by 10 minutes from the current setting.
	*/
	virtual void alarmSnooze(ALARM_NO snoozeAlarmNo);
	
	/////////////////////////// Off the shelf functions ////////////////////////////
	/** bcdToDec
	* Converts a BCD (Binary-Coded Decimal) value to its decimal equivalent.
	* 
	* @brief Transforms BCD value to decimal format.
	* @param bcdValue The BCD value to convert.
	* @return The decimal representation of the BCD value.
	* 
	* @note This function is essential for interpreting BCD-encoded data from hardware registers, 
	*       particularly in applications dealing with digital clocks or calendars.
	*/
	static unsigned int bcdToDec(unsigned char bcdValue) {
    unsigned int tens = (bcdValue >> 4) * 10;
    unsigned int ones = bcdValue & 0x0F;
    return tens + ones;
	}
	
	/** dumpRegisters
	* Dumps the current state of device registers for debugging purposes.
	* 
	* @brief Outputs the register states to the console.
	* @param number The number of registers to dump. Defaults to all if not specified.
	* 
	* @note This function is useful for debugging and development, allowing developers to inspect
	*       the internal states of device registers at runtime.
	*/
	void dumpRegisters(unsigned int number = 0xff) {
        debugDumpRegisters(number); // Calls the public function from the base class
    }
	
	/** decimalToBCD
	* Converts a decimal value to its BCD (Binary-Coded Decimal) equivalent.
	* 
	* @brief Transforms decimal value to BCD format.
	* @param decimal The decimal value to convert.
	* @return The BCD representation of the decimal value.
	* 
	* @note This function facilitates writing data to hardware registers that expect BCD encoding, 
	*       such as those in digital clocks or calendars.
	*/
	static unsigned char decimalToBCD(int decimal);


	virtual ~i2c_device_ds3231();
};

} /* namespace i2c */

#endif /* I2C_DEVICE_DS3231_H_ */