#include "..\rt.h"
#include "..\resources.h"
#include "passenger.h"
#include <iostream>  
#include <numeric>   
#include <conio.h>
#include <stdio.h>
#include <Windows.h>
#include <fstream>


int arraySum(int a[], int n);


int main(void) {

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //to change console colors etc 

	CSemaphore raceStart("Race Start", 0, 1);
	CSemaphore monitor("Monitor", 1, 1);

	//READY
	cout << "Welcome to the Elevator..." << endl << "Please come in..." << endl;

	//READY PEEPS

	passenger* myPs[MAX_PASSENGERS]; // create people as active class objects
	int myPsTracker[MAX_PASSENGERS] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	
	int index = 0;
	int flag = 0;

	while (1) {

		////Find next available spot for person
		//for (int i = 0; i < MAX_PASSENGERS; i++) {

		//	if (myPsTracker[i] == 0)
		//	{
		//		index = i;
		//	}
		//}

		//random passenger creation
		//if ((rand() / 100) / 7 != 0) {
		if (flag == 0) {
			myPs[index] = new passenger(index);
			myPsTracker[index] = 1;
			
			//SET current floor
			myPs[index]->setpFloorCurrent(rand() % 10);

			//SET desired floor
			myPs[index]->setpFloorDesired(rand() % 10);

			//SET Passenger to enabled
			myPs[index]->setpStatus(1);

			cout << "Passenger " << myPs[index]->getpNum() << " on floor " << myPs[index]->getpFloorCurrent() << " requesting floor " << myPs[index]->getpFloorDesired() << endl;

		
			index++;
			flag = 1;

		}

	}

	getchar(); 

	return 0;
}


int arraySum(int a[], int n)
{
	int initial_sum = 0;
	return accumulate(a, a + n, initial_sum);
}