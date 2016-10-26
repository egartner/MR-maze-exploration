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
class WirelessMessage;
class NetworkInterface;
class P2PNetworkInterface;
class WirelessNetworkInterface;

typedef std::shared_ptr<Message> MessagePtr;
typedef std::shared_ptr<WirelessMessage> WirelessMessagePtr;

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
    //NetworkInterface *sourceInterface, *destinationInterface;
	P2PNetworkInterface *sourceInterface, *destinationInterface;

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
//          WirelessMessage  (class)
//
//===========================================================================================================

class WirelessMessage {
protected:
    static uint64_t nextId;
    static uint64_t nbMessages;
public:
    uint64_t id;
    unsigned int type;

    WirelessNetworkInterface *sourceInterface;
    bID destinationId;
    //, *destinationInterface;
    
    WirelessMessage(bID destId);
    virtual ~WirelessMessage();
    
    static uint64_t getNbMessages();
    virtual string getMessageName();
    
    virtual unsigned int size() { return(4); }
    virtual WirelessMessage* clone();
};

template <class T>
class WirelessMessageOf:public WirelessMessage {
    T *ptrData;
    public :
    WirelessMessageOf(int t,const T &data, bID destId):WirelessMessage(destId) { type=t; ptrData = new T(data);};
    ~WirelessMessageOf() { delete ptrData; };
    T* getData() const { return ptrData; };
    virtual WirelessMessage* clone() {
        WirelessMessageOf<T> *ptr = new WirelessMessageOf<T>(type,*ptrData, destinationId);
        ptr->sourceInterface = sourceInterface;
        //ptr->destinationInterface = destinationInterface;
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

//class P2PNetworkInterface : public NetworkInterface {
class P2PNetworkInterface {
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
    
    MessagePtr messageBeingTransmitted;

    
    P2PNetworkInterface *connectedInterface;
    
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

// TODO
// La puissance d'émission est configurée sur l'interface d'envoi
// Mais le calcul d'atténuation doit être effectué sur le recepteur (pour pouvoir gérer les collisions).
// La puissance d'émission doit donc être transmise au récepteur pour qu'il puisse effectuer le calcul.
//
// On n'exprime en général pas la puissance par une portée en mètre, mais par des dbm ou des watts.
// Un calcul permet de calculer une distance moyenne en fonction de la puissance (et en fonction du modèle
// de propagation utilisé).
//
// Il faut pouvoir faire la distinction entre les message broadcastés et ceux unicastés
// Il faut ajouter une adresse destination au message, alors qu'il n'y en avait pas dans les interfaces P2P
// Normalement, l'adresse destination est propre à la carte réseau ciblée ( et donc différente de l'ID du bloc)
// Ici on peut peut-être simplifier en utilisant l'ID du bloc (mais cela signifie qu'un bloc ne peut avoir qu'une interface
// wireless.
//
// Remettre le système de statitiques (statsIndividual)
class WirelessNetworkInterface {
protected:
    static unsigned int nextId;
    static int defaultDataRate;
    
    BaseSimulator::Rate* dataRate;
	// unsigned int range;
    float transmitPower;
    float receptionThreshold;
    WirelessMessagePtr messageBeingReceived;
    bool collisionOccuring;
public:
    unsigned int globalId;
    unsigned int localId;
    deque<WirelessMessagePtr> outgoingQueue;
    BaseSimulator::BuildingBlock * hostBlock;
    Time availabilityDate;
    
    WirelessMessagePtr messageBeingTransmitted;
    
    WirelessNetworkInterface(BaseSimulator::BuildingBlock *b, float power);
	~WirelessNetworkInterface();
    
    void setReceptionThreshold(float threshold);
    float getReceptionThreshold();
    
    bool addToOutgoingBuffer(WirelessMessagePtr msg);
	void send();
    void startReceive(WirelessMessagePtr msg);
    void stopReceive();
	void setTransmitPower(int power);
    float getTransmitPower();
	Time getTransmissionDuration(WirelessMessagePtr &m);
};
#endif /* NETWORK_H_ */
