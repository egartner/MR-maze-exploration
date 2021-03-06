/*
 * BbCycleBlockCode.cpp
 *
 *  Created on: 26 mars 2013
 *      Author: dom
 */

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <memory>

#include "scheduler.h"
#include "network.h"
#include "bbCycleBlockCode.h"
#include "bbCycleEvents.h"
#include "trace.h"
#include "lattice.h"

using namespace std;
using namespace BlinkyBlocks;


//#define ENABLE_SYNCHRONIZATION

#define SYNC_PERIOD (2*1000*1000)
#define COLOR_CHANGE_PERIOD_USEC (2*1000*1000)

BbCycleBlockCode::BbCycleBlockCode(BlinkyBlocksBlock *host): BlinkyBlocksBlockCode(host) {
  OUTPUT << "BbCycleBlockCode constructor" << endl;
  cout << "BbCycleBlockCode constructor" << endl;
}

BbCycleBlockCode::~BbCycleBlockCode() {
  OUTPUT << "BbCycleBlockCode destructor" << endl;
}

void BbCycleBlockCode::init() {
  BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
  stringstream info;	
  Color c = PINK;
  block2Answer=NULL;
  cycle=true;

  #ifdef ENABLE_SYNCHRONIZATION
  if(hostBlock->blockId==1){
    idMessage=0;	
    getScheduler()->schedule(new SynchronizeEvent(getScheduler()->now()+SYNC_PERIOD,hostBlock));	
    info << "This block is the Master Block" << endl;
  }
  #endif
  getScheduler()->schedule(new SetColorEvent(COLOR_CHANGE_PERIOD_USEC,bb,c));
  getScheduler()->trace(info.str(),hostBlock->blockId);
}

void BbCycleBlockCode::startup() {
  stringstream info;
  delay=0;
  info << "  Starting BbCycleBlockCode in block " << hostBlock->blockId;
  init();
}

void BbCycleBlockCode::processLocalEvent(EventPtr pev) {
  stringstream info;
  MessagePtr message;
  BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
  info.str("");
	
  OUTPUT << bb->blockId << " processLocalEvent: date: "<< BaseSimulator::getScheduler()->now() << " process event " << pev->getEventName() << "(" << pev->eventType << ")" << ", random number : " << pev->randomNumber << endl;
	
  switch (pev->eventType) {
  case EVENT_SET_COLOR:
    {
      Color color = (std::static_pointer_cast<SetColorEvent>(pev))->color;
      bb->setColor(color);
      info << "set color "<< color << endl;
      if (cycle){
	color = BLUE;
	cycle = false;
      }
      else{
	color = RED;
	cycle = true;
      }
      getScheduler()->schedule(new SetColorEvent(bb->getLocalTime()+COLOR_CHANGE_PERIOD_USEC+delay,bb,color));
      info << "Setcolor scheduled" << endl;
    }
    break;
    //case EVENT_PLAY_NOTE:
  case EVENT_NI_RECEIVE:
    {
      message = (std::static_pointer_cast<NetworkInterfaceReceiveEvent>(pev))->message; 
      P2PNetworkInterface * recvInterface = message->destinationInterface;
      switch(message->id){
      case SYNC_MSG_ID: 
	{
	  SynchroMessage_ptr recvMessage =  std::static_pointer_cast<SynchroMessage>(message);
	  if (!received[recvMessage->idSync]){
	    received[recvMessage->idSync]=true;
	    block2Answer=recvInterface;
	    sendClockToNeighbors(block2Answer,recvMessage->nbhop+1,recvMessage->time,recvMessage->idSync); 	
	    delay = recvMessage->time - bb->getLocalTime() + 6000*recvMessage->nbhop;  
	    info<<"synchronized with delay : "<< delay << endl;
	  }
	}
	break;					
      default:
	break;
      }
    }
    break;
  case EVENT_SYNC:
    {
      received[idMessage]=true;
      sendClockToNeighbors(NULL,1,bb->getLocalTime(),idMessage);
      idMessage++;
      Time nextSync = bb->getLocalTime()+SYNC_PERIOD;
      getScheduler()->schedule(new SynchronizeEvent(nextSync,bb));
      info << "scheduled synchro" << endl;
    }
    break;
  case EVENT_TAP: {
    int face = (std::static_pointer_cast<TapEvent>(pev))->tappedFace;
    info << "tapped on face " << lattice->getDirectionString(face);
  } break;
  default:
    ERRPUT << "*** ERROR *** : unknown local event" << endl;
    break;
  }
  getScheduler()->trace(info.str(),hostBlock->blockId);
}

BlockCode* BbCycleBlockCode::buildNewBlockCode(BuildingBlock *host) {
  return(new BbCycleBlockCode((BlinkyBlocksBlock*)host));
}

void BbCycleBlockCode::sendClockToNeighbors (P2PNetworkInterface *p2pExcept, int hop, Time clock, int id){
  P2PNetworkInterface * p2p;
  BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	
  for (int i=0; i<6 ; i++) {
    p2p = bb->getInterface(SCLattice::Direction(i));
    if (p2p->connectedInterface && p2p!=p2pExcept){	
      SynchroMessage *message = new SynchroMessage(clock, hop, id);
      getScheduler()->schedule(new NetworkInterfaceEnqueueOutgoingEvent (getScheduler()->now(), message, p2p));
    }
  }
}

SynchroMessage::SynchroMessage(Time t, int hop, int ids) :Message(){
  id = SYNC_MSG_ID;
  idSync=ids;
  time = t;
  nbhop = hop;
}

SynchroMessage::~SynchroMessage(){
}
