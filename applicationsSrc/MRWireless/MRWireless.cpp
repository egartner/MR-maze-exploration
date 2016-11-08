#include <iostream>
#include <trace.h>

#include "multiRobotsSimulator.h"
#include "multiRobotsBlockCode.h"
#include "MRWirelessBlockCode.h"

using namespace std;
using namespace MultiRobots;

int main(int argc, char **argv) {

	OUTPUT << "\033[1;33m" << "Starting Multi Robots simulation (main) ..." << "\033[0m" << endl;

	createSimulator(argc, argv, MRWirelessBlockCode::buildNewBlockCode);

	{
		using namespace BaseSimulator;

		Simulator *s = Simulator::getSimulator();
		s->printInfo();
	}

	deleteSimulator();

	OUTPUT << "\033[1;33m" << "end (main)" << "\033[0m" << endl;
	return(0);
}
