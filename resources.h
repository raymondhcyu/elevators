#ifndef __resources__
#define __resources__

#include "rt.h"

const CMutex console("Console");

class TheMonitorOne {
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
	TheMonitorOne(); // default constructor; default floor 0
	void setFloor(int initFloor); // update floor
	int getFloorIO(void); // floor currently on
	int getFloorDispatch(void); // floor currently on
	~TheMonitorOne();
};

class TheMonitorTwo {
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
	TheMonitorTwo(); // default constructor; default floor 0
	void setFloor(int initFloor); // update floor
	int getFloorIO(void); // floor currently on
	int getFloorDispatch(void); // floor currently on
	~TheMonitorTwo();
};

#endif