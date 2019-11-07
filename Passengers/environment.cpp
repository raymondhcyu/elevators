#include "..\rt.h"
#include "..\resources.h"
#include "passenger.h"
#include <iostream>  
#include <numeric>   

int arraySum(int a[], int n);


int main(void) {

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //to change console colors etc 

	CSemaphore raceStart("Race Start", 0, 1);
	CSemaphore monitor("Monitor", 1, 1);

	//READY
	cout << "Welcome to the Elevator..." << endl << "Please come in..." << endl;

	//READY PEEPS

	passenger* myPs[MAX_PASSENGERS]; // create people as active class objects
	int myPsTracker[MAX_PASSENGERS] = {};
	
	int index = 0;

	while (1) {
		

		//random passenger creation
		if (rand() / 100 / 7 == 0) {
			myPs[index] = new passenger(index); //assigns unique id num
			myPsTracker[index] = 1;
			index++;
		}
		



	}


	return 0;
}


int arraySum(int a[], int n)
{
	int initial_sum = 0;
	return accumulate(a, a + n, initial_sum);
}