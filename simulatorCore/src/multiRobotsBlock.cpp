/*
 * @file multiRobotsBlock.cpp
 *
 *  Created on: 14/07/2016
 *      Author: pthalamy
 */

#include <iostream>

#include "multiRobotsBlock.h"

#include "multiRobotsWorld.h"
#include "multiRobotsSimulator.h"
#include "translationEvents.h"
#include "meldInterpretEvents.h"
#include "trace.h"
#include "clock.h"
#include "network.h"

using namespace std;

namespace MultiRobots {

MultiRobotsBlock::MultiRobotsBlock(int bId, BlockCodeBuilder bcb)
	: BaseSimulator::BuildingBlock(bId, bcb, BCLattice::MAX_NB_NEIGHBORS) {
    OUTPUT << "MultiRobotsBlock constructor" << endl;

    for( int i=0 ; i<BCLattice::MAX_NB_NEIGHBORS ; i++)
	P2PNetworkInterfaces.pop_back();

    for( int i =0 ; i<BCLattice::MAX_NB_NEIGHBORS ; i++)
	P2PNetworkInterfaces.push_back(new WirelessNetworkInterface(this,2));
}

MultiRobotsBlock::~MultiRobotsBlock() {
    OUTPUT << "MultiRobotsBlock destructor " << blockId << endl;
}

int MultiRobotsBlock::getDirection(P2PNetworkInterface *given_interface) {
    return BCLattice::Direction(0); // NONE
}

void MultiRobotsBlock::stopBlock(Time date, State s) {
    OUTPUT << "Simulator: stop scheduled" << endl;
    setState(s);
    if (s == STOPPED) {
		// patch en attendant l'objet 3D qui modelise un MR stopped
		color = Color(0.1, 0.1, 0.1, 0.5);
    }

	getWorld()->updateGlData(this);

	if (BaseSimulator::Simulator::getType() == BaseSimulator::Simulator::MELDINTERPRET) {
		getScheduler()->schedule(new MeldInterpret::VMStopEvent(getScheduler()->now(), this));
    }
}

void MultiRobotsBlock::setRange (int dist){
	for(int i=0 ; i<BCLattice::MAX_NB_NEIGHBORS ; i++){
		P2PNetworkInterfaces[i]->setRange(dist);
	}
}

std::ostream& operator<<(std::ostream &stream, MultiRobotsBlock const& bb) {
    stream << bb.blockId << "\tcolor: " << bb.color;
    return stream;
}

}
