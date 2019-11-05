#include "..\rt.h"
#include "..\resources.h"

TheMonitorOne E1Monitor;
CRendezvous r1("CreationRendezvous", 4); // sync creation of 4x processes
UINT mailboxMessage;
CMailbox dispatchMailbox;

int E1Current[5] = {1, 0, 1, 1, 0}; // current status of E1 before commands
int E1Command[5] = {};

int E1StartFlag = 0; // only once start condition

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

			// If not a stop command (later) and if difference greater than 0 move up
			if (E1Command[4] - E1Current[4] > 0) {
				E1StartFlag = 1; // not starting anymore since command sent
				cout << __LINE__ << endl;

				// Check if doors open
				if (E1Current[3] == 1) {
					cout << __LINE__ << endl;

					Sleep(500); // delay to close doors
					E1Current[3] = 0; // close doors
					E1Monitor.setInfo(E1Current); // send data
				}
				// Check if not moving
				else if (E1Current[1] == 0) {
					cout << __LINE__ << endl;

					Sleep(500); // delay to start moving up
					E1Current[1] = 2;
					E1Monitor.setInfo(E1Current);
				}
				// Else increment floor
				else {
					cout << __LINE__ << endl;

					Sleep(1000); // delay to move between floors
					E1Current[4]++;
					E1Monitor.setInfo(E1Current);
				}
			}
			// If not a stop command (later) and if difference less than 0 move down
			else if (E1Command[4] - E1Current[4] < 0) {
				cout << __LINE__ << endl;

				E1StartFlag = 1; // not start anymore since command sent

				// Check if doors open
				if (E1Current[3] == 1) {
					cout << __LINE__ << endl;

					Sleep(500); // delay to close doors
					E1Current[3] = 0; // close doors
					E1Monitor.setInfo(E1Current); // send data
				}
				// Check if not moving
				else if (E1Current[1] == 0) {
					cout << __LINE__ << endl;

					Sleep(500); // delay to start moving down
					E1Current[1] = 1;
					E1Monitor.setInfo(E1Current);
				}
				// Else decrement floor
				else {
					cout << __LINE__ << endl;

					Sleep(1000); // delay to move between floors
					E1Current[4]--;
					E1Monitor.setInfo(E1Current);
				}
			}
			// If arrived on floor
			else if ((E1Command[4] == E1Current[4]) && (E1StartFlag != 0)) {
				cout << __LINE__ << endl;

				// Check if moving up or down to stop moving
				if ((E1Current[1] == 2) || (E1Current[1] == 1)) {
					cout << __LINE__ << endl;

					Sleep(500); // delay to stop moving
					E1Current[1] = 0;
					E1Monitor.setInfo(E1Current);
				}
				// Open doors
				else if (E1Current[3] == 0) {
					cout << __LINE__ << endl;

					Sleep(500);
					E1Current[3] = 1;
					E1Monitor.setInfo(E1Current);
				}
			}

			// Only time this should be triggered is first time
			else {
				cout << __LINE__ << endl;

				// Send to monitors
				E1Monitor.setInfo(E1Command);
			}

			cout << "E1 current is ";
			for (auto& mpData : E1Current) // GCOM magic
				cout << mpData;
			cout << endl;
			cout << __LINE__ << endl;

		}
	}

	getchar();
	return 0;
}