


#ifndef __passenger__
#define __passenger__



class passenger : public ActiveClass
{
private:

	//VARS
	int pNum; //passenger ID number
	int pStatus = 0; // 0 - 
	int pFloor = 0; //Lap counter


	//MAIN
	int main(void);




public:



	//STRUCTORS
	car(int num, int pit1, int pit2);

};

#endif