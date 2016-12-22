#ifndef MRMELDBLOCKCODE_H_
#define MRMELDBLOCKCODE_H_

#include "multiRobotsBlock.h"
#include "multiRobotsBlockCode.h"
#include "multiRobotsSimulator.h"

class MRWirelessBlockCode : public MultiRobots::MultiRobotsBlockCode {
private:
	MultiRobots::MultiRobotsBlock *bb;
	Time currentLocalDate; // fastest mode
public:
	MRWirelessBlockCode(MultiRobots::MultiRobotsBlock *host);
	~MRWirelessBlockCode();

	void startup();
	void init();
	void processLocalEvent(EventPtr pev);
	void setCurrentLocalDate(Time t) {currentLocalDate = t;}
	static BlockCode *buildNewBlockCode(BuildingBlock *host);
};

#endif /* MRMELDBLOCKCODE_H_ */
