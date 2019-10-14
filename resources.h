#ifndef __resources__
#define __resources__

#include "rt.h"

class TheMonitor {
private:
	struct theData {
		int floor; // which floor it is on; European convention
		int targetFloor;
		int status; // 0 = not moving, 1 = moving up, -1 = moving down
	};

	CDataPool* theDataPool;  // a datapool containing the data to be protected 
	CMutex* theMutex;	       // a pointer to a hidden mutex protecting the data
	theData* dataPtr;			// pointer to the data

public:
	void setFloor(int initFloor) {
		theMutex->Wait();
		dataPtr->floor = initFloor;
		theMutex->Signal();
	}

	int getFloor(void) {
		theMutex->Wait();
		return dataPtr->floor;
		theMutex->Signal();
	}

	void targetFloor(int desiredFloor) {
		theMutex->Wait();
		dataPtr->targetFloor = desiredFloor;
		theMutex->Signal();
	}

	void updateStatus(void) {
		// Move up
		theMutex->Wait();
		if (dataPtr->targetFloor > dataPtr->floor) {
			cout << "Elevator moving up..." << endl;
		}
		// Move down
		else if (dataPtr->targetFloor < dataPtr->floor)
			cout << "Elevator moving down..." << endl;
		else
			cout << "Elevator not moving..." << endl;
		theMutex->Signal();
	}

	// Constructor with single producer and two consumers
	TheMonitor (string Name) {
		theMutex = new CMutex(string("Mutex") + string(Name));
		theDataPool = new CDataPool(string("DataPool") + string(Name), sizeof(struct theData));
		dataPtr = (struct theData*)(theDataPool->LinkDataPool());

		// Where should these semaphores go?
		CSemaphore ps1("PS1", 0, 1); // semaphore w/ init value 0; max 1
		CSemaphore cs1("CS1", 1, 1); // sempahore w/ init value 1; max 1
		CSemaphore ps2("PS2", 0, 1);
		CSemaphore cs2("CS2", 1, 1);
	}
	~TheMonitor() {
	}
};

void testFunction(TheMonitor thisMonitor) {
	cout << "Hello from the embedded test function!" << endl;
	cout << "Elevator X is on floor " << thisMonitor.getFloor() << endl;
}

#endif