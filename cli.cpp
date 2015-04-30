#include "cli.h"


extern CEC_Device device;

clibuffer_t clibuffer;

// char input
void
cliCharInput(){
  char c = Serial.read();
  if (c == 0x0D){
    clibuffer.complete = true;
    Serial.write("\r\n"); 
  } 
  else {
    if (isalnum(c) || c == SPACE){

      clibuffer.buffer[clibuffer.position++] = c;
      clibuffer.len++;
      Serial.write(c);
      if (clibuffer.len == CLIBUFLEN){
        Serial.write("\r\nmax input length reached\r\n");
        clibuffer.complete = true; 
      }
    }
    if (c == BACKSPACE && clibuffer.len > 0){
      Serial.write("\b \b");
      clibuffer.buffer[clibuffer.position--] = '\n';
      clibuffer.len--; 
    }

  }
}


void
cliProcessInput(){
  cliCharInput();

  if (clibuffer.complete){
    cliParseInput();
    clibuffer.len = 0;
    clibuffer.position = 0; 
    clibuffer.complete = false;
  }


}

void
cliParseInput(){
  char target;
  unsigned char cmdbuffer[16];
  char buffpos;
  boolean targetset = false;


  clibuffer.position = 0;
  clibuffer.buffer[clibuffer.len] = '\0';
  char *endptr;
  long val;

  char * tok = strtok(clibuffer.buffer, " ");
  while (tok != NULL && buffpos < clibuffer.len) {
    long toklen = strlen(tok);
    errno = 0;
    val = strtoul(tok, &endptr, 16);

    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) 
      || (errno !=0 && val == 0)) {
      Serial.write("error parsing byte values");
      return; 
    }
    if (!targetset){
      target = (char) val;
      targetset = true;
      //we've got the target, continue parsing 
    }
    else {
      cmdbuffer[buffpos++] = (char) val;
    }
    tok = strtok(NULL, " ");
  }
  //print packet

  DbgPrint("\r\nTarget: %X Packet: ", target);

  for (int i = 0; i < buffpos; i++){
    DbgPrint("%X ", cmdbuffer[i]);
  } 
  DbgPrint("\r\n");

  device.TransmitFrame(target, cmdbuffer, buffpos);
}

void
cliSwitchCase(unsigned char c) {
  unsigned char buffer[23];
  int target = 0;
  switch (c)
  {
  case '1':
    DbgPrint("<Standby0>\r\n");
    buffer[0] = 0x36;
    device.TransmitFrame(target, buffer, 1);
    break;
  case '2':
    DbgPrint("<Standby15>\r\n");
    buffer[0] = 0x36;
    device.TransmitFrame(15, buffer, 1);
    break;

  case '3':
    DbgPrint("Req Pwr State:\r\n");
    buffer[0] = 0x8F; 
    device.TransmitFrame(target, buffer, 1);
    break;

  case 'q':
    DbgPrint("<Tune step Up>\r\n");
    buffer[0] = 0x05;
    device.TransmitFrame(target, buffer, 1);
    break;
  case 'a':
    DbgPrint("<Tune step Dwn>\r\n");
    buffer[0] = 0x06; 
    device.TransmitFrame(target, buffer, 1);
    break;
  case 'z':
    DbgPrint("<Status Req>\r\n");
    buffer[0] = 0x83;
    device.TransmitFrame(target, buffer, 1);
    break;        
  case 'i':
    DbgPrint("<Img View On>\r\n");
    buffer[0] = 0x04;
    device.TransmitFrame(target, buffer, 1);
    break;        
  case 'c':
    DbgPrint("<CEC Ver Query>\r\n");
    buffer[0] = 0x9F; 
    device.TransmitFrame(target, buffer, 1);
    break;      
  case 'o':
    DbgPrint("<Query OSD Name>\r\n");
    buffer[0] = 0x46; 
    device.TransmitFrame(target, buffer, 1);
    break;   
  case 'f':
    DbgPrint("<Query Phys Adr>\r\n");
    buffer[0] = 0x83; 
    device.TransmitFrame(target, buffer, 1);
    break;   
  case 'r':
    DbgPrint("<Announce my Adr>\r\n");
    buffer[0] = 0x84; 
    buffer[1] = 0x30;
    buffer[2] = 0x00;
    buffer[3] = 0x04;
    device.TransmitFrame(target, buffer, 1);
    break;  
  case 'v':
    DbgPrint("<Query Vend ID>\r\n");
    buffer[0] = 0x8C;
    device.TransmitFrame(target, buffer, 1);
    break;  
  case 'w':
    DbgPrint("<Send OSD name>\r\n");
    buffer[0] = 0x47;
    memcpy(buffer+1, "Andrew-HTPC", 11);
    device.TransmitFrame(target, buffer, 12);
    break;         
  case 'p':
    DbgPrint("<Query Pwr Status>\r\n");
    buffer[0] = 0x8F;
    device.TransmitFrame(target, buffer, 1);
    break;          
  case 's':
    DbgPrint("<Remote pwr off>\r\n");
    buffer[0] = 0x44;
    buffer[0] = 0x6C;
    device.TransmitFrame(target, buffer, 2);
    buffer[0] = 0x45;
    delay(100);
    device.TransmitFrame(target, buffer, 1);
    break;                 
  default:
    DbgPrint("Keycode: %X\r\n", c);   
    break;
  }
}

