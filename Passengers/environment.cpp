#include "..\rt.h"
#include "..\resources.h"
#include "passenger.h"


int main(void) {

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //to change console colors etc 

	CSemaphore raceStatus("Race Status", 0, numPassengers);
	CSemaphore raceStart("Race Start", 0, 1);
	CSemaphore monitor("Monitor", 1, 1);

	//READY
	cout << "Welcome to the Elevator..." << endl << "Please come in..." << endl;


	//READY PEEPS
	passenger* myPs[numPassengers]; // create cars as active class objects

	for (int i = 0; i < MAX_PASSENGERS; i++) {
		myPs[i] = new passenger(i + 1); //assigns unique id num
	}

	//START RACE
	monitor.Wait();
	cout << "Race starting in 3...2...1..." << endl; //anouncement
	monitor.Signal();

	for (int i = 0; i < NUMBER_OF_CARS; i++) { //start cars running
		myPs[i]->Resume(); //GET TO START AT SAME TIME LATER WITH A REDEVOUZ
	}

	Sleep(3000);

	monitor.Wait();
	cout << "GO!" << endl; //anouncement
	monitor.Signal();

	raceStart.Signal(); //lets cars begin at same time


	//FINISH
	for (int i = 0; i < MAX_PASSENGERS; i++) {
		myPs[i]->WaitForThread();
	}


	return 0;
}