#include "WProgram.h"
#include <avr/pgmspace.h>
#include "CEC_Debug.h"
#include "CEC_OpCodes.h"
#include "Common.h"

// OpCode String Table

char *op_code(char op)
{
  switch (op)
  {
    case COP_ABORT: return PSTR("Abort");
    case COP_ACTIVE_SOURCE: return PSTR("Active Source");
    case COP_CEC_VERSION: return PSTR("CEC Version");
    case COP_CLEAR_ANALOGUE_TIMER: return PSTR("Clear Analogue Timer");
    case COP_CLEAR_DIGITAL_TIMER: return PSTR("Clear Digital Timer");
    case COP_CLEAR_EXTERNAL_TIMER: return PSTR("Clear External Timer");
    case COP_DECK_CONTROL: return PSTR("Deck Control");
    case COP_DECK_STATUS: return PSTR("Deck Status");
    case COP_DEVICE_VENDOR_ID: return PSTR("Device Vendor ID");
    case COP_FEATURE_ABORT: return PSTR("Feature Abort");
    case COP_GET_CEC_VERSION: return PSTR("Get CEC Version");
    case COP_GET_MENU_LANGUAGE: return PSTR("Get Menu Language");
    case COP_GIVE_AUDIO_STATUS: return PSTR("Give Audio Status");
    case COP_GIVE_DECK_STATUS: return PSTR("Give Deck Status");
    case COP_GIVE_DEVICE_POWER_STATUS: return PSTR("Give Device Power Status");
    case COP_GIVE_DEVICE_VENDOR_ID: return PSTR("Give Device Vendor ID");
    case COP_GIVE_OSD_NAME: return PSTR("Give OSD Name");
    case COP_GIVE_PHYSICAL_ADDRESS: return PSTR("Give Physical Address");
    case COP_GIVE_SYSTEM_AUDIO_MODE_STATUS: return PSTR("Give System audio Mode Status");
    case COP_GIVE_TUNER_DEVICE_STATUS: return PSTR("Give Tuner Device Status");
    case COP_IMAGE_VIEW_ON: return PSTR("Image View On");
    case COP_INACTIVE_SOURCE: return PSTR("Inactive Source");
    case COP_MENU_REQUEST: return PSTR("Menu Request");
    case COP_MENU_STATUS: return PSTR("Menu Status");
    case COP_PLAY: return PSTR("Play");
    case COP_RECORD_OFF: return PSTR("Record Off");
    case COP_RECORD_ON: return PSTR("Record On");
    case COP_RECORD_STATUS: return PSTR("Record Status");
    case COP_RECORD_TV_SCREEN: return PSTR("Record TV Screen");
    case COP_REPORT_AUDIO_STATUS: return PSTR("Report Audio Status");
    case COP_REPORT_PHYSICAL_ADDRESS: return PSTR("Report Physical Address");
    case COP_REPORT_POWER_STATUS: return PSTR("Report Power Status");
    case COP_REQUEST_ACTIVE_SOURCE: return PSTR("Request Active Source");
    case COP_ROUTING_CHANGE: return PSTR("Routing Change");
    case COP_ROUTING_INFORMATION: return PSTR("Routing Information");
    case COP_SELECT_ANALOGUE_SERVICE: return PSTR("Select Analogue Service");
    case COP_SELECT_DIGITAL_SERVICE: return PSTR("Select Digital Service");
    case COP_SET_ANALOGUE_TIMER: return PSTR("Set Analogue Timer");
    case COP_SET_AUDIO_RATE: return PSTR("Set Audio Rate");
    case COP_SET_DIGITAL_TIMER: return PSTR("Set Digital Timer");
    case COP_SET_EXTERNAL_TIMER: return PSTR("Set External Timer");
    case COP_SET_MENU_LANGUAGE: return PSTR("Set Menu Language");
    case COP_SET_OSD_NAME: return PSTR("Set OSD Name");
    case COP_SET_OSD_STRING: return PSTR("Set OSD String");
    case COP_SET_STREAM_PATH: return PSTR("Set Stream Path");
    case COP_SET_SYSTEM_AUDIO_MODE: return PSTR("Set System Audio Mode");
    case COP_SET_TIMER_PROGRAM_TITLE: return PSTR("Set Timer Program Title");
    case COP_STANDBY: return PSTR("Standby");
    case COP_SYSTEM_AUDIO_MODE_REQUEST: return PSTR("System Audio Mode Request");
    case COP_SYSTEM_AUDIO_MODE_STATUS: return PSTR("System Audio Mode Status");
    case COP_TEXT_VIEW_ON: return PSTR("Text View On");
    case COP_TIMER_CLEARED_STATUS: return PSTR("Timer Cleared Status");
    case COP_TIMER_STATUS: return PSTR("Timer Status");
    case COP_TUNER_DEVICE_STATUS: return PSTR("Tuner Device Status");
    case COP_TUNER_STEP_DECREMENT: return PSTR("Tuner Step Decrement");
    case COP_TUNER_STEP_INCREMENT: return PSTR("Tuner Step Increment");
    case COP_USER_CONTROL_PRESSED: return PSTR("User Control Pressed");
    case COP_USER_CONTROL_RELEASED: return PSTR("User Control Released");
    case COP_VENDOR_COMMAND: return PSTR("Vendor Command");
    case COP_VENDOR_COMMAND_WITH_ID: return PSTR("Vendor Command With ID");
    case COP_VENDOR_REMOTE_BUTTON_DOWN: return PSTR("Vendor Remote Button Down");
    case COP_VENDOR_REMOTE_BUTTON_UP: return PSTR("Vendor Remote Button Up");
    default: return PSTR("Unknown");
  }
}
char *abort_reason(char c)
{
  switch (c)
  {
    case 0: return PSTR("Unrecognized opcode");
    case 1: return PSTR("Not in correct mode to respond");
    case 2: return PSTR("Cannot provide source");
    case 3: return PSTR("Invalid operand");
    case 4: return PSTR("Refused");
    default: return PSTR("???");
  }
}
char *analogue_broadcast_type(char c)
{
  switch (c)
  {
    case 0x00: return PSTR("Cable");
    case 0x01: return PSTR("Satellite");
    case 0x02: return PSTR("Terrestrial");
    default: return PSTR("???");
  }
}
char *audio_rate(char c)
{
  switch (c)
  {
    case 0: return PSTR("Rate Control Off");
    case 1:
    case 4: return PSTR("Standard Rate");
    case 2:
    case 5: return PSTR("Fast Rate");
    case 3:
    case 6: return PSTR("Slow Rate");
    default: return PSTR("???");
  }
}
char *audio_mute_status(bool b)
{
  return b?PSTR("Audio Mute On"):PSTR("Audio Mute Off");
}
char *cec_boolean(bool b)
{
  return b?PSTR("True"):PSTR("False");
}
char *cec_version(char c)
{
  switch (c)
  {
    case 0x00: return PSTR("Version 1.1");
    case 0x01: return PSTR("Version 1.2");
    case 0x02: return PSTR("Version 1.2a");
    case 0x03: return PSTR("Version 1.3");
    case 0x04: return PSTR("Version 1.3a");
    default: return PSTR("???");
  }
}
char *channel_number_format(char c)
{
  switch (c)
  {
    case 0x01: return PSTR("1-part Channel Number");
    case 0x02: return PSTR("2-part Channel Number");
    default: return PSTR("???");
  }
}
char *deck_control_mode(char c)
{
  switch (c)
  {
    case 1: return PSTR("Skip Forward / Wind");
    case 2: return PSTR("Skip Reverse / Rewind");
    case 3: return PSTR("Stop");
    case 4: return PSTR("Eject");
    default: return PSTR("???");
  }
}
char *deck_info(char c)
{
  switch (c)
  {
    case 0x11: return PSTR("Play");
    case 0x12: return PSTR("Record");
    case 0x13: return PSTR("Play Reverse");
    case 0x14: return PSTR("Still");
    case 0x15: return PSTR("Slow");
    case 0x16: return PSTR("Slow Reverse");
    case 0x17: return PSTR("Fast Forward");
    case 0x18: return PSTR("Fast Reverse");
    case 0x19: return PSTR("No Media");
    case 0x1A: return PSTR("Stop");
    case 0x1B: return PSTR("Skip Forward / Wind");
    case 0x1C: return PSTR("Skip Reverse / Rewind");
    case 0x1D: return PSTR("Index Search Forward");
    case 0x1E: return PSTR("Index Search Reverse");
    case 0x1F: return PSTR("Other Status");
    default: return PSTR("???");
  }
}
char *device_type(char c)
{
  switch (c)
  {
    case 0: return PSTR("TV");
    case 1: return PSTR("Recording Device");
    case 3: return PSTR("Tuner");
    case 4: return PSTR("Playback Device");
    case 5: return PSTR("Audio System");
    default: return PSTR("???");
  }
}
char *service_identification_method(bool b)
{
  return b?PSTR("Service identified by Channel"):PSTR("Service identified by Digital IDs");
}
char *digital_broadcast_system(char c)
{
  switch (c)
  {
    case 0x00: return PSTR("ARIB generic");
    case 0x01: return PSTR("ATSC generic");
    case 0x02: return PSTR("DVB generic");
    case 0x08: return PSTR("ARIB-BS");
    case 0x09: return PSTR("ARIB-CS");
    case 0x0a: return PSTR("ARIB-T");
    case 0x10: return PSTR("ATSC Cable");
    case 0x11: return PSTR("ATSC Satellite");
    case 0x12: return PSTR("ATSC Terrestrial");
    case 0x18: return PSTR("DVB-C");
    case 0x19: return PSTR("DVB-S");
    case 0x1a: return PSTR("DVB S2");
    case 0x1b: return PSTR("DVB-T");
    default: return PSTR("???");
  }
}
char *display_control(char c)
{
  switch (c)
  {
    case 0b00000000: return PSTR("Display for the default time");
    case 0b01000000: return PSTR("Display until cleared");
    case 0b10000000: return PSTR("Clear previous message");
    default: return PSTR("???");
  }
}
char *external_source_specifier(char c)
{
  switch (c)
  {
    case 4: return PSTR("External Plug");
    case 5: return PSTR("External Physical Address");
    default: return PSTR("???");
  }
}
char *menu_request_type(char c)
{
  switch (c)
  {
    case 0: return PSTR("Activate");
    case 1: return PSTR("Deactivate");
    case 2: return PSTR("Query");
    default: return PSTR("???");
  }
}
char *menu_state(char c)
{
  return c?PSTR("Deactivated"):PSTR("Activated");
}
char *play_mode(char c)
{
  switch (c)
  {
    case 0x24: return PSTR("Play Forward");
    case 0x20: return PSTR("Play Reverse");
    case 0x25: return PSTR("Play Still");
    case 0x05: return PSTR("Fast Forward Min Speed");
    case 0x06: return PSTR("Fast Forward Medium Speed");
    case 0x07: return PSTR("Fast Forward Max Speed");
    case 0x09: return PSTR("Fast Reverse Min Speed");
    case 0x0a: return PSTR("Fast Reverse Medium Speed");
    case 0x0b: return PSTR("Fast Reverse Max Speed");
    case 0x15: return PSTR("Slow Forward Min Speed");
    case 0x16: return PSTR("Slow Forward Medium Speed");
    case 0x17: return PSTR("Slow Forward Max Speed");
    case 0x19: return PSTR("Slow Reverse Min Speed");
    case 0x1a: return PSTR("Slow Reverse Medium Speed");
    case 0x1b: return PSTR("Slow Reverse Max Speed");
    default: return PSTR("???");
  }
}
char *power_status(char c)
{
  switch (c)
  {
    case 0x00: return PSTR("On");
    case 0x01: return PSTR("Standby");
    case 0x02: return PSTR("In transition Standby to On");
    case 0x03: return PSTR("In transition On to Standby");
    default: return PSTR("???");
  }
}
char *record_source_type(char c)
{
  switch (c)
  {
    case 1: return PSTR("Own Source");
    case 2: return PSTR("Digital Service");
    case 3: return PSTR("Analogue Service");
    case 4: return PSTR("External Plug");
    case 5: return PSTR("External Physical Address");
    default: return PSTR("???");
  }
}
char *record_status_info(char c)
{
  switch (c)
  {
    case 0x01: return PSTR("Recording currently selected source");
    case 0x02: return PSTR("Recording Digital Service");
    case 0x03: return PSTR("Recording Analogue Service");
    case 0x04: return PSTR("Recording External Input");
    case 0x05: return PSTR("No recording - unable to record Digital Service");
    case 0x06: return PSTR("No recording - unable to record Analogue Service");
    case 0x07: return PSTR("No recording - unable to select required source");
    case 0x09: return PSTR("No recording - invalid External plug number");
    case 0x0a: return PSTR("No recording - invalid External Physical Address");
    case 0x0b: return PSTR("No recording - CA system not supported");
    case 0x0c: return PSTR("No recording - No or Insufficient CA Entitlements");
    case 0x0d: return PSTR("No recording - Not allowed to copy source");
    case 0x0e: return PSTR("No recording - No further copies allowed");
    case 0x10: return PSTR("No recording - no media");
    case 0x11: return PSTR("No recording - playing");
    case 0x12: return PSTR("No recording - already recording");
    case 0x13: return PSTR("No recording - media protected");
    case 0x14: return PSTR("No recording - no source signal");
    case 0x15: return PSTR("No recording - media problem");
    case 0x16: return PSTR("No recording - not enough space available");
    case 0x17: return PSTR("No recording - Parental Lock On");
    case 0x1a: return PSTR("Recording terminated normally");
    case 0x1b: return PSTR("Recording has already terminated");
    case 0x1f: return PSTR("No recording - other reason");
    default: return PSTR("???");
  }
}
char *recording_sequence(char c)
{
  switch (c)
  {
    case 0b0000001: return PSTR("Sunday");
    case 0b0000010: return PSTR("Monday");
    case 0b0000100: return PSTR("Tuesday");
    case 0b0001000: return PSTR("Wednesday");
    case 0b0010000: return PSTR("Thursday");
    case 0b0100000: return PSTR("Friday");
    case 0b1000000: return PSTR("Saturday");
    case 0b0000000: return PSTR("Once only");
    default: return PSTR("???");
  }
}
char *status_request(char c)
{
  switch (c)
  {
    case 1: return PSTR("On");
    case 2: return PSTR("Off");
    case 3: return PSTR("Once");
    default: return PSTR("???");
  }
}
char *system_audio_status(bool b)
{
  return b?PSTR("On"):PSTR("Off");
}
char *timer_cleared_status_data(char c)
{
  switch (c)
  {
    case 0x00: return PSTR("Timer not cleared - recording");
    case 0x01: return PSTR("Timer not cleared - no matching");
    case 0x02: return PSTR("Timer not cleared - no info available");
    case 0x80: return PSTR("Timer cleared");
    default: return PSTR("???");
  }
}
char *timer_overlap_warning(bool b)
{
  return b?PSTR("Timer blocks overlap"):PSTR("No overlap");
}
char *media_info(char c)
{
  switch (c)
  {
    case 0b00: return PSTR("Media present and not protected");
    case 0b01: return PSTR("Media present, but protected");
    case 0b10: return PSTR("Media not present");
    default: return PSTR("???");
  }
}
char *programmed_indicator(bool b)
{
  return b?PSTR("Programmed"):PSTR("Not programmed");
}
char *programmed_info(char c)
{
  switch (c)
  {
    case 0b1000: return PSTR("Enough space available for recording");
    case 0b1001: return PSTR("Not enough space available for recording");
    case 0b1011: return PSTR("May not be enough space available");
    case 0b1010: return PSTR("No Media Info available");
    default: return PSTR("???");
  }
}
char *not_programmed_error_info(char c)
{
  switch (c)
  {
    case 0b0001: return PSTR("No free timer available");
    case 0b0010: return PSTR("Date out of range");
    case 0b0011: return PSTR("Recording Sequence error");
    case 0b0100: return PSTR("Invalid External Plug Number");
    case 0b0101: return PSTR("Invalid External Physical Address");
    case 0b0110: return PSTR("CA system not supported");
    case 0b0111: return PSTR("No or insufficient CA Entitlements");
    case 0b1000: return PSTR("Does not support resolution");
    case 0b1001: return PSTR("Parental Lock on");
    case 0b1010: return PSTR("Clock Failure");
    case 0b1110: return PSTR("Duplicate: already programmed");
    default: return PSTR("???");
  }
}
char *recording_flag(bool b)
{
  return b?PSTR("Being used for recording"):PSTR("Not being used for recording");
}
char *tuner_display_info(char c)
{
  switch (c)
  {
    case 0: return PSTR("Displaying Digital Tuner");
    case 1: return PSTR("Not displaying Tuner");
    case 2: return PSTR("Displaying Analogue tuner");
    default: return PSTR("???");
  }
}
char *user_control_codes(char c)
{
  switch (c)
  {
    case 0x00: return PSTR("Select");
    case 0x01: return PSTR("Up");
    case 0x02: return PSTR("Down");
    case 0x03: return PSTR("Left");
    case 0x04: return PSTR("Right");
    case 0x05: return PSTR("Right-Up");
    case 0x06: return PSTR("Right-Down");
    case 0x07: return PSTR("Left-Up");
    case 0x08: return PSTR("Left-Down");
    case 0x09: return PSTR("Root Menu");
    case 0x0a: return PSTR("Setup Menu");
    case 0x0b: return PSTR("Contents Menu");
    case 0x0c: return PSTR("Favorite Menu");
    case 0x0d: return PSTR("Exit");
    case 0x20: return PSTR("0");
    case 0x21: return PSTR("1");
    case 0x22: return PSTR("2");
    case 0x23: return PSTR("3");
    case 0x24: return PSTR("4");
    case 0x25: return PSTR("5");
    case 0x26: return PSTR("6");
    case 0x27: return PSTR("7");
    case 0x28: return PSTR("8");
    case 0x29: return PSTR("9");
    case 0x2a: return PSTR("Dot");
    case 0x2b: return PSTR("Enter");
    case 0x2c: return PSTR("Clear");
    case 0x2f: return PSTR("Next Favorite");
    case 0x30: return PSTR("Channel Up");
    case 0x31: return PSTR("Channel Down");
    case 0x32: return PSTR("Previous Channel");
    case 0x33: return PSTR("Sound Select");
    case 0x34: return PSTR("Input Select");
    case 0x35: return PSTR("Display Information");
    case 0x36: return PSTR("Help");
    case 0x37: return PSTR("Page Up");
    case 0x38: return PSTR("Page Down");
    case 0x40: return PSTR("Power");
    case 0x41: return PSTR("Volume Up");
    case 0x42: return PSTR("Volume Down");
    case 0x43: return PSTR("Mute");
    case 0x44: return PSTR("Play");
    case 0x45: return PSTR("Stop");
    case 0x46: return PSTR("Pause");
    case 0x47: return PSTR("Record");
    case 0x48: return PSTR("Rewind");
    case 0x49: return PSTR("Fast Forward");
    case 0x4a: return PSTR("Eject");
    case 0x4b: return PSTR("Forward");
    case 0x4c: return PSTR("Backward");
    case 0x4d: return PSTR("Stop-Record");
    case 0x4e: return PSTR("Pause-Record");
    case 0x50: return PSTR("Angle");
    case 0x51: return PSTR("Sub picture");
    case 0x52: return PSTR("Video on Demand");
    case 0x53: return PSTR("Electronic Program Guide");
    case 0x54: return PSTR("Timer Programming");
    case 0x55: return PSTR("Initial Configuration");
    case 0x60: return PSTR("Play Function");
    case 0x61: return PSTR("Pause-Play Function");
    case 0x62: return PSTR("Record Function");
    case 0x63: return PSTR("Pause-Record Function");
    case 0x64: return PSTR("Stop Function");
    case 0x65: return PSTR("Mute Function");
    case 0x66: return PSTR("Restore Volume Function");
    case 0x67: return PSTR("Tune Function");
    case 0x68: return PSTR("Select Media Function");
    case 0x69: return PSTR("Select A/V Input Function");
    case 0x6a: return PSTR("Select Audio Input Function");
    case 0x6b: return PSTR("Power Toggle Function");
    case 0x6c: return PSTR("Power Off Function");
    case 0x6d: return PSTR("Power On Function");
    case 0x71: return PSTR("F1 (Blue)");
    case 0x72: return PSTR("F2 (Red)");
    case 0x73: return PSTR("F3 (Green)");
    case 0x74: return PSTR("F4 (Yellow)");
    case 0x75: return PSTR("F5");
    case 0x76: return PSTR("Data");
    default: return PSTR("???");
  }
}
char *broadcast_system(char c)
{
  switch (c)
  {
    case 0: return PSTR("PAL B/G");
    case 1: return PSTR("SECAM L'");
    case 2: return PSTR("PAL M");
    case 3: return PSTR("NTSC M");
    case 4: return PSTR("PAL I");
    case 5: return PSTR("SECAM DK");
    case 6: return PSTR("SECAM B/G");
    case 7: return PSTR("SECAM L");
    case 8: return PSTR("PAL DK");
    case 31: return PSTR("Other System");
    default: return PSTR("???");
  }
}
char *stringp(const char *pstr)
{
  static char buffer[32];
  strncpy_P(buffer, pstr, 31);
  buffer[31] = '\0'; // just in case!
  return buffer;
}  
void printp(const char *pstr)
{
  while(pgm_read_byte(pstr) != 0x00)
    Serial.print(pgm_read_byte(pstr++));
}
void print_physical_address(unsigned char *p)
{
  DbgPrint("%d.%d.%d.%d", p[0] >> 4, p[0] & 0x0F, p[1] >> 4, p[1] & 0x0F);
}
void print_buffer(unsigned char *buf, int len, int format = BYTE)
{
  for(int i = 0; i < len; i++)
  {
    Serial.print(buf[i], format);
    Serial.print(' ');
  }
}
void print_timer(unsigned char *buf)
{
  static const char *moy[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                              "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  static const char *dow[] = {"Once", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

  DbgPrint("%d %s %02d:%02d %02d:%02d ",
           buf[0], // Day of Month 
           moy[buf[1]], // Month of Year 
           buf[2], buf[3], // Start Time 
           buf[4], buf[5]); // Duration 
  if (buf[6]) // Recording Sequence 
  {
    Serial.print("every");
    for (unsigned char c = 0b0000001; c < 0b10000000; c = c << 1)
      if (buf[6] & c)
        DbgPrint(" %s", dow[c]);
  }
}
void cec_debug(unsigned char *buf, int len)
{
  if (len == 0)
  {
    Serial.print('<');
    printp(PSTR("Polling Message"));
    Serial.print('>');
    return;
  }

  Serial.print('<');
  printp(op_code(buf[0]));  
  Serial.print("> ");
    
  switch (buf[0])
  {
    case COP_ABORT:
    case COP_REQUEST_ACTIVE_SOURCE:
    case COP_GET_CEC_VERSION:
    case COP_GET_MENU_LANGUAGE:
    case COP_GIVE_AUDIO_STATUS:
    case COP_GIVE_DEVICE_POWER_STATUS:
    case COP_GIVE_DEVICE_VENDOR_ID:
    case COP_GIVE_OSD_NAME:
    case COP_GIVE_PHYSICAL_ADDRESS:
    case COP_GIVE_SYSTEM_AUDIO_MODE_STATUS:
    case COP_IMAGE_VIEW_ON:
    case COP_TEXT_VIEW_ON:
    case COP_RECORD_OFF:
    case COP_RECORD_TV_SCREEN:
    case COP_STANDBY:
    case COP_TUNER_STEP_DECREMENT:
    case COP_TUNER_STEP_INCREMENT:
    case COP_USER_CONTROL_RELEASED: 
    case COP_VENDOR_REMOTE_BUTTON_UP:
      // no parameters
      break;
    case COP_ACTIVE_SOURCE:
    case COP_INACTIVE_SOURCE:
    case COP_ROUTING_INFORMATION:
    case COP_SET_STREAM_PATH:
    case COP_SYSTEM_AUDIO_MODE_REQUEST:
      print_physical_address(buf+1);
      break;
    case COP_CEC_VERSION:
      printp(cec_version(buf[1]));           
      break;
    case COP_DECK_CONTROL:
      printp(deck_control_mode(buf[1]));
      break;
    case COP_DECK_STATUS:
      printp(deck_info(buf[1]));
      break;
    case COP_DEVICE_VENDOR_ID:
      print_buffer(buf+1, 3, HEX);
      break;
    case COP_FEATURE_ABORT:
      Serial.print('<');
      printp(op_code(buf[1])); // feature opcode
      Serial.print("> ");
      printp(abort_reason(buf[2])); // abort reason
      break;
    case COP_GIVE_DECK_STATUS:
    case COP_GIVE_TUNER_DEVICE_STATUS:
      printp(status_request(buf[1]));
      break;
    case COP_MENU_REQUEST:
      printp(menu_request_type(buf[1]));
      break;
    case COP_MENU_STATUS:
      printp(menu_state(buf[1]));
      break;
    case COP_PLAY:
      printp(play_mode(buf[1]));
      break;
    case COP_RECORD_STATUS:
      printp(record_status_info(buf[1]));
      break;
    case COP_REPORT_AUDIO_STATUS:
      printp(audio_mute_status(buf[1] == 0b10000000));
      Serial.print(" Volume ");
      if (buf[1] & 0b01111111 == 0x7f)
        Serial.print("unknown");
      else
      {
        Serial.print(buf[1] & 0b01111111, DEC); // 1..100
        Serial.print("%");
      }
      break;
    case COP_REPORT_PHYSICAL_ADDRESS: // todo: fix this one
      print_physical_address(buf+1);
      Serial.print(' ');
      printp(device_type(buf[3]));
      break;
    case COP_REPORT_POWER_STATUS:
      printp(power_status(buf[1]));
      break;
    case COP_ROUTING_CHANGE:
      print_physical_address(buf+1); // original address
      Serial.print(' ');
      print_physical_address(buf+3); // new address
      break;
    case COP_SET_AUDIO_RATE:
      printp(audio_rate(buf[1]));
      break;
    case COP_SET_MENU_LANGUAGE:
      print_buffer(buf+1, 3); // Language
      break;
    case COP_SET_OSD_STRING:
      printp(display_control(buf[1]));
      Serial.print(' ');
      print_buffer(buf+2, len-2); // osd string
      break;
    case COP_SET_OSD_NAME:
    case COP_SET_TIMER_PROGRAM_TITLE:
      print_buffer(buf+1, len-1); // string
      break;
    case COP_SET_SYSTEM_AUDIO_MODE:
    case COP_SYSTEM_AUDIO_MODE_STATUS:
      printp(system_audio_status(buf[1]));
      break;
    case COP_TIMER_CLEARED_STATUS:
      printp(timer_cleared_status_data(buf[1]));
      break;
    case COP_USER_CONTROL_PRESSED:
      printp(user_control_codes(buf[1]));
      Serial.print(' ');
      switch (buf[1])
      {
        case 0x60: // Play Function
          if (len == 3) printp(play_mode(buf[2]));
          break;
        case 0x67: // Tune Function
          if (len == 6) print_buffer(buf+2, 4, BIN); // channel Identifier
          break;
        case 0x68: // Select Media Function
          if (len == 3) Serial.print(buf[2], DEC); // UI Function Media 
          break;
        case 0x69: // Select A/V Input Function
          if (len == 3) Serial.print(buf[2], DEC); // UI Function Select A/V Input 
          break;
        case 0x6A: // Select Audio Input Function
          if (len == 3) Serial.print(buf[2], DEC); // UI Function Select Audio Input 
          break;
      }
      break;
    case COP_VENDOR_COMMAND:
      print_buffer(buf+1, len-1, HEX); // vendor specific data
      break;
    case COP_VENDOR_COMMAND_WITH_ID:
      print_buffer(buf+1, 3, HEX); // vendor id
      Serial.print(' ');
      print_buffer(buf+4, len-4, HEX); // vendor specific data
      break;
    case COP_VENDOR_REMOTE_BUTTON_DOWN:
      print_buffer(buf+1, len-1, HEX); // vendor specific rc code
      break;

    // the following are untested...
      
    case COP_CLEAR_ANALOGUE_TIMER:
    case COP_SET_ANALOGUE_TIMER:
      print_timer(buf+1);
      Serial.print(' ');
      printp(analogue_broadcast_type(buf[7]));
      DbgPrint(" 0x%02X%02X ", buf[8], buf[9]); // Analogue Frequency
      printp(broadcast_system(buf[10]));
      break;

    case COP_SELECT_ANALOGUE_SERVICE:
      printp(analogue_broadcast_type(buf[1]));
      DbgPrint(" 0x%02X%02X ", buf[2], buf[3]); // Analogue Frequency
      printp(broadcast_system(buf[4]));
      break;

    case COP_CLEAR_DIGITAL_TIMER:
    case COP_SET_DIGITAL_TIMER:
      print_timer(buf+1);
      Serial.print(' ');
      printp(service_identification_method(buf[7] & 0b10000000));
      Serial.print(' ');
      printp(digital_broadcast_system(buf[7] & 0b01111111));
      Serial.print(' ');
      print_buffer(buf+8, len-8, HEX);
      break;
      
    case COP_SELECT_DIGITAL_SERVICE:
      Serial.print(' ');
      printp(service_identification_method(buf[1] & 0b10000000));
      Serial.print(' ');
      printp(digital_broadcast_system(buf[1] & 0b01111111));
      Serial.print(' ');
      print_buffer(buf+2, len-2, HEX);
      break;
      
    case COP_SET_EXTERNAL_TIMER:
    case COP_CLEAR_EXTERNAL_TIMER:
      print_timer(buf+1);
      Serial.print(' ');
      printp(external_source_specifier(buf[7]));
      Serial.print(' ');
      print_buffer(buf+8, len-8, HEX); // External Plug OR External Physical Address
      break;

    // too hard basket

    case COP_RECORD_ON: // record source
    case COP_TIMER_STATUS: // timer Status Data
    case COP_TUNER_DEVICE_STATUS: // tuner device info
      print_buffer(buf+1, len-1, HEX);
      break;
    default: // unknown. print raw message
      print_buffer(buf, len, HEX);
      break;
  }
}

