#include "..\rt.h"
#include "..\resources.h"

TheMonitorTwo elevatorTwoMonitor;
CRendezvous r1("CreationRendezvous", 4); // sync creation of 4x processes
UINT mailboxMessage;
CMailbox dispatchMailbox;

int E2Current[5] = { 1, 0, 1, 1, 0 }; // current status of E1 before commands
int E2Command[5] = {};

int E2StartFlag = 0; // only once start condition
int E2StopFlag = 0; // only once end condition

int main() {
	// Rendezvous to start
	r1.Wait();
	cout << "Elevator process 2 initializing..." << endl;

	while (1) {
		if (dispatchMailbox.TestForMessage() == TRUE) {
			// do stuff
		}
	}

	cout << "Elevator 2 terminated..." << endl;

	return 0;
}