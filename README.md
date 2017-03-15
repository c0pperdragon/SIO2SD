# SIO2SD
An Arduino-based floppy drive emulator for the atari 8 bit computers.

Instead of just buying a complete SIO2SD device, I decided to build my own using my trusty Arduino Micro. 
This seems to have just the right number of pins for the purpose as well as the necessary interfaces. 
Support for an SDCARD library is already included in the Arduino IDE.

I used my own technique to manually wire all components to a board instead of creating a PCD for which I 
do not have the proper equipment. This task is quite time-consuming but much fun nevertheless.
The board layout file does not accurately contain all the components because I did not find them in 
the eagle library. So I just used generic pin-headers to denote the position of the female Arduino
socket and the female SDCARD connector socket. For some reasons the eagle library not even contains a 7-segment
digit display in the correct format, so again I used pin-headers here.

Instead of a standard SDCARD-socket I wired up one of these micro-SD-adapters to form a kind of breakout adapter
for a micro-SD card. By making it detachable I can probably use it for later prototyping projects. 

For the connection to the weird Atari SIO socket, I just built a my own wire connection from 5 individual pin-connectors
that fit on the 1-millimeter pins inside the socket. This looks a bit strange, but works very reliably.

How to use the device:
After being connected to the 5 correct pins from the atari port which also provides the power to drive the Arduino,
the device needs an SDCARD containing the necessary disk files. Currently only .ats files with a sector size of 128 byte 
(which seems to be the standard) are supported. Because there is no elaborated user-interface on the device, 
the naming and location of the files need to follow a fixed system: The files are searched for in a directory ATARI/
on the root of the SDCARD. Every disk file must be named nn_Anything.ats with two leading digits that correspond to the 
numeric display on the device and the mandatory '_' character. Two additional concurrent disk drives will be emulated when there 
are files with name nnBAnything.ats up to nnCAnything.ats. This can be useful to provide writeable save-disks for certain games.
D4 is not emulated, so it is possible to connect a real floppy drive together with this emulator to do file transfers. 
The total number of selectable configurations is 100 which is limited by the two digit display.
If more disk files are needed, they must be stored on another SDCARD.
With the buttons the number in the display can be changed to select a different disk file / set of files. 
If no file for the number can be found, the device emulates an empty floppy drive. 

Have a look at the gallery image to see how the parts are made and how everything connects.

## New Hardware Variants

* First prototype with Arduino Micro. 
This was hand-wired. No longer suppported by the firmware.

* Bare bone ATmega328
Designed to fit into a small form factor. MCU is running at 8MHz on 3.3V and can talk to SDCard without level shifters. 
LEDs have resistors on the digit select lines only, therefore the brightness various with the pattern.

* Breadboard setup with Arduino Nano
Having 8 resistors for the LED and a level shifter to the SDCard interface.
