#include "CEC_Player.h"

#define IN_LINE 10 // was 2
#define OUT_LINE 11 // was 3

CEC_Player device(0x1000);

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
    
    switch (c)
    {
      case 'd':
        device.debug = ! device.debug;
        Serial.print("DEBUG ");
        Serial.println(device.debug?"ON":"OFF");
        break;
      case 'r':
        device.raw = ! device.raw;
        Serial.print("RAW ");
        Serial.println(device.raw?"ON":"OFF");
        break;
      case '0':
        device.Transmit(15, 1, COP_STANDBY);
        break;
      case '1':
        device.Transmit(0,  1, COP_IMAGE_VIEW_ON);
        break;
      case '2':
        device.Transmit(15, 3, COP_ACTIVE_SOURCE, 0x10, 0x00);
        break;
      default:
        Serial.println("HELP (d) DEBUG (r) RAW (0) TV OFF (1) TV ON (2) HTPC");
        break;
    }
  }
  device.Run();
}
