#include "CEC.h"

int CEC_Device::_validLogicalAddresses[6][5] = 
	{	{CLA_TV,					CLA_FREE_USE,			CLA_UNREGISTERED,		CLA_UNREGISTERED,	CLA_UNREGISTERED,	},
		{CLA_RECORDING_DEVICE_1,	CLA_RECORDING_DEVICE_2,	CLA_RECORDING_DEVICE_3,	CLA_UNREGISTERED,	CLA_UNREGISTERED,	},
		{CLA_PLAYBACK_DEVICE_1,		CLA_PLAYBACK_DEVICE_2,	CLA_PLAYBACK_DEVICE_3,	CLA_UNREGISTERED,	CLA_UNREGISTERED,	},
		{CLA_TUNER_1,				CLA_TUNER_2,			CLA_TUNER_3,			CLA_TUNER_4,		CLA_UNREGISTERED,	},
		{CLA_AUDIO_SYSTEM,			CLA_UNREGISTERED,		CLA_UNREGISTERED,		CLA_UNREGISTERED,	CLA_UNREGISTERED,	},
		{CLA_UNREGISTERED,			CLA_UNREGISTERED,		CLA_UNREGISTERED,		CLA_UNREGISTERED,	CLA_UNREGISTERED,	},
		};

#define MAKE_ADDRESS(s,d) ((((s) & 0xf) << 4) | ((d) & 0xf))

bool User_GetLineState();
void User_SetLineState(CEC_Device* device, bool state);
void User_OnReady();
//void User_OnReceive(int source, int dest, unsigned char* rxBuffer, int count);
void User_OnReceive(unsigned char* rxBuffer, int count);

CEC_Device::CEC_Device(int physicalAddress) 
{
	_transmitBufferCount = 0;
	_transmitBufferBit = 7;
	_transmitBufferByte = 0;
	_receiveBufferCount = 0;
	_receiveBufferBit = 7;
	_receiveBufferByte = 0;
	
	MonitorMode = false;
	Promiscuous = false;

	_address = CLA_UNREGISTERED & 0x0f;
	_amLastTransmittor = false;
	_transmitPending = false;
	_xmitretry = 0;
	ResetState();
	
	
	_physicalAddress 	= physicalAddress;
	_logicalAddress 	= CLA_UNREGISTERED;
	_done 				= false;
	_waitTime			= 0;
	_primaryState	 	= CEC_ALLOCATE_LOGICAL_ADDRESS;
	_deviceType			= CDT_OTHER;

	_secondaryState = CEC_XMIT_POLLING_MESSAGE;
	_tertiaryState = 0;
	
	
	_isrTriggered 	= false;
	_lastLineState2 = true;
}

void CEC_Device::ClearTransmitBuffer()
{
	_transmitBufferCount = 0;
	_transmitBufferBit = 7;
	_transmitBufferByte = 0;
}

bool CEC_Device::Transmit(unsigned char* buffer, int count)
{
	if (!TransmitPartial(buffer, count))
		return false;

	OnTransmitBegin();
	return true;
}

/**
 *  Copy contents of transmit buffer into private _transmitBuffer
 */
bool CEC_Device::TransmitPartial(unsigned char* buffer, int count)
{
	if (count < 0 || count >= (SERIAL_BUFFER_SIZE - _transmitBufferCount))
		return false;

	for (int i = 0; i < count; i++)
		_transmitBuffer[_transmitBufferCount + i] = buffer[i];
	_transmitBufferCount += count;
	_transmitBufferBit = 7;
	_transmitBufferByte = 0;
	return true;
}

/**
 *  Pop next transmit bit
 */
int CEC_Device::PopTransmitBit()
{
	if (_transmitBufferByte == _transmitBufferCount)
		return 0;

	int bit = (_transmitBuffer[_transmitBufferByte] >> _transmitBufferBit) & 1;
	if( _transmitBufferBit-- == 0 )
	{
		_transmitBufferBit = 7;
		_transmitBufferByte++;
	}
	return bit;
}

int CEC_Device::RemainingTransmitBytes()
{
	return _transmitBufferCount - _transmitBufferByte;
}

int CEC_Device::TransmitSize()
{
	return _transmitBufferCount;
}

void CEC_Device::PushReceiveBit(int bit)
{
	_receiveBuffer[_receiveBufferByte] &= ~(1 << _receiveBufferBit);
	_receiveBuffer[_receiveBufferByte] |= bit << _receiveBufferBit;
	if (_receiveBufferBit-- == 0)
	{
		_receiveBufferBit = 7;
		_receiveBufferByte++;
	}
}

int CEC_Device::ReceivedBytes()
{
	return _receiveBufferByte;
}

void CEC_Device::ResetReceiveBuffer()
{
	_receiveBufferBit = 7;
	_receiveBufferByte = 0;
}

void CEC_Device::ResetTransmitBuffer()
{
	_transmitBufferBit = 7;
	_transmitBufferByte = 0;
}


/******************************************************************************************/




void CEC_Device::Initialize()
{
	_lastLineState = LineState();
	_lastStateChangeTime = micros();
}

void CEC_Device::SetAddress(int address)
{
	_address = address & 0x0f;
}

bool CEC_Device::Raise()
{
	if (MonitorMode)
		return LineState();

	unsigned long time = micros();
	SetLineState(1);
	// Only update state if the line was actually changed (i.e. it wasn't already in its new state)
	if (LineState())
	{
		_lastLineState = true;
		_lastStateChangeTime = time;
		return true;
	}
	return false;
}

bool CEC_Device::Lower()
{
	if (MonitorMode)
		return LineState();

	unsigned long time = micros();
	SetLineState(0);
	// Only update state if the line was actually changed (i.e. it wasn't already in its new state)
	if (!LineState())
	{
		if (_lastLineState)
			_bitStartTime = time;
		_lastLineState = false;
		_lastStateChangeTime = time;
		return false;
	}
	return true;
}

void CEC_Device::HasRaised(unsigned long time)
{
	_lastLineState = true;
	_lastStateChangeTime = time;
}

void CEC_Device::HasLowered(unsigned long time)
{
	_lastLineState = false;
	_lastStateChangeTime = time;
	_bitStartTime = time;
}

bool CEC_Device::CheckAddress()
{
	if (ReceivedBytes() == 1)
	{
		int address = _receiveBuffer[0] & 0x0f;
		if (address != _address && address != 0x0f)
		{
			// It's not addressed to us and it's not a broadcast.
			// Reset and wait for the next start bit
			return false;
		}
		// It is either addressed to this device or its a broadcast
		if (address == 0x0f)
			_broadcast = true;
		else
			_follower = true;
	}
	return true;
}

void CEC_Device::ReceivedBit(bool state)
{
	if (_e_tertiaryState == CEC_E_RCV_BIT_EOM)
	{
		//DbgPrint("%p: Received eom %d\n", this, state?1:0);
		_eom = state;
	}
	else
	{
		//DbgPrint("%p: Received bit %d\n", this, state?1:0);
		if (state)
			PushReceiveBit(1);
		else
			PushReceiveBit(0);
	}
}

unsigned long CEC_Device::LineError()
{
        //DbgPrint("%p: Line Error!\n", this);
	if (_follower || _broadcast)
	{
		_e_secondaryState = CEC_E_RCV_LINEERROR;
		Lower();
		return micros() + 3600;
	}
	return ResetState() ? micros() : (unsigned long)-1;
}

///
/// CEC_E_Electrical::Process implements our main state machine
/// which includes all reading and writing of state including
/// acknowledgements and arbitration
///

unsigned long CEC_Device::Process()
{
	// We are either called because of an INTerrupt in which case
	// state has changed or because of a poll (during write).

	bool currentLineState = LineState();
	bool lastLineState = _lastLineState;
	unsigned long waitTime = -1;	// INFINITE; (== wait until an external event has occurred)
	unsigned long time = micros();

//DbgPrint("%d %d %d\n", _e_primaryState, _e_secondaryState, _e_tertiaryState);
	// If the state hasn't changed and we're not in a transmit
	// state then this is spurrious.
	if( currentLineState == lastLineState &&
		!(_e_primaryState == CEC_E_TRANSMIT ||
		  (_e_primaryState == CEC_E_RECEIVE &&
		   (_e_secondaryState == CEC_E_RCV_ACK_SENT ||
		    _e_secondaryState == CEC_E_RCV_LINEERROR))))
		return waitTime;

	unsigned long lasttime = _lastStateChangeTime;
	unsigned long difftime = time - _bitStartTime;

	if( currentLineState != lastLineState )
	{
		// Line state has changed, update our internal state
		if (currentLineState)
			HasRaised(time);
		else
			HasLowered(time);
	}

	switch (_e_primaryState)
	{
	case CEC_E_IDLE:
		// If a high to low transition occurs, then we must be
		// beginning a start bit
		if (lastLineState && !currentLineState)
		{
			_e_primaryState = CEC_E_RECEIVE;
			_e_secondaryState = CEC_E_RCV_STARTBIT1;
			_follower = false;
			_broadcast = false;
			_amLastTransmittor = false;
			break;
		}
		
		// Nothing to do until we have a need to transmit
		// or we detect the falling edge of the start bit
		break;

	case CEC_E_RECEIVE:
		switch (_e_secondaryState)
		{
		case CEC_E_RCV_STARTBIT1:
			// We're waiting for the rising edge of the start bit
			if (difftime >= 3500 && difftime <= 3900)
			{
				// We now need to wait for the next falling edge
				_e_secondaryState = CEC_E_RCV_STARTBIT2;
				break;
			}
			// Illegal state.  Go back to CEC_E_IDLE to wait for a valid
			// start bit
//DbgPrint("1: %ld %ld\n", difftime, micros());
			waitTime = ResetState() ? micros() : (unsigned long)-1;
			break;
		
		case CEC_E_RCV_STARTBIT2:
			// This should be the falling edge of the start bit			
			if (difftime >= 4300 && difftime <= 4700)
			{
				// We've fully received the start bit.  Begin receiving
				// a data bit
				_e_secondaryState = CEC_E_RCV_DATABIT1;
				_e_tertiaryState = CEC_E_RCV_BIT0;
				break;
			}
			// Illegal state.  Go back to CEC_E_IDLE to wait for a valid
			// start bit
//DbgPrint("2: %ld %ld\n", difftime, micros());
			waitTime = ResetState() ? micros() : (unsigned long)-1;
			break;
		
		case CEC_E_RCV_DATABIT1:
			// We've received the rising edge of the data bit
			if (difftime >= 400 && difftime <= 800)
			{
				// We're receiving bit '1'
				ReceivedBit(true);
				_e_secondaryState = CEC_E_RCV_DATABIT2;
				break;
			}
			else if (difftime >= 1300 && difftime <= 1700)
			{
				// We're receiving bit '0'
				ReceivedBit(false);
				_e_secondaryState = CEC_E_RCV_DATABIT2;
				break;
			}
			// Illegal state.  Go back to CEC_E_IDLE to wait for a valid
			// start bit
			waitTime = LineError();
			break;

		case CEC_E_RCV_DATABIT2:
			// We've received the falling edge of the data bit
			if (difftime >= 2050 && difftime <= 2750)
			{
				if (_e_tertiaryState == CEC_E_RCV_BIT_EOM)
				{
					_e_secondaryState = CEC_E_RCV_ACK1;
					_e_tertiaryState = (CEC_E_TERTIARY_STATE)(_e_tertiaryState + 1);

					// Check to see if the frame is addressed to us
					// or if we are in promiscuous mode (in which case we'll receive everything)
					if (!CheckAddress() && !Promiscuous)
					{
						// It's not addressed to us.  Reset and wait for the next start bit
                			        waitTime = ResetState() ? micros() : (unsigned long)-1;
						break;
					}

					// If we're the follower, go low for a while
					if (_follower)
					{
						Lower();

						_e_secondaryState = CEC_E_RCV_ACK_SENT;
						waitTime = _bitStartTime + 1500;
					}
					break;
				}
				// Receive another bit
				_e_secondaryState = CEC_E_RCV_DATABIT1;
				_e_tertiaryState = (CEC_E_TERTIARY_STATE)(_e_tertiaryState + 1);
				break;
			}
			// Illegal state.  Go back to CEC_E_IDLE to wait for a valid
			// start bit
			waitTime = LineError();
			break;

		case CEC_E_RCV_ACK_SENT:
			// We're done holding the line low...  release it
			Raise();
			if (_eom)
			{
				// We're not going to receive anything more from
				// the initiator (EOM has been received)
				// And we've sent the ACK for the most recent bit
				// therefore this message is all done.  Go back
				// to the IDLE state and wait for another start bit.
				ProcessFrame();
			        waitTime = ResetState() ? micros() : (unsigned long)-1;
				break;
			}
			// We need to wait for the falling edge of the ACK
			// to finish processing this ack
			_e_secondaryState = CEC_E_RCV_ACK2;
			_e_tertiaryState = CEC_E_ACK;
			break;

		// FIXME:  This is dead state
		// Code currently exists in CEC_E_RCV_DATABIT2 that checks the address
		// of the frame and if it isn't addressed to this device it goes back
		// to watching for a start bit state.  This state, CEC_E_RCV_ACK1, was
		// from when we didn't do that and we followed state for all frames
		// regardless of addressing.  However, I'm not removing this code because
		// it will be needed when we support broadcast frames.
		case CEC_E_RCV_ACK1:
			{
				int ack;
				if (difftime >= 400 && difftime <= 800)
					ack = _broadcast ? CEC_E_ACK : CEC_E_NAK;
				else if (difftime >= 1300 && difftime <= 1700)
					ack = _broadcast ? CEC_E_NAK : CEC_E_ACK;
				else
				{
					// Illegal state.  Go back to CEC_E_IDLE to wait for a valid
					// start bit
					waitTime = LineError();
					break;
				}

				if (_eom && ack == CEC_E_ACK)
				{
					// We're not going to receive anything more from
					// the initiator (EOM has been received)
					// And we've seen the ACK for the most recent bit
					// therefore this message is all done.  Go back
					// to the IDLE state and wait for another start bit.
					ProcessFrame();
			                waitTime = ResetState() ? micros() : (unsigned long)-1;
					break;
				}
				if (ack == CEC_E_NAK)
				{
      			                waitTime = ResetState() ? micros() : (unsigned long)-1;
					break;
				}

				// receive the rest of the ACK (or rather the beginning of the next bit)
				_e_secondaryState = CEC_E_RCV_ACK2;
				break;
			}

		case CEC_E_RCV_ACK2:
			// We're receiving the falling edge of the ack
			if (difftime >= 2050 && difftime <= 2750)
			{
				_e_secondaryState = CEC_E_RCV_DATABIT1;
				_e_tertiaryState = CEC_E_RCV_BIT0;
				break;
			}
			// Illegal state (or NACK).  Either way, go back to CEC_E_IDLE
			// to wait for next start bit (maybe a retransmit)..
			waitTime = LineError();
			break;

		case CEC_E_RCV_LINEERROR:
			//DbgPrint("%p: Done signaling line error\n", this);
			Raise();
			waitTime = ResetState() ? micros() : (unsigned long)-1;
			break;
		
		}

		break;

	case CEC_E_TRANSMIT:
		if (lastLineState != currentLineState)
		{
			// Someone else is mucking with the line.  Wait for the
			// line to clear before appropriately before (re)transmit

			// However it is OK for a follower to ACK if we are in an
			// ACK state
			if (_e_secondaryState != CEC_E_XMIT_ACK &&
				_e_secondaryState != CEC_E_XMIT_ACK2 &&
				_e_secondaryState != CEC_E_XMIT_ACK3 &&
				_e_secondaryState != CEC_E_XMIT_ACK_TEST)
			{
				// If a state changed TO LOW during IDLE wait, someone could be legitimately transmitting
				if (_e_secondaryState == CEC_E_IDLE_WAIT)
				{
					if (currentLineState == false)
					{
						_e_primaryState = CEC_E_RECEIVE;
						_e_secondaryState = CEC_E_RCV_STARTBIT1;
						_transmitPending = true;
					}
					break;
				}
				else
				{
					// Transmit collision
					ResetTransmit(true);
					waitTime = 0;
					break;
				}
			}
			else
			{
				// This is a state change from an ACK and isn't part of our state
				// tracking.
								//DbgPrint("Ack Received\r\n");
                                waitTime = -2;
				break;
			}
		}

		unsigned long neededIdleTime = 0;
		switch (_e_secondaryState)
		{
		case CEC_E_IDLE_WAIT:
			// We need to wait a certain amount of time before we can
			// transmit..

			// If the line is low, we can't do anything now.  Wait
			// indefinitely until a line state changes which will
			// catch in the code just above
			if (currentLineState == 0)
				break;

			// The line is high.  Have we waited long enough?
			neededIdleTime = 0;
			switch (_e_tertiaryState)
			{
			case CEC_E_IDLE_RETRANSMIT_FRAME:
				neededIdleTime = 3 * 2400;
				break;

			case CEC_E_IDLE_NEW_FRAME:
				neededIdleTime = 5 * 2400;
				break;

			case CEC_E_IDLE_SUBSEQUENT_FRAME:
				neededIdleTime = 7 * 2400;
				break;
			}

			if (time - _lastStateChangeTime < neededIdleTime)
			{
				// not waited long enough, wait some more!
				waitTime = lasttime + neededIdleTime;
				break;
			}

			// we've wait long enough, begin start bit
			Lower();
			_amLastTransmittor = true;
			_broadcast = (_transmitBuffer[0] & 0x0f) == 0x0f;

			// wait 3700 microsec
			waitTime = _bitStartTime + 3700;
			
			// and transition to our next state
			_e_secondaryState = CEC_E_XMIT_STARTBIT1;
			break;

		case CEC_E_XMIT_STARTBIT1:
			if (!Raise())
			{
				//DbgPrint("%p: Received Line Error\n", this);
				ResetTransmit(true);
				break;
			}

			waitTime = _bitStartTime + 4500;
			_e_secondaryState = CEC_E_XMIT_STARTBIT2;
			break;

		case CEC_E_XMIT_STARTBIT2:
		case CEC_E_XMIT_ACK3:
			Lower();

			_e_secondaryState = CEC_E_XMIT_DATABIT1;
			_e_tertiaryState = CEC_E_XMIT_BIT0;

			if (PopTransmitBit())
			{
				// Sending bit '1'
				//DbgPrint("%p: Sending bit 1\n", this);
				waitTime = _bitStartTime + 600;
			}
			else
			{
				// Sending bit '0'
				//DbgPrint("%p: Sending bit 0\n", this);
				waitTime = _bitStartTime + 1500;
			}
			break;

		case CEC_E_XMIT_DATABIT1:
			if (!Raise())
			{
				//DbgPrint("%p: Received Line Error\n", this);
				ResetTransmit(true);
				break;
			}

			waitTime = _bitStartTime + 2400;

			if (_e_tertiaryState == CEC_E_XMIT_BIT_EOM)
			{
				// We've just finished transmitting the EOM
				// move on to the ACK
				_e_secondaryState = CEC_E_XMIT_ACK;
			}			
			else
				_e_secondaryState = CEC_E_XMIT_DATABIT2;
			break;

		case CEC_E_XMIT_DATABIT2:
			Lower();

			_e_tertiaryState = (CEC_E_TERTIARY_STATE)(_e_tertiaryState + 1);

			if (_e_tertiaryState == CEC_E_XMIT_BIT_EOM)
			{
				if (RemainingTransmitBytes() == 0)
				{
					// Sending eom '1'
					//DbgPrint("%p: Sending eom 1\n", this);
					waitTime = _bitStartTime + 600;
				}
				else
				{
					// Sending eom '0'
					//DbgPrint("%p: Sending eom 0\n", this);
					waitTime = _bitStartTime + 1500;
				}
			}
			else
			{
				if (PopTransmitBit())
				{
					// Sending bit '1'
					//DbgPrint("%p: Sending bit 1\n", this);
					waitTime = _bitStartTime + 600;
				}
				else
				{
					// Sending bit '0'
					//DbgPrint("%p: Sending bit 0\n", this);
					waitTime = _bitStartTime + 1500;
				}
			}
			_e_secondaryState = CEC_E_XMIT_DATABIT1;
			break;

		case CEC_E_XMIT_ACK:
			Lower();

			// We transmit a '1'
			//DbgPrint("%p: Sending ack\n", this);
			waitTime = _bitStartTime + 600;
			_e_secondaryState = CEC_E_XMIT_ACK2;
			break;

		case CEC_E_XMIT_ACK2:
			Raise();

			// we need to sample the state in a little bit
			waitTime = _bitStartTime + 1050;
			_e_secondaryState = CEC_E_XMIT_ACK_TEST;
			break;

		case CEC_E_XMIT_ACK_TEST:
			//DbgPrint("%p: Testing ack: %d\n", this, (currentLineState == 0) != _broadcast?1:0);
			if ((currentLineState != 0) != _broadcast)
			{
				// not being acknowledged
				// normally we retransmit.  But this is NOT the case for <Polling Message> as its
				// function is basically to 'ping' a logical address in which case we just want 
				// acknowledgement that it has succeeded or failed
				if (RemainingTransmitBytes() == 0 &&  TransmitSize() == 1)
				{
					ResetState();
					OnTransmitComplete(false);
				}
				else
				{
					ResetTransmit(true);
					waitTime = 0;
				}
				break;
			}

			_lastStateChangeTime = lasttime;

			if (RemainingTransmitBytes() == 0)
			{
				// Nothing left to transmit, go back to idle
				ResetState();
				OnTransmitComplete(true);
				break;
			}

			// We have more to transmit, so do so...
			waitTime = _bitStartTime + 2400;
			_e_secondaryState = CEC_E_XMIT_ACK3;
			break;
		}
	}
	return waitTime;	
}

bool CEC_Device::ResetState()
{
	_e_primaryState = CEC_E_IDLE;
	_e_secondaryState = (CEC_E_SECONDARY_STATE)0;
	_e_tertiaryState = (CEC_E_TERTIARY_STATE)0;
	_eom = false;
	_follower = false;
	_broadcast = false;
	ResetReceiveBuffer();

	if (_transmitPending)
        {
		ResetTransmit(false);
                return true;
        }
        return false;
}

void CEC_Device::ResetTransmit(bool retransmit)
{
	_e_primaryState = CEC_E_TRANSMIT;
	_e_secondaryState = CEC_E_IDLE_WAIT;
	_e_tertiaryState = CEC_E_IDLE_NEW_FRAME;
	_transmitPending = false;

	if (retransmit)
	{
		if (++_xmitretry == CEC_E_MAX_RETRANSMIT)
		{
			// No more
			ResetState();
			OnTransmitComplete(false);
		}
		else
		{
			//DbgPrint("%p: Retransmitting current frame\n", this);
			_e_tertiaryState = CEC_E_IDLE_RETRANSMIT_FRAME;
			ResetTransmitBuffer();
		}
	}
	else 
	{
		_xmitretry = 0;
		if (_amLastTransmittor)
		{
			_e_tertiaryState = CEC_E_IDLE_SUBSEQUENT_FRAME;
		}
	}
}

void CEC_Device::ProcessFrame()
{
	// We've successfully received a frame in the serial line buffer
	// Allow it to be processed
	OnReceiveComplete(_receiveBuffer, _receiveBufferByte);
}

void CEC_Device::OnTransmitBegin()
{
	if (!MonitorMode)
	{
		if (_e_primaryState == CEC_E_IDLE)
		{
			ResetTransmit(false);
			return;
		}
		_transmitPending = true;
	}
}




/******************************************************************************/



void CEC_Device::Initialize(CEC_DEVICE_TYPE type)
{
	_lastLineState = LineState();
	_lastStateChangeTime = micros();
	
	_deviceType = type;

        if (MonitorMode)
        {
            _primaryState = CEC_READY;
        }
}

bool CEC_Device::ProcessStateMachine(bool* success)
{
	unsigned char buffer[1];
	bool wait = false;

	switch (_primaryState)
	{
	case CEC_ALLOCATE_LOGICAL_ADDRESS:
		switch (_secondaryState)
		{
		case CEC_XMIT_POLLING_MESSAGE:
			// Section 6.1.3 specifies that <Polling Message> while allocating a Logical Address
			// will have the same initiator and destination address
			buffer[0] = MAKE_ADDRESS(_validLogicalAddresses[_deviceType][_tertiaryState], _validLogicalAddresses[_deviceType][_tertiaryState]);
			ClearTransmitBuffer();
			Transmit(buffer, 1);
			
			_secondaryState = CEC_RCV_POLLING_MESSAGE;
			wait = true;
			break;

		case CEC_RCV_POLLING_MESSAGE:
			if (success)
			{
				if (*success)
				{
					// Someone is there, try the next
					_tertiaryState++;
					if (_validLogicalAddresses[_deviceType][_tertiaryState] != CLA_UNREGISTERED)
						_secondaryState = CEC_XMIT_POLLING_MESSAGE;
					else
					{
						_logicalAddress = CLA_UNREGISTERED;
						//DbgPrint("Logical address assigned: %d\r\n", _logicalAddress);
						//DbgPrint("Physical addresss used: %d\r\n", _physicalAddress);
						_primaryState = CEC_READY;
					}
				}
				else
				{
					// We hereby claim this as our logical address!
					_logicalAddress = _validLogicalAddresses[_deviceType][_tertiaryState];
					SetAddress(_logicalAddress);
					//DbgPrint("Logical address assigned: %d\r\n", _logicalAddress);
					//DbgPrint("Physical addresss used: %d\r\n", _physicalAddress);
					_primaryState = CEC_READY;
				}
			}
			else
				wait = true;
			break;
		}
		break;

	case CEC_READY:
		_primaryState = CEC_IDLE;
		OnReady();
		wait = true;
		break;

	case CEC_IDLE:
		wait = true;
		break;
	}

	return wait;
}

void CEC_Device::OnReceiveComplete(unsigned char* buffer, int count)
{
  ResetReceiveBuffer();
  
	ASSERT(count >= 1);
	//int sourceAddress = (buffer[0] >> 4) & 0x0f;
	//int targetAddress = buffer[0] & 0x0f;
	//OnReceive(sourceAddress, targetAddress, buffer + 1, count - 1);
  User_OnReceive(buffer, count);
}

bool CEC_Device::TransmitFrame(int targetAddress, unsigned char* buffer, int count)
{
	if (_primaryState != CEC_IDLE)
		return false;

	unsigned char addr[1];

	addr[0] = MAKE_ADDRESS(_logicalAddress, targetAddress);
        ClearTransmitBuffer();
	if (!TransmitPartial(addr, 1))
		return false;
	return Transmit(buffer, count);
}

void CEC_Device::OnTransmitComplete(bool success)
{
	if (_primaryState == CEC_ALLOCATE_LOGICAL_ADDRESS &&
		_secondaryState == CEC_RCV_POLLING_MESSAGE &&
		_logicalAddress == CLA_UNREGISTERED)
	{
		while (!ProcessStateMachine(&success))
			;
	}
       // else
          //DbgPrint("Transmit: %d\n\n", success);
}

void CEC_Device::Run()
{
	bool state = User_GetLineState();
	if (_lastLineState2 != state)
	{
		_lastLineState2 = state;
		SignalIRQ();
	}
  
	// Initial pump for the state machine (this will cause a transmit to occur)
	while (!ProcessStateMachine(NULL))
		;

	if (((_waitTime == (unsigned long)-1 && !TransmitPending()) || (_waitTime != (unsigned long)-1 && _waitTime > micros())) && !IsISRTriggered())
		return;

        unsigned long wait = Process();
        if (wait != (unsigned long)-2)
	        _waitTime = wait;
	return;
}

void CEC_Device::OnReady()
{
  // This is called after the logical address has been
  // allocated
  User_OnReady();
}

//extern "C" unsigned long millis();
/*void CEC_Device::OnReceive(int source, int dest, unsigned char* buffer, int count)
{
  // This is called when a frame is received.  To transmit
  // a frame call TransmitFrame.  To receive all frames, even
  // those not addressed to this device, set Promiscuous to true.
  User_OnReceive(source, dest, buffer, count);
}*/

bool CEC_Device::LineState()
{
  return User_GetLineState();
}

void CEC_Device::SetLineState(bool state)
{
  User_SetLineState(this, state);
}

void CEC_Device::SignalIRQ()
{
  // This is called when the line has changed state
  _isrTriggered = true;
}

bool CEC_Device::IsISRTriggered()
{
  if (_isrTriggered)
  {
    _isrTriggered = false;
    return true;
  }
  return false;
}

