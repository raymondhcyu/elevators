#include "..\rt.h"
#include "..\resources.h"
#include <iostream>
#include <string>

TheMonitorOne::TheMonitorOne() {
	// Single producer (elevator) double consumer (dispatch and IO)
	ps1 = new CSemaphore("PS1", 0, 1);
	ps2 = new CSemaphore("PS2", 0, 1);
	cs1 = new CSemaphore("CS1", 1, 1);
	cs2 = new CSemaphore("CS2", 1, 1);
	theDataPool = new CDataPool(string("DataPool1"), sizeof(struct theData));
	dataPtr = (struct theData*)(theDataPool->LinkDataPool());
}

// Update floor (producer)
void TheMonitorOne::setFloor(int initFloor) {
	cs1->Wait();
	cs2->Wait();
	dataPtr->floor = initFloor;
	ps1->Signal();
	ps2->Signal();
}

// Get floor (child dispatcher)
int TheMonitorOne::getFloorDispatch(void) {
	ps1->Wait();
	int theFloor = dataPtr->floor; // can't return here else doesn't signal
	cs1->Signal();

	return theFloor;
}

// Get floor (child IO)
int TheMonitorOne::getFloorIO(void) {
	ps2->Wait();
	int theFloor = dataPtr->floor; // can't return here else doesn't signal
	cs2->Signal();

	return theFloor;
}

TheMonitorOne::~TheMonitorOne() {}

TheMonitorTwo::TheMonitorTwo() {
	// Single producer (elevator) double consumer (dispatch and IO)
	ps1 = new CSemaphore("PS3", 0, 1);
	ps2 = new CSemaphore("PS4", 0, 1);
	cs1 = new CSemaphore("CS3", 1, 1);
	cs2 = new CSemaphore("CS4", 1, 1);
	theDataPool = new CDataPool(string("DataPool2"), sizeof(struct theData));
	dataPtr = (struct theData*)(theDataPool->LinkDataPool());
}

// Update floor (producer)
void TheMonitorTwo::setFloor(int initFloor) {
	cs1->Wait();
	cs2->Wait();
	dataPtr->floor = initFloor;
	ps1->Signal();
	ps2->Signal();
}

// Get floor (child dispatcher)
int TheMonitorTwo::getFloorDispatch(void) {
	ps1->Wait();
	int theFloor = dataPtr->floor; // can't return here else doesn't signal
	cs1->Signal();

	return theFloor;
}

// Get floor (child IO)
int TheMonitorTwo::getFloorIO(void) {
	ps2->Wait();
	int theFloor = dataPtr->floor; // can't return here else doesn't signal
	cs2->Signal();

	return theFloor;
}

TheMonitorTwo::~TheMonitorTwo() {}