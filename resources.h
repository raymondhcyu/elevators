#ifndef __resources__
#define __resources__

#include "rt.h"
#include <string.h>

class TheMonitor {
private:
	struct theData {
		int floor; // which floor it is on; European convention
	};

	CDataPool* theDataPool;  // a datapool containing the data to be protected 
	theData* dataPtr;			// pointer to the data

	CSemaphore* ps1;
	CSemaphore* ps2;
	CSemaphore* cs1;
	CSemaphore* cs2;

public:
	TheMonitor(); // default constructor; default floor 0
	void setFloor(int initFloor); // update floor
	int getFloor(void); // floor currently on
	~TheMonitor();
};

#endif