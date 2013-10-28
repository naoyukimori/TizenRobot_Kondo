/*
 * KBT1Controller.h
 *
 *  Created on: Oct 27, 2013
 *      Author: Mori
 */

#ifndef KBT1CONTROLLER_H_
#define KBT1CONTROLLER_H_

#include <FApp.h>
#include <FBase.h>
//#include <FUi.h>
#include <FNet.h>

class KBT1Controller
	: public Tizen::Net::Bluetooth::IBluetoothManagerEventListener
	, public Tizen::Net::Bluetooth::IBluetoothSppInitiatorEventListener
{
		enum RobotCurrentState { ROBOT_STATE_IDLE,
		                    ROBOT_STATE_IN_MOTION,
		                    ROBOT_STATE_UNKNOWN };

public:
	KBT1Controller(void);
	virtual ~KBT1Controller(void);

	bool Initialize();			// initialize bluetooth managers
	bool IsConnected(void);		// true if bluetooth is activated
	result Connect();		// connect to KBT-1
	result Disconnect();	// disconnect from KBT-1
	result Ping();			// Send ping command to KBT-1
	result Move_Head();		// Move head servo motor
	result PlayMotion(int index);	/* Play embedded motion 1 to n */

	// from IBluetoothManagerEventListener
	virtual void OnBluetoothActivated(result r) {}
	virtual void OnBluetoothDeactivated(result r) {}

	// IBluetoothSppInitiatorEventListener
	virtual void OnSppConnectionResponded(result r);
	virtual void OnSppDataReceived(Tizen::Base::ByteBuffer &buffer);
	virtual void OnSppDisconnected(result r);

	// IBluetoothSppInitiatorEventListener
	virtual void OnSppConnectionRequested(const Tizen::Net::Bluetooth::BluetoothDevice &device);


private:
	bool __isBtKBTReady;		// true if bluetooth is activated
	bool __isSppConstructed;	// ture if spp stack ready
	bool __isBtKBTConnectionResponded;		// true if connection established
	Tizen::Net::Bluetooth::BluetoothManager __btManager;
	Tizen::Net::Bluetooth::BluetoothSppInitiator __sppInitiator;
	Tizen::Net::Bluetooth::BluetoothSppAcceptor __sppAcceptor;

	int robotState;		// unused
	byte RCB4_calc_checksum(byte *cmd, int len);
};



#endif /* KBT1CONTROLLER_H_ */
