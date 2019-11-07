/*
Message packet format to be passed to/from elevators
_ _ _ _ _
0 1 2 3 4

0: Elevator number (1 or 2)
1: Direction (0 not moving, 1 down, 2 up)
2: Service status (0 out of service, 1 in service, 9 emergency stop)
3: Door status (0 closed, 1 open)
4: Floor status (0 - 9)

Debugging tip: "Sprinkle print lines"
			cout << __LINE__ << endl;
*/

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
void TheMonitorOne::setInfo(int* data) {
	cs1->Wait();
	cs2->Wait();
	dataPtr->floor = data[4];
	dataPtr->doorStatus = data[3];
	dataPtr->serviceStatus = data[2];
	dataPtr->direction = data[1];
	ps1->Signal();
	ps2->Signal();
}

int TheMonitorOne::getInfoIO(void) {
	// Convert IO int array message into single int
	UINT IOmessage = 1; // 1 for elevator1

	ps1->Wait(); 
	IOmessage *= 10; // 10
	IOmessage += dataPtr->direction; // ex 11
	IOmessage *= 10; // ex 120
	IOmessage += dataPtr->serviceStatus; // ex 121
	IOmessage *= 10; // ex 1210
	IOmessage += dataPtr->doorStatus; // ex 1210
	IOmessage *= 10; // ex 12100
	IOmessage += dataPtr->floor; // ex 12109
	cs1->Signal();

	return IOmessage;
}

int TheMonitorOne::getInfoDispatch(void) {
	// Convert Dispatch int array message into single int
	UINT dispatchMessage = 1; // 1 for elevator1

	ps2->Wait();
	dispatchMessage *= 10; // 10
	dispatchMessage += dataPtr->direction; // ex 11
	dispatchMessage *= 10; // ex 120
	dispatchMessage += dataPtr->serviceStatus; // ex 121
	dispatchMessage *= 10; // ex 1210
	dispatchMessage += dataPtr->doorStatus; // ex 1210
	dispatchMessage *= 10; // ex 12100
	dispatchMessage += dataPtr->floor; // ex 12109
	cs2->Signal();

	return dispatchMessage;
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