#include "..\rt.h"
#include "..\resources.h"

TheMonitorOne E1Monitor;
CRendezvous r1("CreationRendezvous", 4); // sync creation of 4x processes
UINT mailboxMessage;
CMailbox dispatchMailbox;

int E1Current[5] = {1, 0, 1, 1, 0}; // current status of E1 before commands
int E1Command[5] = {};

int main() {
	// Rendezvous to start
	r1.Wait();
	cout << "Elevator process 1 initializing..." << endl;

	while (1) {
		if (dispatchMailbox.TestForMessage() == TRUE) {
			mailboxMessage = dispatchMailbox.GetMessage();
			cout << "Mailbox message: " << mailboxMessage << endl;

			// Convert mailbox int into int array for message format
			for (int i = 4; i >= 0; i--) {
				E1Command[i] = mailboxMessage % 10;
				mailboxMessage /= 10;
			}

			cout << __LINE__ << endl;

			// If not a stop command and if difference greater than 0
			if (E1Command[4] - E1Current[4] > 0) {
				cout << __LINE__ << endl;

				// Check if doors open
				if (E1Current[3] == 1) {
					cout << __LINE__ << endl;

					Sleep(500); // delay to close doors
					E1Current[3] = 0; // close doors
					E1Monitor.setInfo(E1Current); // send data
				}
				// Check if moving
				else if (E1Current[1] == 0) {
					cout << __LINE__ << endl;

					Sleep(500); // delay to move
					E1Current[1] = 2;
					E1Monitor.setInfo(E1Current);
				}
				//// Check if arrived to floor
				//Sleep(100); // transit between floors
				//E1Current[4]++; // increase floor
			}

			cout << __LINE__ << endl;

			// Send to monitors
			E1Monitor.setInfo(E1Command);
		}
	}

	getchar();
	return 0;
}