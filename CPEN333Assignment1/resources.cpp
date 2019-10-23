#include "..\rt.h"
#include "..\resources.h"

TheMonitor::TheMonitor() {
	// Single producer (elevator) double consumer (dispatch and IO)
	ps1 = new CSemaphore("PS1", 0, 1);
	ps2 = new CSemaphore("PS2", 0, 1);
	cs1 = new CSemaphore("CS1", 1, 1);
	cs2 = new CSemaphore("CS2", 1, 1);
	theDataPool = new CDataPool(string("DataPool"), sizeof(struct theData));
	dataPtr = (struct theData*)(theDataPool->LinkDataPool());

	cs1->Wait();
	cs2->Wait();
	dataPtr->floor = 0;
	ps1->Signal();
	ps2->Signal();
}

// Update floor
void TheMonitor::setFloor(int initFloor) {
	cs1->Wait();
	cs2->Wait();
	dataPtr->floor = initFloor;
	ps1->Signal();
	ps2->Signal();
}

int TheMonitor::getFloor(void) {
	cs1->Wait();
	cs2->Wait();
	int theFloor = dataPtr->floor; // can't return here else doesn't signal
	ps1->Signal();
	ps2->Signal();

	return theFloor;
}

TheMonitor::~TheMonitor() {}