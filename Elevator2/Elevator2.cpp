#include "..\rt.h"
#include "..\resources.h"

TheMonitorTwo elevatorTwoMonitor;
CRendezvous r1("CreationRendezvous", 4); // sync creation of 4x processes

int main() {
	// Rendezvous to start
	r1.Wait();
	cout << "Elevator process 2 initializing..." << endl;

	getchar();
	return 0;
}