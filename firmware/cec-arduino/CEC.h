#ifndef CEC_H__
#define CEC_H__

extern "C"
{
  extern unsigned long micros();
  extern void delayMicroseconds(unsigned int);
}

#define ASSERT(x) ((void)0)
#define NULL 0

#define SERIAL_BUFFER_SIZE 16
#define CEC_E_MAX_RETRANSMIT 5

class CEC_Device
{
public:
	CEC_Device(int physicalAddress);

	// From SerialLine
	void ClearTransmitBuffer();
	bool Transmit(unsigned char* buffer, int count);
	bool TransmitPartial(unsigned char* buffer, int count);
	
	// From CEC_Electrical
	void Initialize();
	void SetAddress(int address);

	unsigned long Process();
	bool TransmitPending() { return _primaryState == CEC_E_TRANSMIT && _secondaryState == CEC_E_IDLE_WAIT; }

	int Promiscuous;
	int MonitorMode;
	
	// From CEC_LogicalDevice
	typedef enum {
		CDT_TV,
		CDT_RECORDING_DEVICE,
		CDT_PLAYBACK_DEVICE,
		CDT_TUNER,
		CDT_AUDIO_SYSTEM,
		CDT_OTHER,				// Not a real CEC type..
	} CEC_DEVICE_TYPE;

	void Initialize(CEC_DEVICE_TYPE type);

	void Run();
	bool TransmitFrame(int targetAddress, unsigned char* buffer, int count);
	
	// From CEC_Device
	//void Run();
	

protected:
	// From SerialLine
	unsigned char _transmitBuffer[SERIAL_BUFFER_SIZE];
	unsigned char _receiveBuffer[SERIAL_BUFFER_SIZE];
	int _transmitBufferCount;
	int _transmitBufferBit;
	int _transmitBufferByte;
	int _receiveBufferCount;
	int _receiveBufferBit;
	int _receiveBufferByte;

	//virtual void OnTransmitBegin() {;}
	void OnReceiveComplete(unsigned char* buffer, int count);

	int PopTransmitBit();
	int RemainingTransmitBytes();
	int TransmitSize();
	void ResetTransmitBuffer();

	void PushReceiveBit(int);
	int ReceivedBytes();
	void ResetReceiveBuffer();
	
	// From CEC_Electrical
	//virtual bool LineState() = 0;
	//virtual void SetLineState(bool) = 0;
	
	// From CEC_LogicalDevice
	//virtual bool IsISRTriggered() = 0;

	bool ProcessStateMachine(bool* success);

	//void OnReceiveComplete(unsigned char* buffer, int count);
	void OnTransmitComplete(bool);

	//virtual void OnReady() {;}
	//virtual void OnReceive(int sourceAddress, int targetAddress, unsigned char* buffer, int count) = 0;
	
	// From CEC_Device
	bool LineState();
	void SetLineState(bool);
	void SignalIRQ();
	bool IsISRTriggered();
	//virtual bool IsISRTriggered2() { return _isrTriggered; }

	void OnReady();
	//void OnReceive(int source, int dest, unsigned char* buffer, int count);

private:
	// From SerialLine
	
	// From CEC_Electrical
	typedef enum {
		CEC_E_IDLE,
		CEC_E_TRANSMIT,
		CEC_E_RECEIVE,
	} CEC_E_PRIMARY_STATE;

	typedef enum {
		CEC_E_RCV_STARTBIT1,
		CEC_E_RCV_STARTBIT2,
		CEC_E_RCV_DATABIT1,
		CEC_E_RCV_DATABIT2,
		CEC_E_RCV_ACK_SENT,
		CEC_E_RCV_ACK1,
		CEC_E_RCV_ACK2,
		CEC_E_RCV_LINEERROR,

		CEC_E_IDLE_WAIT,
		CEC_E_XMIT_STARTBIT1,
		CEC_E_XMIT_STARTBIT2,
		CEC_E_XMIT_DATABIT1,
		CEC_E_XMIT_DATABIT2,
		CEC_E_XMIT_ACK,
		CEC_E_XMIT_ACK2,
		CEC_E_XMIT_ACK3,
		CEC_E_XMIT_ACK_TEST,
	} CEC_E_SECONDARY_STATE;

	typedef enum {
		CEC_E_RCV_BIT0,
		CEC_E_RCV_BIT1,
		CEC_E_RCV_BIT2,
		CEC_E_RCV_BIT3,
		CEC_E_RCV_BIT4,
		CEC_E_RCV_BIT5,
		CEC_E_RCV_BIT6,
		CEC_E_RCV_BIT7,
		CEC_E_RCV_BIT_EOM,
		CEC_E_RCV_BIT_ACK,

		CEC_E_ACK,
		CEC_E_NAK,

		CEC_E_XMIT_BIT0,
		CEC_E_XMIT_BIT1,
		CEC_E_XMIT_BIT2,
		CEC_E_XMIT_BIT3,
		CEC_E_XMIT_BIT4,
		CEC_E_XMIT_BIT5,
		CEC_E_XMIT_BIT6,
		CEC_E_XMIT_BIT7,
		CEC_E_XMIT_BIT_EOM,
		CEC_E_XMIT_BIT_ACK,


		CEC_E_IDLE_RETRANSMIT_FRAME,
		CEC_E_IDLE_NEW_FRAME,
		CEC_E_IDLE_SUBSEQUENT_FRAME,
	} CEC_E_TERTIARY_STATE;

	
	bool ResetState();
	void ResetTransmit(bool retransmit);
	void OnTransmitBegin();
	//virtual void OnTransmitComplete(bool) {;}

	void ProcessFrame();

	// Helper functions
	bool Raise();
	bool Lower();
	void HasRaised(unsigned long);
	void HasLowered(unsigned long);
	bool CheckAddress();
	void ReceivedBit(bool);
	unsigned long LineError();

	int _address;

	bool _lastLineState;
	unsigned long _lastStateChangeTime;
	unsigned long _bitStartTime;

	int _xmitretry;

	bool _eom;
	bool _follower;
	bool _broadcast;
	bool _amLastTransmittor;
	bool _transmitPending;

	CEC_E_PRIMARY_STATE _e_primaryState;
	CEC_E_SECONDARY_STATE _e_secondaryState;
	CEC_E_TERTIARY_STATE _e_tertiaryState;
	int _tertiaryState;
	
	// From CEC_LogicalDevice
	typedef enum {
		CLA_TV = 0,
		CLA_RECORDING_DEVICE_1,
		CLA_RECORDING_DEVICE_2,
		CLA_TUNER_1,
		CLA_PLAYBACK_DEVICE_1,
		CLA_AUDIO_SYSTEM,
		CLA_TUNER_2,
		CLA_TUNER_3,
		CLA_PLAYBACK_DEVICE_2,
		CLA_RECORDING_DEVICE_3,
		CLA_TUNER_4,
		CLA_PLAYBACK_DEVICE_3,
		CLA_RESERVED_1,
		CLA_RESERVED_2,
		CLA_FREE_USE,
		CLA_UNREGISTERED,
	} CEC_LOGICAL_ADDRESS;

	typedef enum {
		CEC_IDLE,
		CEC_READY,
		CEC_ALLOCATE_LOGICAL_ADDRESS,
	} CEC_PRIMARY_STATE;

	typedef enum {
		CEC_XMIT_POLLING_MESSAGE,
		CEC_RCV_POLLING_MESSAGE,
	} CEC_SECONDARY_STATE;

	typedef enum {
	} CEC_TERTIARY_STATE;

	static int _validLogicalAddresses[6][5];
	int _physicalAddress;
	int _logicalAddress;
	bool _done;
	unsigned long _waitTime;

	CEC_PRIMARY_STATE _primaryState;
	CEC_DEVICE_TYPE _deviceType;
	CEC_SECONDARY_STATE _secondaryState;
	//int _tertiaryState;
	
	// From CEC_Device
	friend void User_SetLineState(CEC_Device* device, bool state);
	friend bool User_GetLineState();
	bool _isrTriggered;
	bool _lastLineState2;

  // Custom
  friend void User_OnReady();
  //friend void User_OnReceive(int source, int dest, unsigned char* rxBuffer, int count);
  friend void User_OnReceive(unsigned char* rxBuffer, int count);
  
};


#endif // CEC_H__
