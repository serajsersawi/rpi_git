

#include "i2c_device_ds3231.h"
#include <iostream>
#include <unistd.h>
#include <math.h>
#include <stdio.h>


using namespace std;

namespace i2c {

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
   check += (this->writeRegister(MONTH_CENT_REG, DS3231_REGISTER_MONTH_DEFAULT)); //01
   check += (this->writeRegister(DATE_REG, DS3231_REGISTER_DATE_DEFAULT)); //01
   check += (this->writeRegister(YEAR_REG, DS3231_REGISTER_YEAR_DEFAULT)); //2000
   
   return check;
}



/*********************************************************************************************/


 
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
	
	cout << dateTimeStr <<endl;
	

}

int i2c_device_ds3231::displayTemperature(){
	
	//check for BSY status, if cleared then start conversion and read then display
	if(!(this->readRegister(CTRL_STAT_REG) & 0x04) >> 2){
		
		//Convert temperature
		unsigned int oldRegisterVal = this->readRegister(CTRL_REG);
		this->writeRegister(CTRL_REG, (oldRegisterVal | (0x20)));
		
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
		
		cout << "The temperature is " << this->temperature << "°C" << endl;
		return 0;
	}
	else{
		
		cout << "TCXO is busy" << endl;
		return 1;
	}

}

 
unsigned int i2c_device_ds3231::getSeconds(){return bcdToDec(this->readRegister(SECONDS_REG));}

unsigned int i2c_device_ds3231::getMinutes(){return bcdToDec(this->readRegister(MINUTES_REG));}

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

unsigned int i2c_device_ds3231::getDay(){return bcdToDec(this->readRegister(DAY_REG));}

unsigned int i2c_device_ds3231::getDate(){
	
	unsigned int dateOnes;
	unsigned int dateTens;
	
	dateOnes = (this->readRegister(DATE_REG) & 0x0F);
	dateTens = ((this->readRegister(DATE_REG) & 0x30) >> 4) * 10;
			
	return dateOnes + dateTens;
}

unsigned int i2c_device_ds3231::getMonth(){
	
	unsigned int monthOnes;
	unsigned int monthTens;
	
	monthOnes = (this->readRegister(MONTH_CENT_REG) & 0x0F);
	monthTens = ((this->readRegister(MONTH_CENT_REG) & 0x10) >> 4) * 10;
			
	return monthOnes + monthTens;
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

void i2c_device_ds3231::changeHrMode(unsigned int mode){
	unsigned int oldRegisterVal = this->readRegister(HOURS_REG);
	
	switch(mode){
		case TWENTYFOUR:
		this->writeRegister(HOURS_REG, (oldRegisterVal & !(0x40)));
		break;
		case TWELVE:
		this->writeRegister(HOURS_REG, (oldRegisterVal | (0x40)));
		break;
	}
}
 


i2c_device_ds3231::~i2c_device_ds3231() {}

} /* namespace exploringRPi */