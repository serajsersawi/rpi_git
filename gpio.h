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