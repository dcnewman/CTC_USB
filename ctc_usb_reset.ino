/*

  ***
  *** USE AT YOUR OWN RISK.  THIS MAY RENDER A CTC USB+ "dongle" USELESS.
  ***

  ctc_usb_reset.ino -- Dump the EEPROM contents of a CTC USB+ "dongle"

  See

    https://www.flickr.com/photos/d-newman/sets/72157655345562039

  for info on opening a CTC USB+ dongle and accessing the EEPROM chip.
  The EEPROM chip is a Microchip 11LC010-1 with 1 kbit of memory

  Signal line from CTC USB+ dongle goes to digital pin 7.

  Based entirely upon
  Da Vinci EEPROM update Copyright (C) 2014 by Oliver Fueckert <oliver@voltivo.com>
  UNI/O Library Copyright (C) 2011 by Stephen Early <steve@greenend.org.uk>

  11 July 2015
  Dan Newman <dan.newman@mtbaldy.us>

  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.  */

#ifndef _NANODEUNIO_LIB_H
#define _NANODEUNIO_LIB_H

#if ARDUINO >= 100
#include <Arduino.h> // Arduino 1.0
#else
#include <WProgram.h> // Arduino 0022
#endif

#define NANODE_MAC_DEVICE 0xa0
#define NANODE_MAC_ADDRESS 0xfa

static unsigned char reset_data[] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x32, 0x00, 0xFF,
	0x96, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF

};

void IncrementSerial(unsigned char * cArray, long lAddress, long lSize)
{
	unsigned char szTempBuffer[20] = {0};
	memcpy(szTempBuffer,&cArray[lAddress],lSize);
	long lSerial = atol((char *)szTempBuffer);
	lSerial++;
	sprintf((char *)szTempBuffer,"%04d",lSerial);
	memcpy(&cArray[lAddress],szTempBuffer,lSize);
}

class NanodeUNIO {
private:
	byte addr;
public:
	NanodeUNIO(byte address);

	boolean read(byte *buffer,word address,word length);
	boolean start_write(const byte *buffer,word address,word length);
	boolean enable_write(void);
	boolean disable_write(void);
	boolean read_status(byte *status);
	boolean write_status(byte status);
	boolean await_write_complete(void);
	boolean simple_write(const byte *buffer,word address,word length);
};

#endif /* _NANODEUNIO_LIB_H */

#define UNIO_STARTHEADER 0x55
#define UNIO_READ        0x03
#define UNIO_CRRD        0x06
#define UNIO_WRITE       0x6c
#define UNIO_WREN        0x96
#define UNIO_WRDI        0x91
#define UNIO_RDSR        0x05
#define UNIO_WRSR        0x6e
#define UNIO_ERAL        0x6d
#define UNIO_SETAL       0x67

#define UNIO_TSTBY 600
#define UNIO_TSS    10
#define UNIO_THDR    5
#define UNIO_QUARTER_BIT 10
#define UNIO_FUDGE_FACTOR 5
#define UNIO_OUTPUT() do { DDRD |= 0x80; } while (0)
#define UNIO_INPUT() do { DDRD &= 0x7f; } while (0)

static void set_bus(boolean state) {
	PORTD=(PORTD&0x7f)|(!!state)<<7;
}

static boolean read_bus(void) {
	return !!(PIND&0x80);
}
static void unio_inter_command_gap(void) {
	set_bus(1);
	delayMicroseconds(UNIO_TSS+UNIO_FUDGE_FACTOR);
}

static void unio_standby_pulse(void) {
	set_bus(0);
	UNIO_OUTPUT();
	delayMicroseconds(UNIO_TSS+UNIO_FUDGE_FACTOR);
	set_bus(1);
	delayMicroseconds(UNIO_TSTBY+UNIO_FUDGE_FACTOR);
}

static volatile boolean rwbit(boolean w) {
	boolean a,b;
	set_bus(!w);
	delayMicroseconds(UNIO_QUARTER_BIT);
	a=read_bus();
	delayMicroseconds(UNIO_QUARTER_BIT);
	set_bus(w);
	delayMicroseconds(UNIO_QUARTER_BIT);
	b=read_bus();
	delayMicroseconds(UNIO_QUARTER_BIT);
	return b&&!a;
}

static boolean read_bit(void) {
	boolean b;
	UNIO_INPUT();
	b=rwbit(1);
	UNIO_OUTPUT();
	return b;
}

static boolean send_byte(byte b, boolean mak) {
	for (int i=0; i<8; i++) {
		rwbit(b&0x80);
		b<<=1;
	}
	rwbit(mak);
	return read_bit();
}

static boolean read_byte(byte *b, boolean mak) {
	byte data=0;
	UNIO_INPUT();
	for (int i=0; i<8; i++) {
		data = (data << 1) | rwbit(1);
	}
	UNIO_OUTPUT();
	*b=data;
	rwbit(mak);
	return read_bit();
}

static boolean unio_send(const byte *data,word length,boolean end) {
	for (word i=0; i<length; i++) {
		if (!send_byte(data[i],!(((i+1)==length) && end))) return false;
	}
	return true;
}

static boolean unio_read(byte *data,word length)  {
	for (word i=0; i<length; i++) {
		if (!read_byte(data+i,!((i+1)==length))) return false;
	}
	return true;
}

static void unio_start_header(void) {
	set_bus(0);
	delayMicroseconds(UNIO_THDR+UNIO_FUDGE_FACTOR);
	send_byte(UNIO_STARTHEADER,true);
}

NanodeUNIO::NanodeUNIO(byte address) {
	addr=address;
}

#define fail() do { sei(); return false; } while (0)

boolean NanodeUNIO::read(byte *buffer,word address,word length) {
	byte cmd[4];
	cmd[0]=addr;
	cmd[1]=UNIO_READ;
	cmd[2]=(byte)(address>>8);
	cmd[3]=(byte)(address&0xff);
	unio_standby_pulse();
	cli();
	unio_start_header();
	if (!unio_send(cmd,4,false)) fail();
	if (!unio_read(buffer,length)) fail();
	sei();
	return true;
}

boolean NanodeUNIO::start_write(const byte *buffer,word address,word length) {
	byte cmd[4];
	if (((address&0x0f)+length)>16) return false; // would cross page boundary
	cmd[0]=addr;
	cmd[1]=UNIO_WRITE;
	cmd[2]=(byte)(address>>8);
	cmd[3]=(byte)(address&0xff);
	unio_standby_pulse();
	cli();
	unio_start_header();
	if (!unio_send(cmd,4,false)) fail();
	if (!unio_send(buffer,length,true)) fail();
	sei();
	return true;
}

boolean NanodeUNIO::enable_write(void) {
	byte cmd[2];
	cmd[0]=addr;
	cmd[1]=UNIO_WREN;
	unio_standby_pulse();
	cli();
	unio_start_header();
	if (!unio_send(cmd,2,true)) fail();
	sei();
	return true;
}

boolean NanodeUNIO::disable_write(void) {
	byte cmd[2];
	cmd[0]=addr;
	cmd[1]=UNIO_WRDI;
	unio_standby_pulse();
	cli();
	unio_start_header();
	if (!unio_send(cmd,2,true)) fail();
	sei();
	return true;
}

boolean NanodeUNIO::read_status(byte *status) {
	byte cmd[2];
	cmd[0]=addr;
	cmd[1]=UNIO_RDSR;
	unio_standby_pulse();
	cli();
	unio_start_header();
	if (!unio_send(cmd,2,false)) fail();
	if (!unio_read(status,1)) fail();
	sei();
	return true;
}

boolean NanodeUNIO::write_status(byte status) {
	byte cmd[3];
	cmd[0]=addr;
	cmd[1]=UNIO_WRSR;
	cmd[2]=status;
	unio_standby_pulse();
	cli();
	unio_start_header();
	if (!unio_send(cmd,3,true)) fail();
	sei();
	return true;
}

boolean NanodeUNIO::await_write_complete(void) {
	byte cmd[2];
	byte status;
	cmd[0]=addr;
	cmd[1]=UNIO_RDSR;
	unio_standby_pulse();
	do {
		unio_inter_command_gap();
		cli();
		unio_start_header();
		if (!unio_send(cmd,2,false)) fail();
		if (!unio_read(&status,1)) fail();
		sei();
	} while (status&0x01);
	return true;
}

boolean NanodeUNIO::simple_write(const byte *buffer,word address,word length) {
	word wlen;
	while (length>0) {
		wlen=length;
		if (((address&0x0f)+wlen)>16) {
			wlen=16-(address&0x0f);
		}
		if (!enable_write()) return false;
		if (!start_write(buffer,address,wlen)) return false;
		if (!await_write_complete()) return false;
		buffer+=wlen;
		address+=wlen;
		length-=wlen;
	}
	return true;
}

static void status(boolean r)
{
	if (r) Serial.println("(success)");
	else Serial.println("(failure)");
}

static void dump_eeprom(word address,word length)
{
	byte buf[128];
	char lbuf[80];
	char *x;
	int i,j;

	NanodeUNIO unio(NANODE_MAC_DEVICE);

	memset(buf,0,128);
	status(unio.read(buf,address,length));

	for (i=0; i<128; i+=16) {
		x=lbuf;
		sprintf(x,"%02X: ",i);
		x+=4;
		for (j=0; j<16; j++) {
			sprintf(x,"%02X",buf[i+j]);
			x+=2;
		}
		*x=32;
		x+=1;
		for (j=0; j<16; j++) {
			if (buf[i+j]>=32 && buf[i+j]<127) *x=buf[i+j];
			else *x=46;
			x++;
		}
		*x=0;
		Serial.println(lbuf);
	}
}

#define LED_PIN 13

byte sr;
NanodeUNIO unio(NANODE_MAC_DEVICE);

void setup() {
	pinMode(LED_PIN, OUTPUT);
	Serial.begin(115200);
}

void loop() {

	do {
		digitalWrite(LED_PIN, LOW);
		Serial.println("Testing connection to CTC USB+ EEPROM CHIP\n");
		delay(100);
		digitalWrite(LED_PIN, HIGH);
	} while (!unio.read_status(&sr));

	Serial.println("CTC USB+ EEPROM found...");
	Serial.println("Dumping the old CTC USB+ EEPROM contents...");
	dump_eeprom(0, 128);

	delay(100);
	Serial.println("Resetting the CTC USB+ EEPROM Contents...");
	status(unio.simple_write((const byte *)reset_data, 0, sizeof(reset_data)));

	delay(100);

	Serial.println("Dumping the new CTC USB+ EEPROM contents...");
	dump_eeprom(0, 128);

	digitalWrite(LED_PIN, HIGH);   // turn the LED on
	delay(100000);             // wait for ten seconds
}
