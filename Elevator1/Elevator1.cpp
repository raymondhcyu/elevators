/*
	Produces data using named monitor
*/

#include "D:\\Documents\\CPEN333\\Assignments\\CPEN333Assignment1\\rt.h"
#include "..\\resources.h"

TheMonitor elevatorOneMonitor("Elevator One Monitor");

CRendezvous r1("CreationRendezvous", 4); // sync creation of 4x processes

int main() {
	// Rendezvous to start
	Sleep(5000);
	r1.Wait();
	cout << "Elevator process 1 is done waiting" << endl;

	int i;
	for (i = 0; i < 10; i++) {
		cout << "Helloo " << i << " from elevator process 1..." << endl;
		Sleep(50);
	}

	getchar();
	return 0;
}