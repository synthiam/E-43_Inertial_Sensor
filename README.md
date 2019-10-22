# E-43 Inertial Sensor

**Project Status:** Ready to manufacture

If you are new to GitHub and would like to create a derivative of this project, please have a look at GitHub instructions to [fork a repo](https://help.github.com/en/articles/fork-a-repo)

## Description

Out of our passion to solve the issue of time waste in the robot creation process, EZ-Builder was born. The EZ-Builder software platform reduces the time it takes to get from idea to implementation. Since 2011 we have been listening to the challenges robot builders encounter and continue to provide them with an ever expanding toolbox of solutions. Join us on this journey!

We didn't stop at the software, we also made time saving hardware solutions. This is an accelerometer/gyro/temperature sensor paired with an STM32 ARM cortex-M0 microcontroller. The microcontroller simplifies the sensor data and makes it compatible with EZ-builder. A unique feature of this design is that it connects directly to a visual interface and automatically outputs pitch and roll angles in EZ-Builder. We want to share these files with you so you can create your own version for your community and customers to enjoy!

**Features:** 
- I2C communication
- Mini programming header
- 5V tolerant I/O pins
- Blue LED status indicator
- EZ-Builder behavior control available
- Voltage requirement: 3.2-3.4VDC (3.3V Typical)
- Current draw: 10.8mA
- Dimensions: 15.8(W) x 38.7(L) x 7.5(H) (mm)

**Major components:** 
- STM32F030C8T6 32-bit Cortex-M0 ARM microcontroller (custom firmware provided)
- LSM6DS3USTR Accelerometer/Gyro Combo 

**Manufacturing notes:** 
1. Supplier: programs custom firmware into the STM32F030C8T6 at their facility before sending to manufacturer
2. Manufacturer: Single side placement and soldering of SMT components

**Note:** This camera natively connects to the EZ-Bv4 WiFi Board: https://github.com/synthiam/E-15_EZ-Bv4_WiFi
and EZ-B IoTiny: https://github.com/synthiam/E-20_EZ-B_IoTiny

## Contents

[**Documentation:**](https://github.com/synthiam/E-43_Inertial_Sensor/tree/master/E-43%20Documentation) Schematic PDF, Datasheet PDF, BOM, Test Procedures

[**Hardware:**](https://github.com/synthiam/E-43_Inertial_Sensor/tree/master/E-43%20Hardware) Altium PCB design File, Altium SCH Design File, Gerbers

[**Firmware:**](https://github.com/synthiam/E-43_Inertial_Sensor/tree/master/E-43%20Firmware) Code, Compiled Binary

*Altium Libraries are also available <a href="https://github.com/synthiam/Synthiam_Altium_Librairies">here</a>*

## Photos

<p align="left">
<img src="https://live.staticflickr.com/65535/40785586263_a348ae7096_k.jpg" width="683" height="383">
<img src="https://live.staticflickr.com/65535/46962806074_21fb103b8a_k.jpg" width="683" height="383"></p>

## Contact

For profit use of these files requires written consent. Contact partners@synthiam.com. For everyone else, party on!

For support check out our Synthiam Community: https://synthiam.com/Community

## License

This project is released under the following licenses:

**Hardware:** Creative Commons Plus Attribution-NonCommercial 4.0 International (CC+ BY-NC 4.0)

**Firmware:** Apache 2.0 + “Commons Clause” License Condition v1.0

Please see [LICENSE.md](https://github.com/synthiam/E-43_Inertial_Sensor/blob/master/LICENSE.md) for license details.

<a href="https://synthiam.com"><img src="https://live.staticflickr.com/65535/47791527651_358dffb302_m.jpg"></a>
