
PIC32MZEF CPU @200Mhz

8KB eeprom (256x32), from (256x8)

4 Max31855 on-board TC Sensors (K-type), upgradable to 8 with expansion board

on-board DS18B20 1-wire Digital Thermometer, for CPU fan control

PWM control of CPU fan (6Khz PWm)

120VAC Phase-cut control of COOLING FAN for true speed control
(via 8-bit DAC and FL5160 AC Phase controller)

PWM control of heater/SSR (10Hz PWM)

USB & RS232 on-board interfaces (RS232 3.3v/5v selectable)

5v & 3.3v switching power supplies

unused PIC32 I/O pins brought out to header



*** software port from Werner Johansson's exceptional work on the original T962A improvement software.  I cannot overstate how much credit goes to WJ for his oustanding work on making this software.  I would not have even attempted this project without his work, and all of the contributors, RE work, etc everyone has done in making this oven usable.

***

This port contains simplications of the Temperature sensor code, unused things have been removed, etc, etc, and also due to the more straightforward hardware of the PIC32, many of the hardware interface functions have been changed (ie SPI-2-I2C bridge code, etc all removed).

But overall, the functionality is the same, with all of oddities of the original chinese board removed, like the TC 'gain' and 'offset' params, which are no longer needed.  The MAX31855s contain their own internal Cold-Junction compensation, therefore, no CJ calculations need to be done to the TC readings from the 31855s.


There are spots for '4' MAX31855 sensors on the PCB, a minimum of 1 can actually be used, but the oven has 2 installed, so makes sense to at least install 2.   The other 2 on-board sensors are labelled as 'PCB#1' & 'PCB#2' sensors, works well to use these as 2 add-on K-type TC's for measuring temp real close or on the surface of the target PCB itself.

Also, the on-board DS18B20 sensor is now ONLY being used to drive if/when the small 'CPU FAN' comes on, and the speed (PWM), etc.


The COOLING FAN setup uses an 8-bit DAC feeding the FL5160 phase-cut controller, providing true AC voltage/speed control using mosfet switching.  This allows the speed of the AC fan to be controlled just like a real AC dimmer, controlling how much AC voltage is 'turned on' for each AC wave cycle.  The only issue with the AC fan in this setup is that they don't even spin when the AC voltage goes too low, so running the coolfan at speeds under around 40 or so on the speed setting will produce almost no fan rotation, or none.  

** This COOL FAN setup works very well for running the oven in a 'convection-like' setup, where you keep 
   the COOL FAN running continuously during baking
**


** NOTE:  The stock metal fan this oven comes with is quite heavy and slow to react, I replaced this clunky    fan with a very cheap 120VAC 80mm fan from amazon.com, which works so much better and runs better at       lower voltages.
**

The old setup using the TRIAC was using 'proportional AC voltage' control, which is very ineffective at controlling speed of an AC fan.



******** <><><><> RECOMMENDED OVEN CHANGES <><><><><> ******************


1) Fix the earth grounding issues with chassis paint covering where the earth ground is screwed to the    chassis.  


2) Replace the bulky/metal 80mm cooling fan with a lighter/plastic fan (that has better response over AC    voltage range)...

   example:  https://www.amazon.com/gp/product/B004YTSB7C/ref=oh_aui_search_detailpage?ie=UTF8&psc=1



New fan plan: OA109AP-11/22-1WB

DC option: https://www.digikey.com/en/products/filter/fans/dc-brushless-fans-bldc/217?s=N4IgjCBcoCwGxVAYygMwIYBsDOBTANCAPZQDaIMADHABwwDMIAuoQA4AuUIAyuwE4BLAHYBzEAF9CYAJyNoIFJAw4CxMuGk0amkFM2yATLo0B2EwZjM2nSD37CxkkAYCsR%2BYuV5CJSOQOU9PQGEIQhYDSULlYgHFy8gqIShAC07shQ-ACuqr7k0UziRUA

Power supply option (maybe a bad idea? Maybe triac control is good enough?)
TUHS25F24 - https://www.digikey.com/en/products/detail/cosel-usa-inc/TUHS25F24/7044913


************************************************************************

