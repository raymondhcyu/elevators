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
	theData* dataPtr;			// pointer to the data

	CSemaphore* ps1;
	CSemaphore* ps2;
	CSemaphore* cs1;
	CSemaphore* cs2;

public:
	void setFloor(int initFloor) {
		cs1->Wait();
		cs2->Wait();
		dataPtr->floor = initFloor;
		ps1->Signal();
		ps2->Signal();
	}

	int getFloor(void) {
		cs1->Wait();
		cs2->Wait();
		return dataPtr->floor;
		ps1->Signal();
		ps2->Signal();
	}

	void targetFloor(int desiredFloor) {
		cs1->Wait();
		cs2->Wait();
		dataPtr->targetFloor = desiredFloor;
		ps1->Signal();
		ps2->Signal();
	}

	void updateStatus(void) {
		// Move up
		if (dataPtr->targetFloor > dataPtr->floor) {
			cout << "Elevator moving up..." << endl;
		}
		// Move down
		else if (dataPtr->targetFloor < dataPtr->floor)
			cout << "Elevator moving down..." << endl;
		else
			cout << "Elevator not moving..." << endl;
	}

	// Constructor with single producer and two consumers
	TheMonitor (string Name) {
		ps1 = new CSemaphore("PS1", 0, 1);
		ps2 = new CSemaphore("PS2", 0, 1);
		cs1 = new CSemaphore("PS1", 1, 1);
		cs2 = new CSemaphore("PS2", 1, 1);
		theDataPool = new CDataPool(string("DataPool") + string(Name), sizeof(struct theData));
		dataPtr = (struct theData*)(theDataPool->LinkDataPool());
	}
	~TheMonitor() {
	}
};

void testFunction(TheMonitor thisMonitor) {
	cout << "Hello from the embedded test function!" << endl;
	cout << "Elevator X is on floor " << thisMonitor.getFloor() << endl;
}

#endif