/*
 * network.cpp
 *
 *  Created on: 24 mars 2013
 *      Author: dom
 */

#include <iostream>
#include <sstream>
#include <cmath>

#include "scheduler.h"
#include "network.h"
#include "trace.h"
#include "statsIndividual.h"
#include "utils.h"
#include "world.h"


using namespace std;
using namespace BaseSimulator;
using namespace BaseSimulator::utils;

//unsigned int Message::nextId = 0;
//unsigned int Message::nbMessages = 0;
uint64_t Message::nextId = 0;
uint64_t Message::nbMessages = 0;
uint64_t WirelessMessage::nextId = 0;
uint64_t WirelessMessage::nbMessages = 0;


unsigned int NetworkInterface::nextId = 0;
int NetworkInterface::defaultDataRate = 1000000;
unsigned int P2PNetworkInterface::nextId = 0;
int P2PNetworkInterface::defaultDataRate = 1000000;
unsigned int WirelessNetworkInterface::nextId = 0;
int WirelessNetworkInterface::defaultDataRate = 1000000;


//===========================================================================================================
//
//          Message  (class)
//
//===========================================================================================================

Message::Message() {
	id = nextId;
	nextId++;
	nbMessages++;
	MESSAGE_CONSTRUCTOR_INFO();
}

Message::~Message() {
	MESSAGE_DESTRUCTOR_INFO();
	nbMessages--;
}

uint64_t Message::getNbMessages() {
	return(nbMessages);
}

string Message::getMessageName() {
	return("generic message");
}

Message* Message::clone() {
    Message* ptr = new Message();
    ptr->sourceInterface = sourceInterface;
    ptr->destinationInterface = destinationInterface;
    ptr->type = type;
    return ptr;
}

//===========================================================================================================
//
//          WirelessMessage  (class)
//
//===========================================================================================================

WirelessMessage::WirelessMessage(bID destId) {
    id = nextId;
    destinationId = destId;
    nextId++;
    nbMessages++;
    MESSAGE_CONSTRUCTOR_INFO();
}

WirelessMessage::~WirelessMessage() {
    MESSAGE_DESTRUCTOR_INFO();
    nbMessages--;
}

uint64_t WirelessMessage::getNbMessages() {
    return(nbMessages);
}

string WirelessMessage::getMessageName() {
    return("generic wireless message");
}

WirelessMessage* WirelessMessage::clone() {
    WirelessMessage* ptr = new WirelessMessage(destinationId);
    ptr->sourceInterface = sourceInterface;
    //ptr->destinationInterface = destinationInterface;
    ptr->type = type;
    return ptr;
}

//==========================================================================================================
//
//	    NetworkInterface  (class)
//
//==========================================================================================================
NetworkInterface::NetworkInterface(BaseSimulator::BuildingBlock *b){
	hostBlock = b;
	availabilityDate=0;
	globalId=nextId;
	nextId++;
	dataRate = new StaticRate(defaultDataRate);
	connectedInterface = NULL;	
}

NetworkInterface::~NetworkInterface() {
}

//===========================================================================================================
//
//          P2PNetworkInterface  (class)
//
//===========================================================================================================

//P2PNetworkInterface::P2PNetworkInterface(BaseSimulator::BuildingBlock *b):NetworkInterface(b) {
P2PNetworkInterface::P2PNetworkInterface(BaseSimulator::BuildingBlock *b) {
#ifndef NDEBUG
	OUTPUT << "P2PNetworkInterface constructor" << endl;
#endif
    hostBlock = b;
    availabilityDate=0;
    globalId=nextId;
    nextId++;
    dataRate = new StaticRate(defaultDataRate);
    connectedInterface = NULL;
}

void P2PNetworkInterface::setDataRate(Rate *r) {
  assert(r != NULL);
  delete dataRate;
  dataRate = r;
}

P2PNetworkInterface::~P2PNetworkInterface() {
#ifndef NDEBUG
	OUTPUT << "P2PNetworkInterface destructor" << endl;
#endif
	delete dataRate;
}

void P2PNetworkInterface::send(Message *m) {
  getScheduler()->schedule(new NetworkInterfaceEnqueueOutgoingEvent(getScheduler()->now(), m, this));
}

bool P2PNetworkInterface::addToOutgoingBuffer(MessagePtr msg) {
	stringstream info;

	if (connectedInterface != NULL) {
		outgoingQueue.push_back(msg);
		BaseSimulator::utils::StatsIndividual::incOutgoingMessageQueueSize(hostBlock->stats);
		if (availabilityDate < BaseSimulator::getScheduler()->now()) availabilityDate = BaseSimulator::getScheduler()->now();
		if (outgoingQueue.size() == 1 && messageBeingTransmitted == NULL) { //TODO
			BaseSimulator::getScheduler()->schedule(new NetworkInterfaceStartTransmittingEvent(availabilityDate,this));
		}
		return(true);
	} else {
		info.str("");
		info << "*** WARNING *** [block " << hostBlock->blockId << ",interface " << globalId <<"] : trying to enqueue a Message but no interface connected";
		BaseSimulator::getScheduler()->trace(info.str());
		return(false);
	}
}

void P2PNetworkInterface::send() {
	MessagePtr msg;
	stringstream info;
	Time transmissionDuration;

	if (!connectedInterface) {
		info << "*** WARNING *** [block " << hostBlock->blockId << ",interface " << globalId <<"] : trying to send a Message but no interface connected";
		BaseSimulator::getScheduler()->trace(info.str());
		return;
	}

	if (outgoingQueue.size()==0) {
		info << "*** ERROR *** [block " << hostBlock->blockId << ",interface " << globalId <<"] : The outgoing buffer of this interface should not be empty !";
		BaseSimulator::getScheduler()->trace(info.str());
		exit(EXIT_FAILURE);
	}

	msg = outgoingQueue.front();
	outgoingQueue.pop_front();

	BaseSimulator::utils::StatsIndividual::decOutgoingMessageQueueSize(hostBlock->stats);
	
	transmissionDuration = getTransmissionDuration(msg);
	messageBeingTransmitted = msg;
	messageBeingTransmitted->sourceInterface = this;
	messageBeingTransmitted->destinationInterface = connectedInterface;

	availabilityDate = BaseSimulator::getScheduler()->now()+transmissionDuration;
/*	info << "*** sending (interface " << localId << " of block " << hostBlock->blockId << ")";
	getScheduler()->trace(info.str());*/

	BaseSimulator::getScheduler()->schedule(new NetworkInterfaceStopTransmittingEvent(BaseSimulator::getScheduler()->now()+transmissionDuration, this));
	
	StatsCollector::getInstance().incMsgCount();
	StatsIndividual::incSentMessageCount(hostBlock->stats);
}

void P2PNetworkInterface::connect(P2PNetworkInterface *ni) {
	// test ajouté par Ben, gestion du cas : connect(NULL)
	if (ni) { // Connection
		if (ni->connectedInterface != this) {
			if (ni->connectedInterface != NULL) {
				OUTPUT << "ERROR : connecting to an already connected P2PNetwork interface" << endl;
				ni->connectedInterface->hostBlock->removeNeighbor(ni->connectedInterface);
				ni->hostBlock->removeNeighbor(ni);
			}
			ni->connectedInterface = this;
			hostBlock->addNeighbor(ni->connectedInterface, ni->hostBlock);
			ni->hostBlock->addNeighbor(ni, ni->connectedInterface->hostBlock);
		}
	} else if (connectedInterface != NULL) {
		// disconnect this interface and the remote one
		hostBlock->removeNeighbor(this);
		connectedInterface->hostBlock->removeNeighbor(connectedInterface);
		connectedInterface->connectedInterface = NULL;
	}
	connectedInterface = ni;
}

Time P2PNetworkInterface::getTransmissionDuration(MessagePtr &m) {
  double rate = dataRate->get();
  Time transmissionDuration = (m->size()*8000000ULL)/rate;
  //cerr << "TransmissionDuration: " << transmissionDuration << endl;
  return transmissionDuration;
}

bool P2PNetworkInterface::isConnected() {
  return connectedInterface != NULL;
}

//======================================================================================================
//
//		WirelessNetworkInterface(class)
//
//======================================================================================================

WirelessNetworkInterface::WirelessNetworkInterface(BaseSimulator::BuildingBlock *b, float power) {
#ifndef NDEBUG
    OUTPUT << "WirelessNetworkInterface constructor" << endl;
#endif
    transmitPower = power;
    hostBlock = b;
    availabilityDate=0;
    globalId=nextId;
    nextId++;
    dataRate = new StaticRate(defaultDataRate);
    // arbitrary value, please adjust to fit your simulated radio equipment
    receptionThreshold = 10;
    collisionOccuring = false;
}

WirelessNetworkInterface::~WirelessNetworkInterface(){
#ifndef NDEBUG
	OUTPUT<< "WirelessNetworkInterface destructor" << endl;
#endif
}


void WirelessNetworkInterface::setReceptionThreshold(float threshold) {
    receptionThreshold = threshold;
}
float WirelessNetworkInterface::getReceptionThreshold() {
    return(receptionThreshold);
}

// Effectively start the transmission
// Do not call this function directly, it is called automatically when the previous transmission in the outgoing queue terminates
void WirelessNetworkInterface::send(){
    // ici il va falloir faire une boucle pour envoyer à toutes les cartes réseau des environs
    // Il faut récupérer la map de l'objet World
    
    // ATTENTION, pour l'instant l'interface fonctionne en full duplex ... elle peut émettre en même temps qu'elle reçoit
    // C'EST PHYSIQUEMENT IMPOSSIBLE
    // Il va falloir gérer cela ...
    
    
    
    WirelessMessagePtr msg;
    stringstream info;
    Time transmissionDuration;
    
    if (outgoingQueue.size()==0) {
        info << "*** ERROR *** [block " << hostBlock->blockId << ",wireless interface " << globalId <<"] : The outgoing buffer of this interface should not be empty !";
        BaseSimulator::getScheduler()->trace(info.str());
        exit(EXIT_FAILURE);
    }
    
    msg = outgoingQueue.front();
    outgoingQueue.pop_front();

    transmissionDuration = getTransmissionDuration(msg);
    messageBeingTransmitted = msg;
    messageBeingTransmitted->sourceInterface = this;
    
    BaseSimulator::getWorld()->broadcastWirelessMessage(msg);
    
    availabilityDate = BaseSimulator::getScheduler()->now()+transmissionDuration;
    /*	info << "*** sending (interface " << localId << " of block " << hostBlock->blockId << ")";
     getScheduler()->trace(info.str());*/
    
    BaseSimulator::getScheduler()->schedule(new WirelessNetworkInterfaceStopTransmittingEvent(BaseSimulator::getScheduler()->now()+transmissionDuration, this));
}

void WirelessNetworkInterface::startReceive(WirelessMessagePtr msg) {
    // ici il faut calculer l'atténuation en fonction de la distance et choisir si le paquet sera compris ou non
    // Si il est compris, il faut regarder si il nous est destiné
    // Si il nous est destiné, on commence la réception
    // Pour cela, on stocke une référence sur le message et on schedule un WirelessNetworkInterfaceStopReceiveEvent
    // Si une autre transmission se présente avant la fin de la réception, ça fera une collision
    
    double distance;
    Vector3D vec1;
    Vector3D vec2;
    vec1=msg->sourceInterface->hostBlock->getPositionVector();
    vec2=this->hostBlock->getPositionVector();
    distance = sqrt(pow(abs(vec1.pt[0] - vec2.pt[0]),2)+pow(abs(vec1.pt[1] - vec2.pt[1]),2));
    
    float pathLoss = 0;
    // ici il faut calculer une atténuation en fonction de la distance.
    // Par exemple on peut utiliser un modèle hybride tworayground / Friss
    // c.f. la section tworayground dans https://www.nsnam.org/docs/release/3.25/models/html/propagation.html
    
    float shadowing = 0;
    // ceci ajoute de la variabilité dans le temps à l'atténuation
    // ce terme doit être distribué de façon gaussienne
    // habituellement on prend un écart type de 7, mais celà dépend de l'environnement.
    // il faudra que ce soit paramétrable
    // c.f. http://www.isi.edu/nsnam/ns/doc/node220.html
    
    
    // la puissance reçue est la puissance émise moins l'atténuation liée à la distance et moins la variabilité.
    float receivedPower = msg->sourceInterface->getTransmitPower()-pathLoss-shadowing;
    
    if (receivedPower > receptionThreshold) {
        // le paquet est reçu avec une puissance suffisante pour être compris
        // on commence donc à le recevoir ...
        
        // mais si au cours de la réception un autre paquet se présente avec une puissance suffisante, il brouillera
        // celui en cours de réception.
        // il va falloir gérer ces cas
        messageBeingReceived = msg;
        
        // TODO
        // la durée de transmission doit dépendre de la taille du message et du débit
        Time transmissionDuration = 10;
        BaseSimulator::getScheduler()->schedule(new WirelessNetworkInterfaceStopReceiveEvent(BaseSimulator::getScheduler()->now()+transmissionDuration, this));
    }
    
    // [...]
}

void WirelessNetworkInterface::stopReceive() {
    // la transmission du message se termine
    // il faut regarder si une collision a eu lieu au cours de sa transmission
    //
    // si il n'y a pas eu collision il faut le passer à la couche supérieure (scheduleLocalEvent)
    
    if (!collisionOccuring) {
        this->hostBlock->scheduleLocalEvent(EventPtr(new WirelessNetworkInterfaceMessageReceivedEvent(BaseSimulator::getScheduler()->now(),this,messageBeingReceived)));
    }
    
}

void WirelessNetworkInterface::setTransmitPower(int power){
	this->transmitPower = power;
}

float WirelessNetworkInterface::getTransmitPower(){
    return(this->transmitPower);
}


Time WirelessNetworkInterface::getTransmissionDuration(WirelessMessagePtr &m) {
    double rate = dataRate->get();
    Time transmissionDuration = (m->size()*8000000ULL)/rate;
    //cerr << "TransmissionDuration: " << transmissionDuration << endl;
    return transmissionDuration;
}

bool WirelessNetworkInterface::addToOutgoingBuffer(WirelessMessagePtr msg) {
    stringstream info;
  
    outgoingQueue.push_back(msg);
    //BaseSimulator::utils::StatsIndividual::incOutgoingMessageQueueSize(hostBlock->stats);
    if (availabilityDate < BaseSimulator::getScheduler()->now()) availabilityDate = BaseSimulator::getScheduler()->now();
    if (outgoingQueue.size() == 1 && messageBeingTransmitted == NULL) {
        //
        // Scheduling this event instead of directly calling send() allows for taking into account processing time
        //
        BaseSimulator::getScheduler()->schedule(new WirelessNetworkInterfaceStartTransmittingEvent(availabilityDate,this));
    }
    // as long as there is no limit to the buffer size, this function will return true
    return(true);
}
