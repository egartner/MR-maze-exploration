#include <iostream>
#include <sstream>
#include <memory>
#include <list>

#include "scheduler.h"
#include "network.h"
#include "lattice.h"

#include "MRWirelessBlockCode.h"

#include "trace.h"

#define DELAY (2*1000*1000)

using namespace std;
using namespace MultiRobots;

MRWirelessBlockCode::MRWirelessBlockCode(MultiRobotsBlock *host): MultiRobotsBlockCode(host) {
	OUTPUT << "MRWirelessBlockCode constructor" << endl;
	currentLocalDate = 0; // mode fastest
	bb = (MultiRobotsBlock*)hostBlock;
}

MRWirelessBlockCode::~MRWirelessBlockCode() {
	OUTPUT << "MRWirelessBlockCode destructor" << endl;
}

void MRWirelessBlockCode::init() {
	stringstream info;
	bb->setColor(GREEN);
	if (hostBlock->blockId==1){
		WirelessNetworkInterface *wireless = bb->getWirelessNetworkInterface();
		WirelessMessage *message = new WirelessMessage();
		getScheduler()->schedule(new WirelessNetworkInterfaceEnqueueOutgoingEvent(getScheduler()->now()+DELAY, message, wireless);
	} 
}

void MRWirelessBlockCode::startup() {
	stringstream info;
	currentLocalDate = BaseSimulator::getScheduler()->now();
	info << "  Starting MRWirelessBlockCode in block " << hostBlock->blockId;
	getScheduler()->trace(info.str(),hostBlock->blockId);
	init();
}

void MRWirelessBlockCode::processLocalEvent(EventPtr pev) {
	stringstream info;
	MessagePtr message;
	info.str("");
#ifdef TEST_DETER
	cout << bb->blockId << " processLocalEvent: date: "<< BaseSimulator::getScheduler()->now() << " process event " << pev->getEventName() << "(" << pev->eventType << ")" << ", random number : " << pev->randomNumber << endl;
#endif
	switch (pev->eventType) {
	case EVENT_SET_COLOR:
	{
		Color color = (std::static_pointer_cast<SetColorEvent>(pev))->color;
		bb->setColor(color);
#ifdef TEST_DETER
		cout << bb->blockId << " SET_COLOR_EVENT" << endl;
#endif
		info << "set color "<< color;
	}
	break;

	case EVENT_SEND_MESSAGE:
	{
		message = new Message();
	}
	break;
	case EVENT_WNI_MESSAGE_RECEIVED: 
	{
		MessagePtr mes = (std::static_pointer_cast<WirelessNetworkInterfaceMessageReceivedEvent>(pev))->message;
		WirelessNetworkInterface * rcvInterface = message->destinationInterface;
		bb->setColor(BLUE);	
#ifdef TEST_DETER
		cout << "message received from " << command->sourceInterface->hostBlock->blockId << endl;
#endif
	}
	break;
	default:
		ERRPUT << "*** ERROR *** : unknown local event";
		break;
	}
	if(info.str() != "") {
		getScheduler()->trace(info.str(),hostBlock->blockId);
	}
}

BlockCode* MRWirelessBlockCode::buildNewBlockCode(BuildingBlock *host) {
	return(new MRWirelessBlockCode((MultiRobotsBlock*)host));
}
