/*
 * application.cpp  Created on: 29 Apr 2014
 * Copyright (c) 2014 Derek Molloy (www.derekmolloy.ie)
 * Made available for the book "Exploring Raspberry Pi"
 * See: www.exploringrpi.com
 * Licensed under the EUPL V.1.1
 *
 * This Software is provided to You under the terms of the European
 * Union Public License (the "EUPL") version 1.1 as published by the
 * European Union. Any use of this Software, other than as authorized
 * under this License is strictly prohibited (to the extent such use
 * is covered by a right of the copyright holder of this Software).
 *
 * This Software is provided under the License on an "AS IS" basis and
 * without warranties of any kind concerning the Software, including
 * without limitation merchantability, fitness for a particular purpose,
 * absence of defects or errors, accuracy, and non-infringement of
 * intellectual property rights other than copyright. This disclaimer
 * of warranty is an essential part of the License and a condition for
 * the grant of any rights to this Software.
 *
 * For more details, see http://www.derekmolloy.ie/
 */

#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include "i2c_device_ds3231.h"

using namespace std;
using namespace i2c;

int main() {
   i2c_device_ds3231 rtc(1,0x68);
   
/*    rtc.displayTimeAndDate();
   sleep(1);
   rtc.displayTimeAndDate();
   sleep(3);
   rtc.changeHrMode(i2c_device_ds3231::TWELVE);
   rtc.displayTimeAndDate();
   rtc.displayTemperature();

   sleep(1);

   rtc.setTimeAndDate(14,30,55,26,10,2024);
   sleep(1);
   rtc.displayTimeAndDate();
   rtc.dumpRegisters();
   
   //leap year test
   rtc.setTimeAndDate(14,30,55,29,2,2024);
   sleep(1);
   rtc.displayTimeAndDate();
   
   rtc.setTimeAndDate(14,30,55,29,2,2025);
   sleep(1);
   rtc.displayTimeAndDate();
   
   //31/30 test
   rtc.setTimeAndDate(14,30,55,30,4,2025);
   sleep(1);
   rtc.displayTimeAndDate(); */
   
   rtc.setTimeAndDate(14,30,55,31,4,2025);
   sleep(1);
   rtc.displayTimeAndDate();
   
   //Testing 1Hz frequency square wave on LED  for 5 seconds
   sleep(1);
   rtc.startSquareWave(i2c_device_ds3231::WAVE_1);
   sleep(5);
   rtc.stopSquareWave();
   
   rtc.setAlarm1(i2c_device_ds3231::DAY_OF_MONTH, MS_MATCH, 1, 1, 21, 55, 0); 
   
   
   return 0;
}