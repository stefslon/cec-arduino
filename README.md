CEC portion of this code is a compilation of work done by phil123 and AndrewNC: https://code.google.com/p/cec-arduino/
Code license: GNU GPL v2

Some additional CEC work posted by Biffidus over at Arduino forum: http://forum.arduino.cc/index.php?topic=22327.60
No license posted.

HID-class USB communication using V-USB adopted to Arduino by Ray Wang at Rayshobby LLC: https://github.com/rayshobby/hid-serial
Creative Commons Attribution-ShareAlike (CC-SA) 3.0 license.

## Overview

This is an Arduino project which implements the HDMI v1.3a CEC wire protocol which allows communication with HDMI CEC capable devices. A typical usage scenario would be a Home Theater PC environment which uses HDMI but does not support CEC. This would allow the HTPC to communicate with other HDMI CEC equipment.

Tested with Arduino 1.6.5 using ATtiny85.

Here is the folder structure:
- firmware
	- cec-arduino -- contains Arduino code.
	- libraries -- contains V-USB library that needs to be copied into your Arduino libraries folder. 
- schematic -- contains cirtcuit schematic for interfacing Arduino/ATtiny85 to HDMI CEC line.

## Other Resources

See http://www.cec-o-matic.com/index.php for a list of CEC op-codes.