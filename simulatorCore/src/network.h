/*
 * network.h
 *
 *  Created on: 24 mars 2013
 *      Author: dom
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <deque>
#include <string.h>

#include "tDefs.h"
#include "rate.h"
#include "buildingBlock.h"

using namespace std;

class Message;
class NetworkInterface;
class P2PNetworkInterface;
class WirelessNetworkInterface;

typedef std::shared_ptr<Message> MessagePtr;

#ifdef DEBUG_MESSAGES
#define MESSAGE_CONSTRUCTOR_INFO()			(cout << getMessageName() << " constructor (" << id << ")" << endl)
#define MESSAGE_DESTRUCTOR_INFO()			(cout << getMessageName() << " destructor (" << id << ")" << endl)
#else
#define MESSAGE_CONSTRUCTOR_INFO()
#define MESSAGE_DESTRUCTOR_INFO()
#endif

//===========================================================================================================
//
//          Message  (class)
//
//===========================================================================================================

class Message {
protected:
	static uint64_t nextId;
	static uint64_t nbMessages;
public:
	uint64_t id;
	unsigned int type;
	NetworkInterface *sourceInterface, *destinationInterface;

	Message();
	virtual ~Message();

	static uint64_t getNbMessages();
	virtual string getMessageName();

	virtual unsigned int size() { return(4); }
	virtual Message* clone();
};

template <class T>
class MessageOf:public Message {
    T *ptrData;
    public :
    MessageOf(int t,const T &data):Message() { type=t; ptrData = new T(data);};
    ~MessageOf() { delete ptrData; };
    T* getData() const { return ptrData; };
    virtual Message* clone() {
        MessageOf<T> *ptr = new MessageOf<T>(type,*ptrData);
        ptr->sourceInterface = sourceInterface;
        ptr->destinationInterface = destinationInterface;
        return ptr;
    }
};

//===========================================================================================================
//
//	    NetworkInterface  (class)
//
//===========================================================================================================
class NetworkInterface {
protected :
	static unsigned int nextId;
	static int defaultDataRate;

	BaseSimulator::Rate* dataRate;
public:
	unsigned int globalId;
	unsigned int localId;
	deque<MessagePtr> outgoingQueue;
	BaseSimulator::BuildingBlock * hostBlock;
	Time availabilityDate;
	NetworkInterface *connectedInterface;

	MessagePtr messageBeingTransmitted;

	NetworkInterface(BaseSimulator::BuildingBlock *b);
	virtual ~NetworkInterface() = 0;

	virtual void send(Message *m) = 0;
	virtual void send() = 0;
};


//===========================================================================================================
//
//          P2PNetworkInterface  (class)
//
//===========================================================================================================

class P2PNetworkInterface : public NetworkInterface {
public:

	P2PNetworkInterface(BaseSimulator::BuildingBlock *b);
	~P2PNetworkInterface();
	
	void send(Message *m);
	
	bool addToOutgoingBuffer(MessagePtr msg);
	void send();
	void connect(P2PNetworkInterface *ni);
	int getConnectedBlockId() {
        return (connectedInterface!=NULL && connectedInterface->hostBlock!=NULL)?connectedInterface->hostBlock->blockId:-1;
	}

	bool isConnected();
	
	void setDataRate(BaseSimulator::Rate* r); 
	Time getTransmissionDuration(MessagePtr &m);

};

//==========================================================================================================
//
//	WirelessNetworkInterface  (class)
//
//==========================================================================================================

class WirelessNetworkInterface : public NetworkInterface {
protected:
	 unsigned int range;
public:
	WirelessNetworkInterface(BaseSimulator::BuildingBlock *b, int Wrange): NetworkInterface(b){
	range=Wrange;
	};
	~WirelessNetworkInterface();
	void send(Message *m);
	void setRange(int dist);
};
#endif /* NETWORK_H_ */
