/**
 * @file rtc_app.cpp
 * @brief Application Demonstrating Usage of the DS3231 RTC and GPIO for Alarms
 *
 * This application demonstrates the use of the DS3231 Real Time Clock (RTC) module
 * with a Raspberry Pi through I2C interface. It showcases setting the time, date, adjusting
 * for leap years, setting alarms, and using a GPIO pin to indicate alarm triggers. The app
 * employs both the i2c_device_ds3231 class for managing the RTC and the LED class for
 * controlling an LED via GPIO as a visual indicator for alarms.
 *
 * @author Sarraj Alsersawi
 * @see https://github.com/serajsersawi/rpi_git.git
 * Inspired by Derek Molloy's Exploring Raspberry Pi
 * @see https://github.com/derekmolloy/exploringrpi.git
 *
 * The application performs the following operations:
 * - Displays the current time and date
 * - Sets a specific time and date and then displays the update
 * - Changes the hour mode between 24-hour and 12-hour formats
 * - Displays the current temperature as read from the DS3231
 * - Performs a register dump to check the correctness of the operations above
 * - Conducts a leap year test by setting dates for leap and non-leap years
 * - Validates date handling by setting valid and invalid dates
 * - Tests the square wave output functionality by toggling an LED connected to the SQW pin
 * - Sets alarms and demonstrates the alarm snooze functionality
 * - Tests the 32kHz output functionality
 * - Implements a polling mechanism for alarm flags to trigger a GPIO pin change
 *
 * Note: This app is designed for demonstration purposes and showcases basic interactions
 * with the DS3231 RTC module and general GPIO usage on a Raspberry Pi.
 */
 
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include "i2c_device_ds3231.h"
#include "gpio.h"


using namespace std;
using namespace i2c;

int main() {
	i2c_device_ds3231 rtc(1,0x68);
	LED alarmLED(23);
	
	rtc.displayTimeAndDate();
	sleep(1);
	
	rtc.setTimeAndDate(14,30,55,26,10,2024);
	rtc.displayTimeAndDate();
	sleep(1);
	
	
	rtc.displayTemperature();
	sleep(1);
	
	//General dump to check correctness of code above
	rtc.dumpRegisters();
   
    //leap year test
    rtc.setTimeAndDate(18,33,12,29,2,2024); //Valid date
    rtc.displayTimeAndDate();
	sleep(2);
    
    rtc.setTimeAndDate(19,44,22,29,2,2025); //Invalid date
    rtc.displayTimeAndDate();
	sleep(2);
    
    //31/30 date test

	rtc.setTimeAndDate(2,35,51,31,4,2025); //Invalid date
    rtc.displayTimeAndDate();
	sleep(2);
	
    rtc.setTimeAndDate(14,30,55,30,4,2025); //Valid date
    rtc.displayTimeAndDate(); 
	sleep(2);
	
    //General dump to check correctness of code above
    rtc.dumpRegisters();
    
    //Testing 1Hz frequency square wave on LED  for 5 seconds
	//LED 2 is connected to the SQW pin of the RTC module
	//LED 2 will be toggled each 500ms for 10 seconds
	
    sleep(1);
    rtc.startSquareWave(i2c_device_ds3231::WAVE_1);
    sleep(10);
    rtc.stopSquareWave();
    
	//General dump to check correctness of code above
    rtc.dumpRegisters(); 
    
	//setting alarms test
	//Setting alarm 1
    rtc.setAlarm(i2c_device_ds3231::ALARM1 ,i2c_device_ds3231::DAY_OF_MONTH, MS_MATCH, 1, 1, 15, 38, 0); 
	sleep(2);
    rtc.dumpRegisters();
	sleep(2);
    
	//Setting alarm 2
    rtc.setAlarm(i2c_device_ds3231::ALARM2, i2c_device_ds3231::DAY_OF_MONTH, MS_MATCH, 1, 1, 17, 2, 0); 
	sleep(2);
    rtc.dumpRegisters();
	sleep(2);
	
    //Snooze alarm then dump to check validity
    rtc.alarmSnooze(i2c_device_ds3231::ALARM2);
    rtc.dumpRegisters();
	
	//Test 32kHz functionality
	rtc.start32kHz();
	
    //Polling for alarm flag test 
	//FUTURE WORK: Receive interrupt on a GPIO then act upon it
	
    rtc.setAlarm(i2c_device_ds3231::ALARM1 ,i2c_device_ds3231::DAY_OF_MONTH, ONCE_PER_SECOND, 1, 1, 15, 38, 0); 
    
    while(1){
	     if(rtc.readAlarm(i2c_device_ds3231::ALARM1)){
	  	   cout << "Interrupt" <<  endl;
	  	   // rtc.dumpRegisters();
	  	   alarmLED.toggleLED();
	  	   rtc.clearAlarmFlag(i2c_device_ds3231::ALARM1);
	  	   // rtc.dumpRegisters();
	  	   cout << "use CTRL + C to stop polling" <<  endl;
	  	     
	     }
    }
    
   return 0;
}