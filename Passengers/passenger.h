


#ifndef __Car__
#define __Car__



class car : public ActiveClass
{
private:

	//VARS
	int carNum; //unique car number
	int speed = 0; // vehicle speed
	int lapCount = 0; //Lap counter
	int lapCountNoPit = 0; //Laps since last pit
	int pitDecision = 0; //Flag to signal pit or no pit; 0 = nopit, 1 = pit
	int pitCount = 0; //to ensure 2 pits met, but for now being used to limit number of pits 
	int pitLap1;
	int pitLap2;
	int pitFlag = 0;

	//MAIN
	int main(void);




public:



	//STRUCTORS
	car(int num, int pit1, int pit2);

};

#endif