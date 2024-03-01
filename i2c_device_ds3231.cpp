

#include "i2c_device_ds3231.h"
#include <iostream>
#include <unistd.h>
#include <math.h>
#include <stdio.h>


using namespace std;

namespace i2c {
 

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
	i2c_device(I2CBus, I2CAddress){   // this member initialisation list calls the parent constructor
	
	this->I2CAddress = I2CAddress;
	this->I2CBus = I2CBus;
	
	this->seconds = SECONDS_DEFAULT;
	this->minutes = MINUTES_DEFAULT;
	this->hours = HOURS_DEFAULT;
	this->day = DAY_OF_WEEK_DEFAULT;
	this->date = DATE_DEFAULT;
	this->month = MONTH_DEFAULT;
	this->year = YEAR_DEFAULT;
	
	this->hr_mode = i2c_device_ds3231::TWENTYFOUR;
	this->wave = i2c_device_ds3231::WAVE_2;
	this->clk = i2c_device_ds3231::CLOCK_RUN;
		
	this->initUpdateAllRegisters();
}

//Update all registers (time and date)
int i2c_device_ds3231::initUpdateAllRegisters(){

   int check = 0;
   
   check += (this->writeRegister(SECONDS_REG, SECONDS_DEFAULT)); //Seconds: 00
   check += (this->writeRegister(MINUTES_REG, MINUTES_DEFAULT)); //Minutes: 00
   check += (this->writeRegister(HOURS_REG, HOURS_DEFAULT)); //Hours: 00 | 24hr format
   check += (this->writeRegister(DAY_REG, DAY_OF_WEEK_DEFAULT)); //User defined 01 Monday
   check += (this->writeRegister(MONTH_CENT_REG, MONTH_DEFAULT)); //01
   check += (this->writeRegister(DATE_REG, DATE_DEFAULT)); //01
   check += (this->writeRegister(YEAR_REG, YEAR_DEFAULT)); //2000
   
   return check;
}



/*********************************************************************************************/
void i2c_device_ds3231::setAlarm1(ALARM_TYPE A1_dom_dow, unsigned int A1_match_mode, unsigned int A1_date, unsigned int A1_day, unsigned int A1_hours, unsigned int A1_minutes, unsigned int A1_seconds){
	
	//unsigned int RegisterVal = this->readRegister();
	//setting mask bits
	this->writeRegister(ALARM1_DAY_DATE_REG, ((A1_match_mode >> 3) & 0x01) << 7 ); //A1M4

	this->writeRegister(ALARM1_HR_REG	   , ((A1_match_mode >> 2) & 0x01) << 7 ); //A1M3
	if(A1_dom_dow)
		this->writeRegister(ALARM1_MIN_REG	   , (((A1_match_mode >> 1) & 0x01) << 7) | 0x40 ); //A1M2 DOW
	else
		this->writeRegister(ALARM1_MIN_REG	   , (((A1_match_mode >> 1) & 0x01) << 7) & (~(0x40)) ); //A1M2 DOM	
	this->writeRegister(ALARM1_SEC_REG     , ((A1_match_mode >> 0) & 0x01) << 7 ); //A1M1
	
	setAlarm1Time(A1_hours, A1_minutes, A1_seconds);
	setAlarm1DayDate(A1_date, A1_day, A1_dom_dow);
	
	enableAlarm1();
	
	
}
void i2c_device_ds3231::startSquareWave(SQR_WAVES wave){
	
	unsigned int RegisterVal = this->readRegister(CTRL_REG);
	//set the BBSQW bit
	RegisterVal |= (1 << 6);
	//reset the INTCN bit
	RegisterVal &= ~(1 << 2);
	//set RS1 and RS2 bits
	RegisterVal |= (wave << 3);
	this->writeRegister(CTRL_REG, (RegisterVal));
	
	switch(wave){
		
		case WAVE_1:
		cout << "Square wave output frequency: 1 Hz " << "\n" << endl;
		break;
		
		case WAVE_2:
		cout << "Square wave output frequency: 1.024 kHz " << "\n" << endl;
		break;
		
		case WAVE_3:
		cout << "Square wave output frequency: 4.096 kHz " << "\n" << endl;
		break;
		
		case WAVE_4:
		cout << "Square wave output frequency: 8.192 kHz " << "\n" << endl;
		break;
		
		default:
		cerr << "Invalid wave input! setting wave frequency to 1Hz" << endl;
		break;
		
	}
}

void i2c_device_ds3231::stopSquareWave(){
	
	unsigned int RegisterVal = this->readRegister(CTRL_REG);
	//reset the BBSQW bit
	RegisterVal &= ~(1 << 6);
	//set the INTCN bit
	RegisterVal |= (1 << 2);
	
	this->writeRegister(CTRL_REG, (RegisterVal));
}

void i2c_device_ds3231::enableAlarm1(){
	stopSquareWave();
	unsigned int RegisterVal = this->readRegister(CTRL_REG);
	//enable interrupt
	RegisterVal |= (1 << 0);
	this->writeRegister(CTRL_REG, (RegisterVal));
}

 
void i2c_device_ds3231::displayTimeAndDate(){

	//reading is done first (reading the register data into the class parameters)
	//reading the values that do not change quickly first
	this->year = 	getYear();
	this->month = 	getMonth();
	this->date = 	getDate();
	this->day = 	getDay();
	this->hours   = getHours();
	this->minutes = getMinutes();
	this->seconds = getSeconds();
	
	char dateTimeStr[30];
	switch(hr_mode){
		case(TWELVE):
		if(am_pm)
			sprintf(dateTimeStr, "%02d:%02d:%02d PM   %02d/%02d/%d", hours, minutes, seconds, date, month, year);
		else
			sprintf(dateTimeStr, "%02d:%02d:%02d AM   %02d/%02d/%d", hours, minutes, seconds, date, month, year);
		break;
		case(TWENTYFOUR):
			sprintf(dateTimeStr, "%02d:%02d:%02d   %02d/%02d/%d", hours, minutes, seconds, date, month, year);
	}
	
	cout << dateTimeStr << "\n" <<endl;
	
}

int i2c_device_ds3231::displayTemperature(){
	
	//Convert temperature
	unsigned int oldRegisterVal = this->readRegister(CTRL_REG);
	this->writeRegister(CTRL_REG, (oldRegisterVal | (0x20)));

	//check for BSY status, if cleared then read then display
	while((this->readRegister(CTRL_STAT_REG) & 0x04) >> 2){
		//wait untill the BSY bit is cleared
		//this means conversion is complete and device no longer busy
	}
		
		// Read the temperature registers
		unsigned char temp_msb = this->readRegister(TEMP_MSB_REG); // Register 11h
		unsigned char temp_lsb = this->readRegister(TEMP_LSB_REG); // Register 12h

		// Combine the MSB and the top two bits of the LSB to get the 10-bit raw temperature value
		int raw_temperature = ((temp_msb << 2) | (temp_lsb >> 6));

		// Check if the temperature is negative
		bool negative = temp_msb & 0x80; // Check sign bit

		// If negative, we must convert the 10-bit value from 2's complement to a negative decimal
		if (negative) {
			// Invert and add 1 to get the two's complement
			raw_temperature = ~raw_temperature & 0x3FF; // Mask to 10 bits
			raw_temperature = (raw_temperature + 1) * -1;
		}

		// Convert the raw temperature to Celsius
		this->temperature = raw_temperature * 0.25;
		
		cout << "The temperature is " << this->temperature << "°C" << "\n" << endl;
		return 0;
}

void i2c_device_ds3231::setDate(unsigned int date, unsigned int month, int year){
	unsigned int invalidData = 0;
	invalidData = setYear(year)
	+ setMonth(month)
	+ setDate(date, RTC_REGS);
	
	if(invalidData){
		//set date to default
		cerr << "Setting date back to 01/01/2000" << endl;
		setYear(2000);
		setMonth(1);
		setDate(1, RTC_REGS);
	}
}

void i2c_device_ds3231::setAlarm1DayDate(unsigned int A1_date, unsigned int A1_day, ALARM_TYPE A1_dom_dow){
	unsigned int invalidData = 0;
	
	if(A1_dom_dow)//day of week
		invalidData = setDay(A1_day, ALARM1_REGS);
	else
		invalidData = setDate(A1_date, ALARM1_REGS);
	
	
	if(invalidData){
		//set date to default
		cerr << "Setting date/day back to 1" << endl;
		setDate(1, ALARM1_REGS);
	}
}

void i2c_device_ds3231::setTime(unsigned int hours, unsigned int minutes, unsigned int seconds){
	
	cout << "setTime function user input hours "<< hours <<endl;
	cout << "setTime function user input minutes "<< minutes <<endl;
	cout << "setTime function user input seconds "<< seconds <<endl;
	unsigned int invalidData = 0;
	invalidData = setSeconds(seconds, RTC_REGS)
	+ setMinutes(minutes, RTC_REGS)
	+ setHours(hours, RTC_REGS);
	
	if(invalidData){
		//set date to default
		cerr << "Setting time back to 00:00:00" << endl;
		setSeconds(0, RTC_REGS);
		setMinutes(0, RTC_REGS);
		setHours(0, RTC_REGS);
	}
	
}

void i2c_device_ds3231::setAlarm1Time(unsigned int A1_hours, unsigned int A1_minutes, unsigned int A1_seconds){
	
	unsigned int invalidData = 0;
	invalidData = setSeconds(A1_seconds, ALARM1_REGS)
	+ setMinutes(A1_minutes, ALARM1_REGS)
	+ setHours(A1_hours, ALARM1_REGS);
	
	if(invalidData){
		//set date to default
		cerr << "Setting alarm1 back to 00:00:00" << endl;
		setSeconds(0, ALARM1_REGS);
		setMinutes(0, ALARM1_REGS);
		setHours(0, ALARM1_REGS);
	}
}

void i2c_device_ds3231::setTimeAndDate(unsigned int hours, unsigned int minutes, unsigned int seconds, unsigned int date, unsigned int month, int year){
	setDate(date, month, year);
	setTime(hours, minutes, seconds);
	
}
 
unsigned int i2c_device_ds3231::getSeconds(){return bcdToDec(this->readRegister(SECONDS_REG));}

unsigned int i2c_device_ds3231::setSeconds(unsigned int seconds, unsigned int reg){
	
	unsigned int targetRegister = SECONDS_REG;
	switch (reg){
		case RTC_REGS:
			targetRegister = SECONDS_REG;
			break;
		case ALARM1_REGS:
			targetRegister = ALARM1_SEC_REG;
			break;
		case ALARM2_REGS:
			cerr << "Invalid register input!" << endl;
			//targetRegister = SECONDS_REG;
			return 1;
			break;	
		default:
			cerr << "Invalid register input!" << endl;
			//targetRegister = SECONDS_REG;
			return 1;
	}
	if(seconds < 60){
		this->writeRegister(targetRegister, decimalToBCD(seconds));
		//update the object 
		this->seconds = getSeconds();
		return 0;
	}
	else{
		cerr << "Seconds out of range (00-59)" << endl;
		return 1;
	}
}

unsigned int i2c_device_ds3231::getMinutes(){return bcdToDec(this->readRegister(MINUTES_REG));}

unsigned int i2c_device_ds3231::setMinutes(unsigned int minutes, unsigned int reg){
	cout << "inside setTime function user input minutes "<< minutes <<endl;
	unsigned int targetRegister = MINUTES_REG;
	switch (reg){
		case RTC_REGS:
			targetRegister = MINUTES_REG;
			cout << "setting rtc minutes and target is " << targetRegister <<endl;
			break;
		case ALARM1_REGS:
			targetRegister = ALARM1_MIN_REG;
			break;
		case ALARM2_REGS:
			targetRegister = ALARM2_MIN_REG;
			break;	
		default:
			cerr << "Invalid register input!" << endl;
			//targetRegister = MINUTES_REG;
			return 1;
	}
	if(minutes < 60){
		cout << "MINUTES_REG is "  << MINUTES_REG <<endl;
		this->writeRegister(MINUTES_REG, decimalToBCD(minutes));
		return 0;
	}
	else{
		cerr << "Minutes out of range (00-59)" << endl;
		return 1;
	}
}

unsigned int i2c_device_ds3231::getHours(){
	
	unsigned int hourTens;
	unsigned int hourOnes;
	
	switch((this->readRegister(HOURS_REG) & 0x40) >> 6){
		case 0:
			this->hr_mode = i2c_device_ds3231::TWENTYFOUR;
			hourOnes = this->readRegister(HOURS_REG) & 0x0F;
			hourTens = ((this->readRegister(HOURS_REG) & 0x30) >> 4) * 10;
			break;
		case 1:
			this->hr_mode = i2c_device_ds3231::TWELVE;
			am_pm = static_cast<AFTER_BEFORE_NOON>((this->readRegister(HOURS_REG) & 0x20) >> 5);
			hourOnes = this->readRegister(HOURS_REG) & 0x0F;
			hourTens = ((this->readRegister(HOURS_REG) & 0x10) >> 4) * 10;
			break;
	}

	return hourOnes + hourTens;
}

//this function will accept hours 0 - 12 and not change anything 
//if the hour is greater than 12 till 24 wil accept it and make sure system is 24
unsigned int i2c_device_ds3231::setHours(unsigned int hours, unsigned int reg){
	
	unsigned int targetRegister = HOURS_REG;
	switch (reg){
		case RTC_REGS:
			targetRegister = DAY_REG;
			break;
		case ALARM1_REGS:
			targetRegister = ALARM1_HR_REG;
			break;
		case ALARM2_REGS:
			targetRegister = ALARM2_HR_REG;
			break;	
		default:
			cerr << "Invalid register input!" << endl;
			//targetRegister = HOURS_REG;
			return 1;
	}
	unsigned int hourTens;
	unsigned int hourOnes;
	unsigned int oldRegisterVal = this->readRegister(targetRegister);
	
	if(hours > -1 && hours < 13){
		//will write to the register and leave everything else unchanged
		hourTens = hours / 10;
		hourOnes = hours % 10;
		this->writeRegister(targetRegister, ((oldRegisterVal & 0xE0) | (((hourTens << 4) | hourOnes) & 0x1F)));
		this->hours   = getHours();
		return 0;
	} 
	else if(hours > 12 && hours < 24){
		
		hourTens = hours / 10;
		hourOnes = hours % 10;
		this->changeHrMode(TWENTYFOUR);
		this->writeRegister(targetRegister, ((oldRegisterVal & 0xC0) | (((hourTens << 4) | hourOnes) & 0x3F)));
		this->hours   = getHours();
		return 0;
	}
	else{
		cerr << "Hours out of range (00-23) (1-12)" << endl;
		return 1;
	}
}




unsigned int i2c_device_ds3231::getDay(){return bcdToDec(this->readRegister(DAY_REG));}

unsigned int i2c_device_ds3231::setDay(unsigned int day, unsigned int reg){
	
	unsigned int targetRegister = DAY_REG;
	switch (reg){
		case RTC_REGS:
			targetRegister = DAY_REG;
			break;
		case ALARM1_REGS:
			targetRegister = ALARM1_DAY_DATE_REG;
			break;
		case ALARM2_REGS:
			targetRegister = ALARM2_DAY_DATE_REG;
			break;	
		default:
			cerr << "Invalid register input!" << endl;
			//targetRegister = DATE_REG;
			return 1;
	}
	
	if(day > 0 && minutes < 8){
		unsigned int oldRegisterVal = this->readRegister(targetRegister);
		this->writeRegister(targetRegister, ((oldRegisterVal) & (~(0x0F))) | ((decimalToBCD(day)) & 0x0F));
		this->day = 	getDay();
		return 0;
	}
	else{
		cerr << "Days out of range (1-7)" << endl;
		return 1;
	}
}

unsigned int i2c_device_ds3231::getDate(){
	
	unsigned int dateOnes;
	unsigned int dateTens;
	
	dateOnes = (this->readRegister(DATE_REG) & 0x0F);
	dateTens = ((this->readRegister(DATE_REG) & 0x30) >> 4) * 10;
			
	return dateOnes + dateTens;
}

unsigned int i2c_device_ds3231::setDate(unsigned int date, unsigned int reg){
	//according to which register is being manipulated 
	unsigned int targetRegister = DATE_REG;
	switch (reg){
		case RTC_REGS:
			targetRegister = DATE_REG;
			break;
		case ALARM1_REGS:
			targetRegister = ALARM1_DAY_DATE_REG;
			break;
		case ALARM2_REGS:
			targetRegister = ALARM2_DAY_DATE_REG;
			break;	
		default:
			cerr << "Invalid register input!" << endl;
			//targetRegister = DATE_REG;
			return 1;
	}
	
	bool isValidDate = false;
	switch(getMonth()) {
		case 4: case 6: case 9: case 11: // April, June, September, November have 30 days
            if(date > 0 && date < 31){
				isValidDate = true;
			}
			break;
        case 2: // February needs special handling for leap years
            if (this->isLeapYear) {
                isValidDate = (date > 0 && date <= 29);
			}
			else{
				isValidDate = (date > 0 && date <= 28);
			}
			break;
        case 1: case 3: case 5: case 7: case 8: case 10: case 12: // All other months have 31 days
			isValidDate = (date > 0 && date <= 31);
			break;
		default:
			isValidDate = false;
			break;
    }
	
	if(isValidDate){
		unsigned int oldRegisterVal = this->readRegister(targetRegister);
		this->writeRegister(targetRegister, ((oldRegisterVal) & (~(0x3F))) | ((decimalToBCD(date)) & 0x3F));
		this->date = 	getDate();
		return 0;
	}
	
	else{
		cerr << "Date out of range or invalid" << endl;
		return 1;
		
	}
	

}

unsigned int i2c_device_ds3231::getMonth(){
	
	unsigned int monthOnes;
	unsigned int monthTens;
	
	monthOnes = (this->readRegister(MONTH_CENT_REG) & 0x0F);
	monthTens = ((this->readRegister(MONTH_CENT_REG) & 0x10) >> 4) * 10;
			
	return monthOnes + monthTens;
}

unsigned int i2c_device_ds3231::setMonth(unsigned int month){
	
	unsigned int oldRegisterVal = this->readRegister(MONTH_CENT_REG);
	if(month > 0 && month < 13){
		this->writeRegister(MONTH_CENT_REG, ((oldRegisterVal & 0xE0) | (decimalToBCD(month) & 0x1F)));
		this->month = getMonth();
		return 0;
	}
	else{
		cerr << "Month out of range (1-12)" << endl;
		return 1;
	}
}

int i2c_device_ds3231::getYear(){
	
	unsigned int yearOnes;
	unsigned int yearTens;
	
	yearOnes = (this->readRegister(YEAR_REG) & 0x0F);
	yearTens = ((this->readRegister(YEAR_REG) & 0xF0) >> 4) * 10;
	
	if ((this->readRegister(MONTH_CENT_REG) & 0x80) >> 7){
		return (2100 + yearOnes + yearTens);
	}
	else {
		return (2000 + yearOnes + yearTens);
	}
	
}

int i2c_device_ds3231::setYear(int year){
	
	if(year >= 2000 && year < 2100){
		
		if((year % 4 == 0) && (year % 100 != 0 || year % 400 == 0)){
			this->isLeapYear = true;
		}
		else{
			this->isLeapYear = false;
		}
		int yearTensAndOnes = year % 100;
		
		
		this->writeRegister(YEAR_REG, (decimalToBCD(yearTensAndOnes)));
		this->year = 	getYear();
		return 0;
	}
	
	else{
		cerr << "Year out of range (2000 - 2099)" << endl;
		return 1;
		
	}
}

void i2c_device_ds3231::changeHrMode(unsigned int mode){
	unsigned int oldRegisterVal = this->readRegister(HOURS_REG);
	
	switch(mode){
		case i2c_device_ds3231::TWENTYFOUR:
		this->writeRegister(HOURS_REG, (oldRegisterVal & !(0x40)));
		break;
		case i2c_device_ds3231::TWELVE:
		this->writeRegister(HOURS_REG, (oldRegisterVal | (0x40)));
		break;
	}
}
 unsigned char i2c_device_ds3231::decimalToBCD(int decimal){
	 
    if (decimal < 0 || decimal > 99) {
        cerr << "Decimal number out of range for a single BCD byte (0-99)" << std::endl;
        return 0;
    }
	
	int tens = decimal / 10;
    int ones = decimal % 10;
	return static_cast<unsigned char>((tens << 4) | ones);
}


i2c_device_ds3231::~i2c_device_ds3231() {}

} /* namespace exploringRPi */