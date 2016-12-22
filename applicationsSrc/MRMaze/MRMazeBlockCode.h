#ifndef MRMELDBLOCKCODE_H_
#define MRMELDBLOCKCODE_H_

#include "multiRobotsBlock.h"
#include "multiRobotsBlockCode.h"
#include "multiRobotsSimulator.h"

class MRMazeBlockCode : public MultiRobots::MultiRobotsBlockCode {
private:
	MultiRobots::MultiRobotsBlock *bb;
	Time currentLocalDate; // fastest mode
public:
	MRMazeBlockCode(MultiRobots::MultiRobotsBlock *host);
	~MRMazeBlockCode();

	void startup();
	void init();
	void processLocalEvent(EventPtr pev);
	void setCurrentLocalDate(Time t) {currentLocalDate = t;}
	static BlockCode *buildNewBlockCode(BuildingBlock *host);
};

#endif /* MRMELDBLOCKCODE_H_ */
