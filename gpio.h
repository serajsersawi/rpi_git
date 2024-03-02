/*
	this code been slightly modifified as part of assignment 1 work
	by Sarraj Alsersawi: serajsersawi98@gmail.com
	25/02/2024
	Created by Derek Molloy's Exploring Raspberry Pi
	@see https://github.com/derekmolloy/exploringrpi.git
 
*/

#ifndef GPIO_H_
#define GPIO_H_

#include <string>

class LED {
private:
    std::string gpioPath; // Use std::string to avoid namespace pollution
    int gpioNumber;
    void writeSysfs(std::string path, std::string filename, std::string value);
public:
    LED(int gpioNumber); // Constructor
    virtual void turnLEDOn();
    virtual void turnLEDOff();
    virtual void toggleLED();
    virtual void displayState();
    virtual unsigned int getState(); // Returns the state as an unsigned int
    virtual ~LED(); // Destructor
};

#endif /* GPIO_H_ */