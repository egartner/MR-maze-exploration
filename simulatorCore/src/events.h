/*
 * events.h
 *
 *  Created on: 26 mars 2013
 *      Author: dom
 */

#ifndef EVENTS_H_
#define EVENTS_H_

//class BuildingBlock;

#include <inttypes.h>
#include <string>
#include "buildingBlock.h"
#include "uniqueEventsId.h"
#include "network.h"
#include "color.h"
#include "tDefs.h"
#include "random.h"

using namespace std;

class Event;

typedef std::shared_ptr<Event> EventPtr;

#ifdef DEBUG_EVENTS
#define EVENT_CONSTRUCTOR_INFO()			(OUTPUT << getEventName() << " constructor (" << id << ")" << endl)
#define EVENT_DESTRUCTOR_INFO()				(OUTPUT << getEventName() << " destructor (" << id << ")" << endl)
#else
#define EVENT_CONSTRUCTOR_INFO()
#define EVENT_DESTRUCTOR_INFO()
#endif

#ifdef DEBUG_CONSUME_EVENTS
#define EVENT_CONSUME_INFO()				({ stringstream debuginfo1; debuginfo1 << "consuming event " << id << " (" << getEventName() << ")"; Scheduler::trace(debuginfo1.str());})
#else
#define EVENT_CONSUME_INFO()
#endif

//===========================================================================================================
//
//          Event  (class)
//
//===========================================================================================================

class Event {
protected:
	static int nextId;
	static unsigned int nbLivingEvents;

public:
	int id;				//!< unique ID of the event (mainly for debugging purpose)
	Time date;		//!< time at which the event will be processed. 0 means simulation start
	int eventType;		//!< see the various types at the beginning of this file
	BaseSimulator::ruint randomNumber;

	Event(Time t);
	Event(Event *ev);
	virtual ~Event();

	virtual void consume() = 0;
	virtual const string getEventName();

	static unsigned int getNextId();
	static unsigned int getNbLivingEvents();
	virtual BaseSimulator::BuildingBlock* getConcernedBlock() { return NULL; };
};

//===========================================================================================================
//
//          BlockEvent  (class)
//
//===========================================================================================================

class BlockEvent : public Event {

protected:
	BaseSimulator::BuildingBlock *concernedBlock;
	BlockEvent(Time t, BaseSimulator::BuildingBlock *conBlock);
	BlockEvent(BlockEvent *ev);
	virtual ~BlockEvent();
	virtual const string getEventName();

public:
	BaseSimulator::BuildingBlock* getConcernedBlock() {return concernedBlock;};
	virtual void consumeBlockEvent() = 0;
	virtual void consume() {
		if (concernedBlock->getState() >= BaseSimulator::BuildingBlock::ALIVE) {
			this->consumeBlockEvent();
		}
	};
};

//===========================================================================================================
//
//          CodeStartEvent  (class)
//
//===========================================================================================================

class CodeStartEvent : public BlockEvent {
public:

	CodeStartEvent(Time, BaseSimulator::BuildingBlock *conBlock);
	~CodeStartEvent();
	void consumeBlockEvent();
	const virtual string getEventName();
};

//===========================================================================================================
//
//          CodeEndSimulationEvent  (class)
//
//===========================================================================================================

class CodeEndSimulationEvent : public Event {
public:

	CodeEndSimulationEvent(Time);
	~CodeEndSimulationEvent();
	void consume();
	const virtual string getEventName();
};

//===========================================================================================================
//
//          ProcessLocalEvent  (class)
//
//===========================================================================================================

class ProcessLocalEvent : public BlockEvent {
public:

	ProcessLocalEvent(Time, BaseSimulator::BuildingBlock *conBlock);
	~ProcessLocalEvent();
	void consumeBlockEvent();
	const virtual string getEventName();
};

//===========================================================================================================
//
//          NetworkInterfaceStartTransmittingEvent  (class)
//
//===========================================================================================================

class NetworkInterfaceStartTransmittingEvent : public Event {
public:
	//NetworkInterface *interface;
    P2PNetworkInterface *interface;

	//NetworkInterfaceStartTransmittingEvent(Time, NetworkInterface *ni);
    NetworkInterfaceStartTransmittingEvent(Time, P2PNetworkInterface *ni);
	~NetworkInterfaceStartTransmittingEvent();
	void consume();
	const virtual string getEventName();
};

//===========================================================================================================
//
//          WirelessNetworkInterfaceStartTransmittingEvent  (class)
//
//===========================================================================================================

class WirelessNetworkInterfaceStartTransmittingEvent : public Event {
public:
    //NetworkInterface *interface;
    WirelessNetworkInterface *interface;
    
    //NetworkInterfaceStartTransmittingEvent(Time, NetworkInterface *ni);
    WirelessNetworkInterfaceStartTransmittingEvent(Time, WirelessNetworkInterface *ni);
    ~WirelessNetworkInterfaceStartTransmittingEvent();
    void consume();
    const virtual string getEventName();
};

//===========================================================================================================
//
//          NetworkInterfaceStopTransmittingEvent  (class)
//
//===========================================================================================================

class NetworkInterfaceStopTransmittingEvent : public Event {
public:
    //NetworkInterface *interface;
    P2PNetworkInterface *interface;
    
    //NetworkInterfaceStopTransmittingEvent(Time, NetworkInterface *ni);
    NetworkInterfaceStopTransmittingEvent(Time, P2PNetworkInterface *ni);
    ~NetworkInterfaceStopTransmittingEvent();
    void consume();
    const virtual string getEventName();
};

//===========================================================================================================
//
//          WirelessNetworkInterfaceStopTransmittingEvent  (class)
//
//===========================================================================================================

class WirelessNetworkInterfaceStopTransmittingEvent : public Event {
public:
	//NetworkInterface *interface;
	WirelessNetworkInterface *interface;

	//NetworkInterfaceStopTransmittingEvent(Time, NetworkInterface *ni);
    WirelessNetworkInterfaceStopTransmittingEvent(Time, WirelessNetworkInterface *ni);
	~WirelessNetworkInterfaceStopTransmittingEvent();
	void consume();
	const virtual string getEventName();
};

//===========================================================================================================
//
//          NetworkInterfaceReceiveEvent  (class)
//
//===========================================================================================================

class NetworkInterfaceReceiveEvent : public Event {
public:
	P2PNetworkInterface *interface;
	MessagePtr message;
    //NetworkInterfaceReceiveEvent(Time,NetworkInterface *ni, MessagePtr mes);
	NetworkInterfaceReceiveEvent(Time,P2PNetworkInterface *ni, MessagePtr mes);
	~NetworkInterfaceReceiveEvent();
	void consume();
	const virtual string getEventName();
};

//===========================================================================================================
//
//          WirelessNetworkInterfaceStartReceiveEvent  (class)
//
//===========================================================================================================

class WirelessNetworkInterfaceStartReceiveEvent : public Event {
public:
    WirelessNetworkInterface *interface;
    WirelessMessagePtr message;
    //NetworkInterfaceReceiveEvent(Time,NetworkInterface *ni, MessagePtr mes);
    WirelessNetworkInterfaceStartReceiveEvent(Time,WirelessNetworkInterface *ni, WirelessMessagePtr mes);
    ~WirelessNetworkInterfaceStartReceiveEvent();
    void consume();
    const virtual string getEventName();
};

//===========================================================================================================
//
//          WirelessNetworkInterfaceStopReceiveEvent  (class)
//
//===========================================================================================================

class WirelessNetworkInterfaceStopReceiveEvent : public Event {
public:
    WirelessNetworkInterface *interface;
    //NetworkInterfaceReceiveEvent(Time,NetworkInterface *ni, MessagePtr mes);
    WirelessNetworkInterfaceStopReceiveEvent(Time t,WirelessNetworkInterface *ni);
    ~WirelessNetworkInterfaceStopReceiveEvent();
    void consume();
    const virtual string getEventName();
};

//===========================================================================================================
//
//          WirelessNetworkInterfaceMessageReceivedEvent  (class)
//
//===========================================================================================================

class WirelessNetworkInterfaceMessageReceivedEvent : public Event {
public:
    WirelessNetworkInterface *interface;
    WirelessMessagePtr message;
    //NetworkInterfaceReceiveEvent(Time,NetworkInterface *ni, MessagePtr mes);
    WirelessNetworkInterfaceMessageReceivedEvent(Time,WirelessNetworkInterface *ni, WirelessMessagePtr mes);
    ~WirelessNetworkInterfaceMessageReceivedEvent();
    void consume();
    const virtual string getEventName();
};

//===========================================================================================================
//
//          NetworkInterfaceEnqueueOutgoingEvent  (class)
//
//===========================================================================================================

class NetworkInterfaceEnqueueOutgoingEvent : public Event {
public:
	MessagePtr message;
	P2PNetworkInterface *sourceInterface;

    //NetworkInterfaceEnqueueOutgoingEvent(Time, Message *mes, NetworkInterface *ni);
    //NetworkInterfaceEnqueueOutgoingEvent(Time, MessagePtr mes, NetworkInterface *ni);

	NetworkInterfaceEnqueueOutgoingEvent(Time, Message *mes, P2PNetworkInterface *ni);
	NetworkInterfaceEnqueueOutgoingEvent(Time, MessagePtr mes, P2PNetworkInterface *ni);
	~NetworkInterfaceEnqueueOutgoingEvent();
	void consume();
	const virtual string getEventName();
};

//===========================================================================================================
//
//          WirelessNetworkInterfaceEnqueueOutgoingEvent  (class)
//
//===========================================================================================================

class WirelessNetworkInterfaceEnqueueOutgoingEvent : public Event {
private:
    unsigned int destinationId;
public:
    WirelessMessagePtr message;
    WirelessNetworkInterface *sourceInterface;
    
    //NetworkInterfaceEnqueueOutgoingEvent(Time, Message *mes, NetworkInterface *ni);
    //NetworkInterfaceEnqueueOutgoingEvent(Time, MessagePtr mes, NetworkInterface *ni);
    
    WirelessNetworkInterfaceEnqueueOutgoingEvent(Time, WirelessMessage *mes, WirelessNetworkInterface *ni);
    WirelessNetworkInterfaceEnqueueOutgoingEvent(Time, WirelessMessagePtr mes, WirelessNetworkInterface *ni);
    ~WirelessNetworkInterfaceEnqueueOutgoingEvent();
    void consume();
    const virtual string getEventName();
};

//===========================================================================================================
//
//          WirelessNetworkInterfaceChannelListeningEvent  (class)
//
//===========================================================================================================

class WirelessNetworkInterfaceChannelListeningEvent : public Event {
public:
    WirelessNetworkInterface *interface;
    
    WirelessNetworkInterfaceChannelListeningEvent(Time, WirelessNetworkInterface *ni);
    ~WirelessNetworkInterfaceChannelListeningEvent();
    void consume();
    const virtual string getEventName();
};

//===========================================================================================================
//
//          WirelessNetworkInterfaceIdleEvent  (class)
//
//===========================================================================================================

class WirelessNetworkInterfaceIdleEvent : public Event {
public:
    WirelessNetworkInterface *interface;
    
    WirelessNetworkInterfaceIdleEvent(Time, WirelessNetworkInterface *ni);
    ~WirelessNetworkInterfaceIdleEvent();
    void consume();
    const virtual string getEventName();
};

//===========================================================================================================
//
//          SetColorEvent  (class)
//
//===========================================================================================================

class SetColorEvent : public BlockEvent {
public:
	Color color;

	SetColorEvent(Time, BaseSimulator::BuildingBlock *conBlock, float r, float g, float b, float a);
	SetColorEvent(Time, BaseSimulator::BuildingBlock *conBlock, Color &c);
	SetColorEvent(SetColorEvent *ev);
	~SetColorEvent();
	void consumeBlockEvent();
	const virtual string getEventName();
};


//===========================================================================================================
//
//          AddNeighborEvent  (class)
//
//===========================================================================================================

class AddNeighborEvent : public BlockEvent {
public:
	uint64_t face;
	uint64_t target;

	AddNeighborEvent(Time, BaseSimulator::BuildingBlock *conBlock, uint64_t f, uint64_t ta);
	AddNeighborEvent(AddNeighborEvent *ev);
	~AddNeighborEvent();
	void consumeBlockEvent();
	const virtual string getEventName();
};

//===========================================================================================================
//
//          RemoveNeighborEvent  (class)
//
//===========================================================================================================

class RemoveNeighborEvent : public BlockEvent {
public:
	uint64_t face;

	RemoveNeighborEvent(Time, BaseSimulator::BuildingBlock *conBlock, uint64_t f);
	RemoveNeighborEvent(RemoveNeighborEvent *ev);
	~RemoveNeighborEvent();
	void consumeBlockEvent();
	const virtual string getEventName();
};

//===========================================================================================================
//
//          TapEvent  (class)
//
//===========================================================================================================

class TapEvent : public BlockEvent {
public:
	const int tappedFace;

	TapEvent(Time, BaseSimulator::BuildingBlock *conBlock, const int face);
	TapEvent(TapEvent *ev);
	~TapEvent();
	void consumeBlockEvent();
	const virtual string getEventName();
};


//===========================================================================================================
//
//          AccelEvent  (class)
//
//===========================================================================================================

class AccelEvent : public BlockEvent {
public:
	uint64_t x;
	uint64_t y;
	uint64_t z;

	AccelEvent(Time, BaseSimulator::BuildingBlock *conBlock, uint64_t xx, uint64_t yy, uint64_t zz);
	AccelEvent(AccelEvent *ev);
	~AccelEvent();
	void consumeBlockEvent();
	const virtual string getEventName();
};

//===========================================================================================================
//
//          ShakeEvent  (class)
//
//===========================================================================================================

class ShakeEvent : public BlockEvent {
public:
	uint64_t force;

	ShakeEvent(Time, BaseSimulator::BuildingBlock *conBlock, uint64_t f);
	ShakeEvent(ShakeEvent *ev);
	~ShakeEvent();
	void consumeBlockEvent();
	const virtual string getEventName();
};



#endif /* EVENTS_H_ */
