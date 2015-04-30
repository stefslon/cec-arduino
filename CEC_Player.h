#ifndef CEC_PLAYER_H__
#define CEC_PLAYER_H__

#include "CEC_OpCodes.h"
#include "CEC_Debug.h"
#include "CEC_Device.h"

class CEC_Player: public CEC_Device
{
public:
  CEC_Player(int physicalAddress);
  virtual ~CEC_Player();
  
  void Transmit(int targetAddress, int count ...);

  bool debug;
  bool raw;
  
protected:
  virtual void OnReady();
  virtual void OnReceive(int source, int dest, unsigned char* buffer, int count);
  
private:
  int _physicalAddress;
  unsigned char *ptr;
};

#endif // CEC_PLAYER_H__
