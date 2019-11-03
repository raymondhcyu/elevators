#include "..\rt.h"
#include "..\resources.h"

TheMonitorOne E1Monitor;
CRendezvous r1("CreationRendezvous", 4); // sync creation of 4x processes
UINT mailboxMessage;
CMailbox dispatchMailbox;
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

			// Send to monitors
			E1Monitor.setInfo(E1Command);
		}
	}

	getchar();
	return 0;
}