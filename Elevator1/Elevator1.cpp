/*
	Produces data using named monitor
*/

#include "D:\\Documents\\CPEN333\\Assignments\\CPEN333Assignment1\\rt.h"
#include "..\\resources.h"

TheMonitor elevatorOneMonitor("Elevator One Monitor");

int main() {

	elevatorOneMonitor.setFloor(0);

	int i;
	for (i = 0; i < 10; i++) {
		cout << "Helloo " << i << " from elevator process 1..." << endl;
		Sleep(50);
	}

	testFunction(elevatorOneMonitor);

	getchar();
	return 0;
}