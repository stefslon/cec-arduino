#include "CEC_Device.h"

#define IN_LINE 2
#define OUT_LINE 3

CEC_Device device(0x1000);

bool XX_GetLineState()
{
  int state = digitalRead(IN_LINE);
  return state == LOW;
}

void XX_SetLineState(CEC_Device* device, bool state)
{
  digitalWrite(OUT_LINE, state?LOW:HIGH);
  // give enough time for the line to settle before sampling
  // it
  delayMicroseconds(50);
  device->_lastLineState2 = XX_GetLineState();
}

void setup()
{
  pinMode(OUT_LINE, OUTPUT);
  pinMode(IN_LINE, INPUT);

  digitalWrite(OUT_LINE, LOW);
  delay(200);

  Serial.begin(115200);
  device.MonitorMode = true;
  device.Promiscuous = true;
  device.Initialize(CEC_LogicalDevice::CDT_PLAYBACK_DEVICE);
}

void loop()
{
  if (Serial.available())
  {
    unsigned char c = Serial.read();
    unsigned char buffer[3];
    
    Serial.print(" Read: ");
    Serial.println(char(c));
    
    switch (c)
    {
      case '1':
        DbgPrint("<Image View On>\r\n");
        buffer[0] = 0x04;
        device.TransmitFrame(0, buffer, 1);
        
        DbgPrint("<Set Stream Path >\r\n"); // tv HDMI1, Reciever HDMI4 selected
        buffer[0] = 0x82; 
        buffer[1] = 0x14;
        buffer[2] = 0x00;
        device.TransmitFrame(15, buffer, 3);
      
      case '2':
        DbgPrint("<Standby>\r\n");
        buffer[0] = 0x36;   // <Standby>
        device.TransmitFrame(15, buffer, 1);
        break;
        
      case 'w':
        DbgPrint("<Send OSD name>\r\n");
        buffer[0] = 0x47; // <Set OSD Name>
        memcpy(buffer+1, "Andrew-HTPC", 11);
        device.TransmitFrame(target, buffer, 12);
        break; 
        
    }
  }
  device.Run();
}


