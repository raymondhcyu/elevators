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
	CSemaphore pitLight("Pit Light", 1, 1);
	CSemaphore pitStart("Pit Start", 1, 1);
	CSemaphore pitStop("Pit Stop", 1, 1);
	CSemaphore pitDecision("Pit Decision", 1, 1);
	CSemaphore raceStart("Race Start", 0, 1);
	CSemaphore monitor("Monitor", 1, 1);
	CSemaphore raceStatus("Race Status", 0, NUMBER_OF_CARS);



	while (lapCount < NUMBER_OF_LAPS) { //Race / Lap Counter

		int randVar = rand() % 10; //creates NEW random number betwen 0-9 each time car completes lap

		if (lapCountNoPit > 2) {

			randVar = LAP_THRESHOLD;

		}

		if (raceStart.Read() == 1) {

			if (lapCount == 0) { //Race start

				int launchTime = 1000 + 10 * carNum;
				Sleep(launchTime); // Launch - could later add bonus based on grid position (faster accel) *****************

			}
			else { //Remaining laps...

				//if(lapCount == pit1 || lapCount == pit2)
				if ((pitCount < PIT_MAX) && (lapCount == pitLap1 || lapCount == pitLap2 || pitFlag == 1)) {//(pitDecision.Read() && pitLight.Read()) { ////check if want to pit and check if anyone else pitting

					if ((pitStart.Read() == 1)) {

						monitor.Wait();
						SetConsoleTextAttribute(hConsole, YELLOW);
						cout << "car " << carNum << " pitting" << endl;
						monitor.Signal();

						pitStart.Wait(); //gives permission to Supervisor to begin pit
						pitStop.Wait();

						while (pitStop.Read() == 0); //wait until pit complete to continue

						lapCountNoPit = 0; //Reset number of laps since last pit
						pitCount++;
						pitFlag = 0;



					}
					else {
						pitFlag = 1;
					}


				}

				// Drive a Lap 

				int rand = 1; //Make random number from 0 - 9 later *********************

				speed = speed + 5; //adjust speed based on random factors

				int sleepTime = 5000 + (randVar * 100) + (lapCountNoPit * 50);

				Sleep(sleepTime); //Do a lap			

			}

			lapCount++; //increment lap counter

			monitor.Wait();
			SetConsoleTextAttribute(hConsole, PURPLE);
			cout << "Car " << carNum << " completed lap " << lapCount << endl; //anouncement
			monitor.Signal();

			//cout << "lap " << lapCount << " completed by car " << carNum << endl; //anouncement

		}

	}

	monitor.Wait();
	SetConsoleTextAttribute(hConsole, GREEN);
	cout << " race completed by car " << carNum << endl; //anouncement
	monitor.Signal();

	raceStatus.Signal(); // 

	return 0;

}