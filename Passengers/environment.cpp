#include "..\rt.h"
#include "..\resources.h"
#include "passenger.h"


int main(void) {

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //to change console colors etc 

	CSemaphore raceStatus("Race Status", 0, NUMBER_OF_CARS);
	CSemaphore raceStart("Race Start", 0, 1);
	CSemaphore monitor("Monitor", 1, 1);

	//READY SUPERVISOR
	Supervisor S;
	S.Resume();
	getchar();


	//READY CARS
	car* myCars[NUMBER_OF_CARS]; // create cars as active class objects

	for (int i = 0; i < NUMBER_OF_CARS; i++) {

		int pit1 = 0;
		int pit2 = 0;

		while ((pit1 == 0 && pit2 == 0) || (pit2 > NUMBER_OF_LAPS) || (pit1 > NUMBER_OF_LAPS)) {
			pit1 = 1 + (rand() % 10) * (NUMBER_OF_LAPS / 4); //radomly generate lap number to pit on
			pit2 = 1 + (rand() % 10) + (NUMBER_OF_LAPS / 2);

		}
		myCars[i] = new car(i + 1, pit1, pit2); //assigns carnum from 1-5 instead of 0-4
	}


	//START RACE
	monitor.Wait();
	cout << "Race starting in 3...2...1..." << endl; //anouncement
	monitor.Signal();

	for (int i = 0; i < NUMBER_OF_CARS; i++) { //start cars running
		myCars[i]->Resume(); //GET TO START AT SAME TIME LATER WITH A REDEVOUZ
	}

	Sleep(3000);

	monitor.Wait();
	cout << "GO!" << endl; //anouncement
	monitor.Signal();

	raceStart.Signal(); //lets cars begin at same time


	//FINISH
	for (int i = 0; i < NUMBER_OF_CARS; i++)
		myCars[i]->WaitForThread();
	S.WaitForThread();
	return 0;
}