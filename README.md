# SIO2SD
An Arduino-based floppy drive emulator for the atari 8 bit computers.

Instead of just buying a complete SIO2SD device, I decided to build my own using my trusty Arduino Micro. 
This seems to have just the right number of pins for the purpose as well as the necessary interfaces. 
Support for an SDCARD library is already included in the Arduino IDE.

I used my own technique to manually wire all components to a board instead of creating a PCD for which I 
do not have the proper equippment. This task is quite time-consuming but much fun nevertheless.
The board layout file does not accurately contain all the components because I did not find them in 
the eagle library. So I just used generic pin-headers to the denote the position of the female arduino
socket and the female sdcard connector socket. For some reasons the eagle library not even contains a 7-segment
digit display in the correct format, so again I used pin-headers here.

Instead of a standard sdcard-socket I wired up one of these micro-sd-adapters to form a kind of breakout adapter
for a micro-sd card. By making it detachable I can probably use it for later prototyping projects. 

For the connection to the weird atari SIO socket, I just built a my own wire connection from 5 individual pin-connectors
that fit on the 1-millimeter pins inside the socket. This looks a bit strange, but works very reliably.

How to use the device:
After being connected to the 5 correct pins from the atari port which also provides the power to drive the arduino,
the device needs a SDCARD containing the necessary disk files. Currently only .ats files with a sector size of 128 byte 
(which seems to be the standard) are supported. Because there is no elaborated user-interface on the device, 
the naming and location of the files need to follow a fixed system: The files are searched for in a directory ATARI/
on the root of the SDCARD. Every disk file must be named nnANYTNG.ATS with two leading digits that correspond to the 
numeric display on the device. Only one disk file per 2-digit-code can be selected - this limits the number
of disk files to 100. If more disk files are needed, they must be stored on another SDCARD.
With the 4 buttons the number in the display can be changed (+1, -1, +10, -10) to select a different file. 
If no file for the number can be found, the device emulates an empty floppy drive. 
