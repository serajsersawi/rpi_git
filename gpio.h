

#ifndef GPIO_H_
#define GPIO_H_


class LED{
   private:                // the following is part of the implementation
      string gpioPath;     // private states
      int    gpioNumber;
      void writeSysfs(string path, string filename, string value);
   public:                 // part of the public interface
      LED(int gpioNumber); // the constructor -- create the object
      virtual void turnLEDOn();
      virtual void turnLEDOff();
	  virtual void toggleLED();
      virtual void displayState();
	  virtual unsigned int getState();
      virtual ~LED();      // the destructor -- called automatically
};

#endif /* I2C_DEVICE_DS3231_H_ */