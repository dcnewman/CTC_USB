# CTC_USB

Code to manipulate the CTC USB+ dongle which contains a Microchip 11LC010-1 EEPROM chip.  This chip controls how long a CTC USB+ printer will operate.  Usually only a few hundred hours after which you must obtain a new USB "dongle" from CTC.

The dongle is **not** a USB device.  Do **not** insert it into a computer.

The Arduino sketch `ctc_usb_dump.ino` found here may be used to dump the memory in one of these dongles.  See the Flickr photo set https://www.flickr.com/photos/d-newman/sets/72157655345562039 for information on opening one of these devices and accessing the EEPROM contained within.  The EEPROM chip's data line (SOIC, Pin 5) goes to the Digital Pin 7 on an Arduino.  Do not plug the USB dongle into your CTC printer while it is also connected to an Arduino.

It should also be possible to rewrite the memory in a dongle and reset it to a new, unused state.  That will require having a new, unused dongle to dump.

