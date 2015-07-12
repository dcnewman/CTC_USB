# CTC_USB

Code to manipulate the CTC USB+ dongle which contains a Microchip 11LC010-1 EEPROM chip used to control how long a CTC USB+ printer will operate.  Usually only a few hundred hours after which you need to obtain a new USB "dongle" from CTC.

The dongle is *not* a USB device.  Do *not* insert it into a computer.

The Arduino sketch found here may be used to dump the memory in one of these dongles.  See the Flickr photo set https://www.flickr.com/photos/d-newman/sets/72157655345562039 for information on opening one of these devices and accessing the EEPROM contained within.

