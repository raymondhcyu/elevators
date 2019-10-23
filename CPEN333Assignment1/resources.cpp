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
}

// Update floor (producer)
void TheMonitor::setFloor(int initFloor) {
	cs1->Wait();
	cs2->Wait();
	dataPtr->floor = initFloor;
	ps1->Signal();
	ps2->Signal();
}

// Get floor (child dispatcher)
int TheMonitor::getFloorDispatch(void) {
	ps1->Wait();
	int theFloor = dataPtr->floor; // can't return here else doesn't signal
	cs1->Signal();

	return theFloor;
}

// Get floor (child IO)
int TheMonitor::getFloorIO(void) {
	ps2->Wait();
	int theFloor = dataPtr->floor; // can't return here else doesn't signal
	cs2->Signal();

	return theFloor;
}

TheMonitor::~TheMonitor() {}