#include "CEC_Player.h"
#include <stdio.h>
#include <stdlib.h>

CEC_Player::CEC_Player(int physicalAddress)
: CEC_Device(physicalAddress)
{
  _physicalAddress = physicalAddress;
  ptr = (unsigned char *)&_physicalAddress;
  debug = false;
  raw = false;
}

CEC_Player::~CEC_Player()
{
}

void CEC_Player::OnReady()
{
  Transmit(0, 5, COP_SET_OSD_NAME, 'H', 'T', 'P', 'C'); // <Set OSD Name> "HTPC"
}

void print_buffer(unsigned char *buf, int len)
{
  for (int i = 0; i < len; i++)
    DbgPrint("%02X ", buf[i]);
}

void CEC_Player::Transmit(int targetAddress, int count ...)
{
  unsigned char buffer[count];
  va_list args;
  va_start(args, count);
  for( int i = 0 ; i < count; i++ )
    buffer[i] = va_arg( args, int );
  va_end(args);
  TransmitFrame(targetAddress, buffer, count);

  if (debug)
  {
    DbgPrint("SEND    -> %2d \t", targetAddress);
    cec_debug(buffer, count);
    DbgPrint("\n");
  }
  if (raw)
  {
    DbgPrint("SEND    -> %2d \t", targetAddress);
    print_buffer(buffer, count);
    DbgPrint("\n");
  }
}

void UserControl(unsigned char input)
{
  DbgPrint("CTRL  %02X\n", input);
}

void CEC_Player::OnReceive(int source, int dest, unsigned char* buffer, int count)
{
  // This is called when a frame is received. To receive all frames, even
  // those not addressed to this device, set Promiscuous to true.

  static unsigned int deck_status = 0x1a; // Stop
  static bool report_deck_status = false;

  if (debug)
  {
    DbgPrint("RECV %2d -> %2d \t", source, dest);
    cec_debug(buffer, count);
    DbgPrint("\n");
  }
  if (raw)
  {
    DbgPrint("RECV %2d -> %2d \t", source, dest);
    print_buffer(buffer, count);
    DbgPrint("\n");
  }

  if (count) switch (buffer[0])
  {
    // INFORMATION
    case COP_GIVE_OSD_NAME:
      Transmit(source, 5, COP_SET_OSD_NAME, 'H', 'T', 'P', 'C'); // <Set OSD Name> "HTPC"
      break;
    case COP_GET_CEC_VERSION:
      Transmit(source, 2, COP_CEC_VERSION, 0x04); // <CEC Version> 1.3a
      break;
    case COP_GIVE_PHYSICAL_ADDRESS:
      Transmit(15, 4, COP_REPORT_PHYSICAL_ADDRESS, ptr[1], ptr[0], 0x04); // repotr physical address & playback device
      break;
    case COP_GIVE_DEVICE_POWER_STATUS:
      Transmit(source, 2, COP_REPORT_POWER_STATUS, 0x00); // <Report Power Status> ON
      break;

    // SOURCE SWITCHING
    case COP_REQUEST_ACTIVE_SOURCE:
      Transmit(15, 3, COP_ACTIVE_SOURCE, ptr[1], ptr[0]); // <Active Source> physicalAddress
      break;
    case COP_SET_STREAM_PATH:
      if (buffer[1] == ptr[1] && buffer[2] == ptr[0]) // you talking to me?
        Transmit(15, 3, COP_ACTIVE_SOURCE, ptr[1], ptr[0]); // <Active Source> physicalAddress
      break;

    // USER CONTROL
    case COP_MENU_REQUEST:
      if (buffer[1] == 0x01) // <Menu Request> "disable"
        Transmit(source, 3, COP_FEATURE_ABORT, COP_MENU_REQUEST, 0x04); // Refuse!
      Transmit(source, 2, COP_MENU_STATUS, 0x00); // always activated!
      break;
    case COP_USER_CONTROL_PRESSED:
      UserControl(buffer[1]);
      break;
    case COP_USER_CONTROL_RELEASED:
      // we can probbaly ignore this one
      break;
    case COP_VENDOR_REMOTE_BUTTON_DOWN: 
      UserControl(buffer[1]);
      break;

    // DECK STATUS & CONTROL
    case COP_GIVE_DECK_STATUS:
      switch (buffer[1])
      {
        case 1: // on
          report_deck_status = true;
          // no break
        case 3: // once
          Transmit(source, 2, COP_DECK_STATUS, deck_status);
          break;
        case 2: // off
          report_deck_status = false;
          break;
      }
      break;
    case COP_DECK_CONTROL:
      switch (buffer[1])
      {
        case 1: // Skip Forward / Wind
          UserControl(0x49); // Fast Forward
          break;
        case 2: // Skip Reverse / Rewind
          UserControl(0x48); // Rewind
          break;
        case 3: // Stop
          UserControl(0x45); // Stop
          deck_status = 0x1a; // Stop
          break;
        case 4: // Eject
          UserControl(0x4a); // Eject
          deck_status = 0x1a; // Stop
          break;
      }
      if (report_deck_status) Transmit(source, 2, COP_DECK_STATUS, deck_status);
      break;
    case COP_PLAY:
      switch (buffer[1])
      {
        case 0x24: // Play Forward
          deck_status = 0x11; // play
          UserControl(0x44); // Play
          break;
        case 0x25: // Play Still
          deck_status = 0x14; // still
          UserControl(0x46); // Pause
          break;
        case 0x05: // Fast Forward Min Speed
        case 0x06: // Fast Forward Medium Speed
        case 0x07: // Fast Forward Max Speed
        case 0x15: // Slow Forward Min Speed
        case 0x16: // Slow Forward Medium Speed
        case 0x17: // Slow Forward Max Speed
          deck_status = 0x17; // fast forward
          UserControl(0x49); // Fast Forward
          break;
        case 0x20: // Play Reverse
        case 0x09: // Fast Reverse Min Speed
        case 0x0A: // Fast Reverse Medium Speed
        case 0x0B: // Fast Reverse Max Speed
        case 0x19: // Slow Reverse Min Speed
        case 0x1A: // Slow Reverse Medium Speed
        case 0x1B: // Slow Reverse Max Speed
          deck_status = 0x18; // fast reverse
          UserControl(0x48); // Rewind
          break;
      }
      if (report_deck_status) Transmit(source, 2, COP_DECK_STATUS, deck_status);
      break;
      
    // IGNORE THESE MESSAGES
    case COP_FEATURE_ABORT:
      break;

    // FEATURE ABORT THESE MESSAGES!!
    case COP_GIVE_DEVICE_VENDOR_ID:
    default:
      if (dest != 15)
        Transmit(source, 3, COP_FEATURE_ABORT, buffer[0], 0x00); // <Feature Abort> "Unrecognised Opcode"
  }
}

