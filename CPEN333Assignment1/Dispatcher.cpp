/*
7 November 2019

Raymond Yu 11298149
Jason LeBlanc 45015154
*/

#include "..\rt.h"
#include "..\resources.h"
#include <unordered_map>

TheMonitorOne E1Monitor;
TheMonitorTwo E2Monitor;
CRendezvous r1("CreationRendezvous", 4); // sync creation of 4x processes

struct IODispatch {
	char inputs[3];
};

IODispatch pipeIOData;
int messagePacket[5] = {}; // message packet interpretted from IO
int E1Message = 0; // message packet to be sent to E1 mailbox
int E1MessageFromPipe = 0;
int E2Message = 0; // message packet to be sent to E2 mailbox
int E2MessageFromPipe = 0;

int E1MessagePrevious = 0; // previous message packet sent to E1 mailbox for comparison
int E1MessageResponse = 0; // update from E1
int E2MessagePrevious = 0; // previous message packet sent to E2 mailbox for comparison
int E2MessageResponse = 0; // update from E2

int startFlag = 0; // start flag to initialize elevator
int arrayStartFlag = 0; // flag to send message again to E1 ***and 2?***
int arrayIncrementFlag = 0; // increment someArray counter by 1
int emergencyStopFlag = 0; // returns all elevators to ground, open doors, and terminate program

int someArray[100] = {}; // store all commands in here; can dynamically allocate for longer runtime

CSemaphore E1MailProducer("E1MailProducer", 0);
CSemaphore E1MailConsumer("E1MailConsumer", 1);
CSemaphore E2MailProducer("E2MailProducer", 0);
CSemaphore E2MailConsumer("E2MailConsumer", 1);

UINT __stdcall Thread1(void* args) {
	int i = 0;

	CTypedPipe <IODispatch> PipeIODispatch("PipelineIODispatch", 100); // room for 100 data

	while (1) {
		if (PipeIODispatch.TestForData() >= sizeof(pipeIOData) / 3) { // size of struct is 3
			PipeIODispatch.Read(&pipeIOData);

			// Debugging
			{console.Wait();
			std::cout << "Received " << pipeIOData.inputs << endl;
			console.Signal(); }

			// Safety first, check for emergency stop command
			if (pipeIOData.inputs[0] == 'e') {
				emergencyStopFlag = 1;

				// Same message format as elevator down
				messagePacket[0] = 1; // elevator select
				messagePacket[1] = 1;
				messagePacket[2] = 1;
				messagePacket[3] = 0;
				messagePacket[4] = 0; // convert character to int

				for (int j = 0; j < 5; j++) {
					E1MessageFromPipe *= 10;
					E1MessageFromPipe += messagePacket[j];
				}

				someArray[i] = E1MessageFromPipe;

				break; // stop while loop
			}
			// 'u' or 'd' command outside
			else if ((pipeIOData.inputs[0] == 'u') || (pipeIOData.inputs[0] == 'd')) {
				std::unordered_map<char, int> commandReference{ {'u', 2}, {'d', 1} }; // GCOM magic

				messagePacket[0] = 1; // elevator select
				messagePacket[1] = commandReference[pipeIOData.inputs[0]];
				messagePacket[2] = 1;
				messagePacket[3] = 0;
				messagePacket[4] = atoi(&pipeIOData.inputs[1]); // convert character to int
			}
			// '1' or '2' leading command inside
			else if ((pipeIOData.inputs[0] == '1') || (pipeIOData.inputs[0] == '2')) {
				std::unordered_map<char, int> floorReference{ {'1', 1}, {'2', 2} }; // GCOM magic x2

				messagePacket[0] = floorReference[pipeIOData.inputs[0]]; // elevator select
				messagePacket[1] = 0;
				messagePacket[2] = 1;
				messagePacket[3] = 0;
				messagePacket[4] = atoi(&pipeIOData.inputs[1]); // convert character to int
			}
			// '-' or '+' leading command take out of service
			else if ((pipeIOData.inputs[0] == '-') || (pipeIOData.inputs[0] == '+')) {
				//std::unordered_map<char, int> statusReference{ {'-', 0}, {'+', 1}, {'1', 1}, {'2', 2} }; // GCOM magic x2

				//messagePacket[0] = statusReference[pipeIOData.inputs[1]];
				//console.Wait();
				//cout << "Zeroth message index is: " << messagePacket[0] << endl;
				//console.Signal();
				//messagePacket[1] = 0;
				//cout << "First message index is: " << messagePacket[1] << endl;

				//messagePacket[2] = statusReference[pipeIOData.inputs[0]];
				//cout << "Second message index is: " << messagePacket[2] << endl;

				//messagePacket[3] = 0;
				//cout << "Third message index is: " << messagePacket[3] << endl;

				//messagePacket[4] = 0;
				//cout << "Fourth message index is: " << messagePacket[4] << endl;

			}
			//{// Debugging
			//	console.Wait();
			//	cout << "Message packet content is ";
			//	for (auto& mpData : messagePacket) // GCOM magic
			//		cout << mpData;
			//	cout << endl;
			//	console.Signal();
			//}

			// Convert message packet int array to int
			for (int j = 0; j < 5; j++) {
				E1MessageFromPipe *= 10;
				E1MessageFromPipe += messagePacket[j];
			}

			someArray[i] = E1MessageFromPipe;

			// Debugging
			console.Wait();
			{cout << __LINE__ << "\t";
			for (int j = 0; j < 99; j++) {
				if (someArray[j] != 0)
					cout << someArray[j];
			}
			cout << endl; }
			console.Signal();
			i++;

			console.Wait();
			cout << "Message to elevator one: " << E1MessageFromPipe << endl;
			console.Signal();

			E1MessageFromPipe = 0; // reset pipeline message after sent
		}
	}
	return 0;
}

UINT __stdcall Thread2(void* args) {
	int i = 0;

	while (1) {
		// Start elevators only happens once, sends start command to elevator once entire process starts
		// Initialize on floor 0 doors open
		if (startFlag == 0) {
			cout << __LINE__ << endl;

			E1MailConsumer.Wait(); // produce message for mailbox

			E1Message = 10110;
			E2Message = 20110;

			// Signal to mailbox that ready to send
			E1MailProducer.Signal();
			startFlag = 1;
		}

		E1MessageResponse = E1Monitor.getInfoDispatch();
		E2MessageResponse = E2Monitor.getInfoDispatch();

		if (emergencyStopFlag) {
			E1MessagePrevious = 11900; // [2] = 9 as termination
			E2MessagePrevious = 21900;
		}
		else if (someArray[i] != 0) {
			E1MessagePrevious = someArray[i]; // before someArray gets modified

			int theMessagePrevious[5] = {};
			// Convert int to int array
			for (int j = 4; j >= 0; j--) {
				theMessagePrevious[j] = someArray[i] % 10;
				someArray[i] /= 10;
			}

			console.Wait();
			cout << "theMessagePrevious is ";
			for (auto& mpData : theMessagePrevious) // GCOM magic
				cout << mpData;
			cout << endl;
			console.Signal();

			int E2SameCommand = 0;
			if (theMessagePrevious[0] == 1) {
				theMessagePrevious[0] = 2;
				// Convert int array to int
				for (int j = 0; j < 5; j++) {
					E2SameCommand *= 10;
					E2SameCommand += theMessagePrevious[j];
				}

				cout << "The same command to send to E2 is: " << E2SameCommand << endl;
			}

			//if (theMessagePrevious[0] == 0) { // means a 'u' or 'd' command received
			//	
			//}

			E2MessagePrevious = E2SameCommand; // brute force make two elevators move

			arrayIncrementFlag = 0;
		}
		else if (arrayStartFlag == 0) { // only happen once 
			E1MessagePrevious = 10110;
			E2MessagePrevious = 20110;
			arrayStartFlag = 1;
		}
		// Else previous message = previous message

		{console.Wait();
		cout << endl;
		cout << "E1 Message response: " << E1MessageResponse << endl;
		cout << "E1 Message previous: " << E1MessagePrevious << endl;
		cout << "E2 Message response: " << E2MessageResponse << endl;
		cout << "E2 Message previous: " << E2MessagePrevious << endl;
		console.Signal(); }

		// Check if termination message is sent by elevator 1
		if ((E1MessageResponse == 10910) && (E2MessageResponse == 20910)) {
			cout << __LINE__ << endl;

			E1MailConsumer.Wait(); // produce message for mailbox
			E1Message = 10910;
			E2Message = 20910;
			E1MailProducer.Signal();
			cout << __LINE__ << endl;

			break; // exit while loop
		}
		// Check if elevator done; -2000 checks up motion, +10 checks door
		// E.g. command - 1990 = elevator arrived on target floor stopped, and doors open
		else if (((E1MessagePrevious - 1990) == E1MessageResponse) && ((E2MessagePrevious - 1990) == E2MessageResponse) && (arrayIncrementFlag == 0)) {
			i++;
			//console.Wait();
			//cout << "Value of i is " << i << endl;
			//console.Signal();
			E1MailConsumer.Wait(); // produce message for mailbox
			E1Message = E1MessagePrevious - 1990;
			E2Message = E2MessagePrevious - 1990;
			E1MailProducer.Signal();
			arrayIncrementFlag = 1;
		}
		// Check if elevator done; -1000 checks down motion, +10 checks door
		// E.g. command - 990 = elevator arrived on target floor stopped, and doors open
		else if (((E1MessagePrevious - 990) == E1MessageResponse) && ((E2MessagePrevious - 990) == E2MessageResponse) && (arrayIncrementFlag == 0)) {
			i++;
			//console.Wait();
			//cout << "Value of i is " << i << endl;
			//console.Signal();
			E1MailConsumer.Wait(); // produce message for mailbox
			E1Message = E1MessagePrevious - 990;
			E2Message = E2MessagePrevious - 990;
			E1MailProducer.Signal();
			arrayIncrementFlag = 1;
		}
		// Check if elevator done from inside; +10 checks door (for up or down)
		// E.g. command + 10 = elevator arrived on target floor stopped, and doors open
		else if (((E1MessagePrevious + 10) == E1MessageResponse) && ((E2MessagePrevious + 10) == E2MessageResponse) && (arrayIncrementFlag == 0)) {
			i++;
			//console.Wait();
			//cout << "Value of i is " << i << endl;
			//console.Signal();
			E1MailConsumer.Wait(); // produce message for mailbox
			E1Message = E1MessagePrevious + 10;
			E2Message = E2MessagePrevious + 10;
			E1MailProducer.Signal();
			arrayIncrementFlag = 1;
		}
		else {
			E1MailConsumer.Wait(); // produce message for mailbox
			E2Message = E2MessagePrevious;
			E1Message = E1MessagePrevious;
			E1MailProducer.Signal();
		}
	}
	return 0;
}

int main(void) {
	// Elevator 1 child process
	CProcess p1("..\\Debug\\Elevator1.exe",
		NORMAL_PRIORITY_CLASS,
		OWN_WINDOW,
		ACTIVE
	);
	// Elevator 2 child process
	CProcess p2("..\\Debug\\Elevator2.exe",
		NORMAL_PRIORITY_CLASS,
		OWN_WINDOW,
		ACTIVE
	);
	// IO child process
	CProcess p3("..\\Debug\\IO.exe",
		NORMAL_PRIORITY_CLASS,
		OWN_WINDOW,
		ACTIVE
	);
	// Passengers child process
	CProcess p4("..\\Debug\\Passengers.exe",
		NORMAL_PRIORITY_CLASS,
		OWN_WINDOW,
		ACTIVE
	);

	// Rendezvous to start processes together
	r1.Wait();
	cout << "Dispatcher initializing..." << endl;

	CThread t1(Thread1, ACTIVE, NULL);
	CThread t2(Thread2, ACTIVE, NULL);

	while (1) {
		// Send mail to E1 through p1
		if ((E1MessageResponse == 10910) && (E2MessageResponse == 20910)) {// check termination first
			cout << "Dispatcher complete 0 ..." << endl;
			break;
		}

		E1MailProducer.Wait();
		cout << __LINE__ << endl;

		if (E1Message != 0) {
			p1.Post(E1Message);
			p2.Post(E2Message);
		}
		E1Message = 0; // reset message after sent
		cout << __LINE__ << endl;

		E1MailConsumer.Signal();
	}

	cout << "Dispatcher complete 1 ..." << endl;

	t1.WaitForThread();
	cout << __LINE__ << endl;

	t2.WaitForThread();

	cout << __LINE__ << endl;

	p1.WaitForProcess();
	cout << __LINE__ << endl;

	p2.WaitForProcess();
	cout << __LINE__ << endl;

	p3.WaitForProcess();

	p4.WaitForProcess();

	cout << "Dispatcher complete 2 ..." << endl;
	return 0;
}