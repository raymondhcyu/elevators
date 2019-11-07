#include "..\rt.h"
#include "..\resources.h"
#include "passenger.h"


int main(void) {

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //to change console colors etc 

	CSemaphore raceStart("Race Start", 0, 1);
	CSemaphore monitor("Monitor", 1, 1);

	//READY
	cout << "Welcome to the Elevator..." << endl << "Please come in..." << endl;

	//READY PEEPS

	passenger* myPs[MAX_PASSENGERS]; // create people as active class objects
	
	int count = 0;

	while (1) {
		
		if (rand() / 100 / 7 == 0) {

		}
		

	}

	

	for (int i = 0; i < MAX_PASSENGERS; i++) {
		myPs[i] = new passenger(i + 1); //assigns unique id num
	}

	//START PEEPS
	for (int i = 0; i < MAX_PASSENGERS; i++) { //start people - ie. they exist, but may not be in the building
		myPs[i]->Resume();
	}

	//WAIT FOR PEEPS
	for (int i = 0; i < MAX_PASSENGERS; i++) {
		myPs[i]->WaitForThread();
	}

	return 0;
}