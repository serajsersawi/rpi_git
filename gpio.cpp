/** Simple Sysfs LED control program - written by Derek Molloy
*    simple OOP  struture for the Exploring Raspberry Pi
*
*    This program can be used to easliy control multiple LEDS using a class.
*    This program uses C++11 for the to_string() function and therefore must
*    be compiled with the -std=c++11 flag.
*/

/*
	this code been slightly modifified as part of assignment 1 work
	by Sarraj Alsersawi: serajsersawi98@gmail.com
	25/02/2024
	Created by Derek Molloy's Exploring Raspberry Pi
	@see https://github.com/derekmolloy/exploringrpi.git
 
 */

#include<iostream>
#include<fstream>
#include<string>
#include<unistd.h>         // for the microsecond sleep function
#include"gpio.h"
using namespace std;

#define GPIO         "/sys/class/gpio/"
#define FLASH_DELAY  50000 // 50 milliseconds

LED::LED(int gpioNumber){  // constructor implementation
   this->gpioNumber = gpioNumber;
   gpioPath = string(GPIO "gpio") + to_string(gpioNumber) + string("/");
   writeSysfs(string(GPIO), "export", to_string(gpioNumber));
   usleep(100000);         // ensure GPIO is exported
   writeSysfs(gpioPath, "direction", "out");
}

// This implementation function is "hidden" outside of the class
void LED::writeSysfs(string path, string filename, string value){
   ofstream fs;
   fs.open((path+filename).c_str());
   fs << value;
   fs.close();
}

void LED::turnLEDOn(){
   writeSysfs(gpioPath, "value", "1");
}

void LED::turnLEDOff(){
   writeSysfs(gpioPath, "value", "0");
}

void LED::toggleLED(){

	//writeSysfs(gpioPath, "value", to_string((!getState()) + 48));
	
	switch(getState()){
		
		case 1: turnLEDOff(); cout << "LED OFF on GPIO " << this->gpioNumber << endl; break;
		case 0: turnLEDOn(); cout << "LED ON on GPIO " << this->gpioNumber << endl; break;
		default: break;
		
	}
}


void LED::displayState(){
   ifstream fs;
   fs.open((gpioPath + "value").c_str());
   string line;
   cout << "The current LED state is ";
   while(getline(fs,line)) cout << line << endl;
   fs.close();
}

unsigned int LED::getState(){
   ifstream fs;
   fs.open((gpioPath + "value").c_str());
   string line;
   getline(fs,line); // Assuming only one line that says either 0 or 1
   fs.close();
   return stoi(line); // Converts the string to an integer (0 or 1) and returns it
}

LED::~LED(){  // The destructor unexports the sysfs GPIO entries
   cout << "Destroying the LED with GPIO number " << gpioNumber << endl;
   writeSysfs(string(GPIO), "unexport", to_string(gpioNumber));
}