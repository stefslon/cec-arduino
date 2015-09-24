#include "CEC.h"
#include "USBComm.h"
#include <usbdrv.h>

/*  
  Used shared PID/VID:
  PID dec (hex) | VID dec (hex) | Description of use
  ==============+===============+============================================
  1503 (0x05df) | 5824 (0x16c0) | For generic HID class devices (which are
                |               | NOT mice, keyboards or joysticks)

  #define USB_CFG_VENDOR_NAME     's','t','e','f','s','l','o','n','@','g','m','a','i','l','.','c','o','m'
  #define USB_CFG_DEVICE_NAME     'C','E','C','-','C','o','n','t','r','o','l','l','e','r'
*/

// On ATtiny85 these are PB0 and PB2
#define IN_LINE 0
#define OUT_LINE 2

// List device types:
//  0x1000 -- Recording 1
//  0x2000 -- Recording 2
//  0x3000 -- Tuner 1
//  0x4000 -- Playback 1

//CEC_Device device(0x1000);
extern CEC_Device device(0x4000);
HIDSerial serial;

unsigned char serBuffer[32];


bool User_GetLineState()
{
  int state = digitalRead(IN_LINE);
  return state == LOW;
}

void User_SetLineState(CEC_Device* device, bool state)
{
  digitalWrite(OUT_LINE, state?LOW:HIGH);
  // give enough time for the line to settle before sampling it
  delayMicroseconds(50);
  device->_lastLineState2 = User_GetLineState();
}

void User_OnReady()
{
  // This is called after the logical address has been allocated
  //serial.write('K');
}

void User_OnReceive(unsigned char* rxBuffer, int count)
{
  // This is called when a frame is received.  To transmit
  // a frame call TransmitFrame.  To receive all frames, even
  // those not addressed to this device, set Promiscuous to true.
  
  //DbgPrint("Packet received at %ld: %d -> %d\n", millis(), source, dest);
  //for (int i = 0; i < count; i++)
  //  serial.write(rxBuffer[i]);
  serial.write(rxBuffer,count);
}

/********************************/




/********************************/




void setup()
{
  pinMode(OUT_LINE, OUTPUT);
  pinMode(IN_LINE, INPUT);

  digitalWrite(OUT_LINE, LOW);
  delay(200);

  serial.begin();
  
  device.MonitorMode = false;  // only receive and do not transmit
  device.Promiscuous = true;   // listen in on all CEC line transmissions
  device.Initialize(CEC_Device::CDT_PLAYBACK_DEVICE);
}

void loop()
{

  // device.TransmitFrame(target, cmdbuffer, buffpos);
  
  if(serial.available()) {
    int serSize = serial.read(serBuffer);
    if (serSize!=0) {
     
      bool retState = device.TransmitFrame(serBuffer[0], serBuffer+1, serSize-1);
      //serial.write(retState?'K':'F');

      //bool retState = device.TransmitFrame(1, serBuffer, serSize);
      //serial.write(retState?'K':'F');
    }
  }

  serial.poll();
  device.Run();
}


