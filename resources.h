/*
7 November 2019

Raymond Yu 11298149
Jason LeBlanc 45015154
*/

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

#ifndef __resources__
#define __resources__

#define NUM_FLOORS 10
#define MAX_PASSENGERS 20

#define ANIMATION_TOPLEFT_X 10
#define ANIMATION_TOPLEFT_Y 11
#define E1_TOPLEFT_X 11
#define E2_TOPLEFT_X 19
#define INPUT_TOPLEFT_Y 5

#define ELEVATOR_DOOR_DELAY 200

#define BLACK 0
#define RED 4
#define YELLOW 6
#define GREEN 2
#define BLUE 1
#define PURPLE 5
#define WHITE 7 
#define GRAY 8

#define FILEPATH_BELL "..\\sounds\\bell1.wav"

#include "rt.h"

const CMutex console("Console");

class TheMonitorOne {
private:
	struct theData {
		int floor; // which floor it is on; European convention
		int doorStatus;
		int direction;
		int serviceStatus;
	};

	CDataPool* theDataPool;  // a datapool containing the data to be protected 
	theData* dataPtr;			// pointer to the data

	CSemaphore* ps1;
	CSemaphore* ps2;
	CSemaphore* cs1;
	CSemaphore* cs2;

public:
	TheMonitorOne(); // default constructor; default floor 0
	void setInfo(int *data); // set data in message packet format
	int getInfoIO(void); // get data in message packet format
	int getInfoDispatch(void); // get data in message packet format
	~TheMonitorOne();
};

class TheMonitorTwo {
private:
	struct theData {
		int floor; // which floor it is on; European convention
		int doorStatus;
		int direction;
		int serviceStatus;
	};

	CDataPool* theDataPool;  // a datapool containing the data to be protected 
	theData* dataPtr;			// pointer to the data

	CSemaphore* ps3;
	CSemaphore* ps4;
	CSemaphore* cs3;
	CSemaphore* cs4;

public:
	TheMonitorTwo(); // default constructor; default floor 0
	void setInfo(int* data); // set data in message packet format
	int getInfoIO(void); // get data in message packet format
	int getInfoDispatch(void); // get data in message packet format
	~TheMonitorTwo();
};

#endif