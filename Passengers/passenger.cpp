#include "..\rt.h"
#include "..\resources.h"
#include "passenger.h"



passenger::passenger(int id) {
	carNum = num;
	pitLap1 = pit1;
	pitLap2 = pit2;

};



int passenger::main(void) {

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //to change console colors etc 

	//SEMAPHORES
	CSemaphore monitor("Monitor", 1, 1);



	

	return 0;

}

int passenger::getpStatus(void) {

	return pStatus;
}

int passenger::getpElevator(void) {

	return pElevator;

}

int passenger::getpFloor(void) {

	return pFloor;

}

int passenger::setpStatus(int status) {

	pStatus = status;
	
}

int passenger::setpElevator(int elevator) {

	pElevator = elevator;

}

int passenger::setpFloor(int floor) {

	pFloor = floor;

}