#ifndef __passenger__
#define __passenger__



class passenger : public ActiveClass
{
private:

	//VARS
	int pNum; //passenger ID number
	int pStatus = 0; // 0 - Deactivated, 1 Activated
	int pElevator = 0; // 1 - Elevator 1, 2 - Elevator 2
	int pFloorCurr = 0; // Start on floor 0 always
	int pFloorDes = 0;


	//MAIN
	int main(void);


public:
	
	
	int getpNum(void);
	int getpStatus(void);
	int getpElevator(void);
	int getpFloorCurrent(void);
	int getpFloorDesired(void);

	void setpStatus(int status);
	void setpElevator(int elevator);
	void setpFloorCurrent(int floor);
	void setpFloorDesired(int floor);

	//STRUCTORS
	passenger(int id);

};

#endif