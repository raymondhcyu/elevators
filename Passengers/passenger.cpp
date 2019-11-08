/*
7 November 2019

Raymond Yu 11298149
Jason LeBlanc 45015154
*/

#include "..\rt.h"
#include "..\resources.h"
#include "passenger.h"
#include <stdio.h>

struct IOPassenger {
	char inputs[3];
};

IOPassenger pipePassengerData;

passenger::passenger(int id) {
	pNum = id;

};



int passenger::main(void) {

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //to change console colors etc 

	//SEMAPHORES
	CSemaphore monitor("Monitor", 1, 1);
CTypedPipe <IOPassenger> PipeIOPassenger("PipelineIOPassenger", 100);
	int flagStatus = 0;

	while (1) {

		if (/*pStatus == 1 && */flagStatus != 1) {


			//send data to elevator via pieline

			char data[3] = {};
			data[0] = '1';
			data[1] = '3';

			strcpy_s(pipePassengerData.inputs, data); // copy input to struct, change to receive user input later

			PipeIOPassenger.Write(&pipePassengerData); //writes data to pipe

			flagStatus = 1; //disables this if staement

			console.Wait();
			cout << "test" << data[0] << data[1] << endl;
			console.Signal();
		}





	}

	

	return 0;

}



int passenger::getpStatus(void) {

	return pStatus;
}

int passenger::getpElevator(void) {

	return pElevator;

}

int passenger::getpFloorCurrent(void) {

	return pFloorCurr;

}

int passenger::getpFloorDesired(void) {

	return pFloorDes;

}

int passenger::getpNum(void) {

	return pNum;
}



void passenger::setpStatus(int status) {

	pStatus = status;
	
}

void passenger::setpElevator(int elevator) {

	pElevator = elevator;

}

void passenger::setpFloorCurrent(int floor) {

	pFloorCurr = floor;

}

void passenger::setpFloorDesired(int floor) {

	pFloorDes = floor;

}