#include <iostream>
#include <sstream>
#include <memory>
#include <list>

#include "scheduler.h"
#include "network.h"
#include "lattice.h"

#include "MRMazeBlockCode.h"

#include "trace.h"

#define DELAY (2*1000*1000)

using namespace std;
using namespace MultiRobots;

MRMazeBlockCode::MRMazeBlockCode(MultiRobotsBlock *host): MultiRobotsBlockCode(host) {
	OUTPUT << "MRMazeBlockCode constructor" << endl;
	currentLocalDate = 0; // mode fastest
	bb = (MultiRobotsBlock*)hostBlock;
}

MRMazeBlockCode::~MRMazeBlockCode() {
	OUTPUT << "MRMazeBlockCode destructor" << endl;
}

void MRMazeBlockCode::init() {
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

void MRMazeBlockCode::startup() {
	stringstream info;
	currentLocalDate = BaseSimulator::getScheduler()->now();
	info << "  Starting MRMazeBlockCode in block " << hostBlock->blockId;
	getScheduler()->trace(info.str(),hostBlock->blockId);
	init();
}

void MRMazeBlockCode::processLocalEvent(EventPtr pev) {
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

BlockCode* MRMazeBlockCode::buildNewBlockCode(BuildingBlock *host) {
	return(new MRMazeBlockCode((MultiRobotsBlock*)host));
}
