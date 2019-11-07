#include "..\rt.h"
#include "..\resources.h"

TheMonitorTwo E2Monitor;
CRendezvous r1("CreationRendezvous", 4); // sync creation of 4x processes
UINT mailboxMessage;
CMailbox dispatchMailbox;

int E2Current[5] = { 1, 0, 1, 1, 0 }; // current status of E2 before commands
int E2Command[5] = {};

int E2StartFlag = 0; // only once start condition
int E2StopFlag = 0; // only once end condition

int main() {
	// Rendezvous to start
	r1.Wait();
	cout << "Elevator process 2 initializing..." << endl;

	while (1) {
		if (dispatchMailbox.TestForMessage() == TRUE) {
			mailboxMessage = dispatchMailbox.GetMessage();
			cout << "Mailbox message: " << mailboxMessage << endl;

			// Convert mailbox int into int array for message format
			for (int i = 4; i >= 0; i--) {
				E2Command[i] = mailboxMessage % 10;
				mailboxMessage /= 10;
			}

			cout << __LINE__ << endl;

			// If not a stop command (later) and if difference greater than 0 move up
			if (E2Command[4] - E2Current[4] > 0) {
				E2StartFlag = 1; // not starting anymore since command sent
				cout << __LINE__ << endl;

				// Check if doors open
				if (E2Current[3] == 1) {
					cout << __LINE__ << endl;

					Sleep(500); // delay to close doors
					E2Current[3] = 0; // close doors
					E2Monitor.setInfo(E2Current); // send data
				}
				// Check if not moving
				else if (E2Current[1] == 0) {
					cout << __LINE__ << endl;

					Sleep(500); // delay to start moving up
					E2Current[1] = 2;
					E2Monitor.setInfo(E2Current);
				}
				// Else increment floor
				else {
					cout << __LINE__ << endl;

					Sleep(1000); // delay to move between floors
					E2Current[4]++;
					E2Monitor.setInfo(E2Current);
				}
			}
			// If not a stop command (later) and if difference less than 0 move down
			else if (E2Command[4] - E2Current[4] < 0) {
				cout << __LINE__ << endl;

				E2StartFlag = 1; // not start anymore since command sent

				// Check if doors open
				if (E2Current[3] == 1) {
					cout << __LINE__ << endl;

					Sleep(500); // delay to close doors
					E2Current[3] = 0; // close doors
					E2Monitor.setInfo(E2Current); // send data
				}
				// Check if not moving
				else if (E2Current[1] == 0) {
					cout << __LINE__ << endl;

					Sleep(500); // delay to start moving down
					E2Current[1] = 1;
					E2Monitor.setInfo(E2Current);
				}
				// Else decrement floor
				else {
					cout << __LINE__ << endl;

					Sleep(1000); // delay to move between floors
					E2Current[4]--;
					E2Monitor.setInfo(E2Current);
				}
			}
			// If arrived on floor
			else if ((E2Command[4] == E2Current[4]) && (E2StartFlag != 0)) {
				cout << __LINE__ << endl;

				// Check if moving up or down to stop moving & open doors (at request of Jason)
				if ((E2Current[1] == 2) || (E2Current[1] == 1)) {
					cout << __LINE__ << endl;
					Sleep(500); // delay to stop moving
					E2Current[1] = 0;
					E2Current[3] = 1;
					E2Monitor.setInfo(E2Current);
				}
				else if (E2Command[2] == 9) { // termination condition
					E2Current[2] = 9;
					E2Monitor.setInfo(E2Current);
					break;
				}
				//// Open doors
				//else if (E2Current[3] == 0) {
				//	cout << __LINE__ << endl;
				//	Sleep(500);
				//	E2Current[3] = 1;
				//	E2Monitor.setInfo(E2Current);
				//}
				else // condition where it stopped on destination floor and doors open
					E2Monitor.setInfo(E2Current);
			}

			// Only time this should be triggered is first time
			else {
				//cout << __LINE__ << endl;
				// Send to monitors
				E2Monitor.setInfo(E2Command);
			}

			cout << "E2 current is ";
			for (auto& mpData : E2Current) // GCOM magic
				cout << mpData;
			cout << endl;
			cout << __LINE__ << endl;
		}
	}

	cout << "Elevator 2 terminated..." << endl;

	return 0;
}