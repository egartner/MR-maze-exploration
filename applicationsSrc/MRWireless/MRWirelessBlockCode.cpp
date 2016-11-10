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
	//if (hostBlock->blockId==1){
		WirelessNetworkInterface *wireless = bb->getWirelessNetworkInterface();
		WirelessMessage *message = new WirelessMessage(255);
		getScheduler()->schedule(new WirelessNetworkInterfaceEnqueueOutgoingEvent(getScheduler()->now()+DELAY, message, wireless));
		info << " Wireless message scheduled in block" << hostBlock->blockId;
		getScheduler()->trace(info.str(),hostBlock->blockId);
	//} 
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
	WirelessMessagePtr message;
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
	}
	break;
	case EVENT_WNI_MESSAGE_RECEIVED: 
	{
		info << "Message received in block" << hostBlock->blockId;
		WirelessMessagePtr mes = (std::static_pointer_cast<WirelessNetworkInterfaceMessageReceivedEvent>(pev))->message;
		bb->setColor(BLUE);
		
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
