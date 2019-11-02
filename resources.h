/*
Message packet format to be passed to/from elevators
_ _ _ _ _
0 1 2 3 4

0: Elevator number (1 or 2)
1: Direction (0 not moving, 1 down, 2 up)
2: Service status (0 out of service, 1 in service)
3: Door status (0 closed, 1 open)
4: Floor status (0 - 9)
*/

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