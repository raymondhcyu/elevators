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

int E1MessagePrevious = 0; // previous message packet sent to E1 mailbox for comparison
int E1MessageResponse = 0; // update from E1

int startFlag = 0; // start flag to initialize elevator
int arrayStartFlag = 0; // flag to send message again to E1
int arrayIncrementFlag = 0; // increment someArray counter by 1

int someArray[100] = {}; // store all commands in here brute forced; can dynamically allocate for longer runtime

CSemaphore E1MailProducer("E1MailProducer", 0);
CSemaphore E1MailConsumer("E1MailConsumer", 1);

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

			if (pipeIOData.inputs[0] == 'e') {
				//console.Wait();
				//cout << "Program should exit now..." << endl;
				//console.Signal();
			}

			// 'u' or 'd' command outside
			if ((pipeIOData.inputs[0] == 'u') || (pipeIOData.inputs[0] == 'd')) {
				std::unordered_map<char, int> commandReference{ {'u', 2}, {'d', 1} }; // GCOM magic

				messagePacket[0] = 1; // elevator select
				messagePacket[1] = commandReference[pipeIOData.inputs[0]];
				messagePacket[2] = 1;
				messagePacket[3] = 0;
				messagePacket[4] = atoi(&pipeIOData.inputs[1]); // convert character to int
			}
			// '1' or '2' leading command inside
			else {
				messagePacket[0] = atoi(&pipeIOData.inputs[0]); // elevator select
				messagePacket[1] = 0;
				messagePacket[2] = 1;
				messagePacket[3] = 0;
				messagePacket[4] = atoi(&pipeIOData.inputs[1]); // convert character to int
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
			cout << __LINE__ << "\t";
			for (int j = 0; j < 99; j++) {
				if (someArray[j] != 0)
					cout << someArray[j];
			}
			cout << endl;
			i++;

			console.Wait();
			cout << "Message to elevator one: " << E1MessageFromPipe << endl;
			console.Signal();

			E1MessageFromPipe = 0; // reset pipeline message after sent

			//E1MailProducer.Signal();
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
			// 10110
			messagePacket[0] = 1;
			messagePacket[1] = 0;
			messagePacket[2] = 1;
			messagePacket[3] = 1;
			messagePacket[4] = 0;

			// Convert message packet int array to int
			for (int j = 0; j < 5; j++) {
				E1Message *= 10;
				E1Message += messagePacket[j];
			}

			// Signal to mailbox that ready to send
			E1MailProducer.Signal();
			startFlag = 1;
		}

		if (someArray[i] != 0) {
			E1MessagePrevious = someArray[i];
			arrayIncrementFlag = 0;
		}
		else if (arrayStartFlag == 0) { // only happen once 
			E1MessagePrevious = 10110;
			arrayStartFlag = 1;
		}
		// Else previous message = previous message

		E1MessageResponse = E1Monitor.getInfoDispatch();

		{console.Wait();
		cout << endl;
		cout << "E1 Message response: " << E1MessageResponse << endl;
		cout << "E1 Message previous: " << E1MessagePrevious << endl;
		console.Signal(); }

		// Check if elevator done; -2000 checks up motion, +10 checks door
		// E.g. command - 1990 = elevator arrived on target floor stopped, and doors open
		if (((E1MessagePrevious - 1990) == E1MessageResponse) && (arrayIncrementFlag == 0)) {
			i++;
			//console.Wait();
			//cout << "Value of i is " << i << endl;
			//console.Signal();
			E1MailConsumer.Wait(); // produce message for mailbox
			E1Message = E1MessagePrevious - 1990;
			E1MailProducer.Signal();
			arrayIncrementFlag = 1;
		}
		// Check if elevator done; -1000 checks down motion, +10 checks door
		// E.g. command - 990 = elevator arrived on target floor stopped, and doors open
		else if (((E1MessagePrevious - 990) == E1MessageResponse) && (arrayIncrementFlag == 0)) {
			i++;
			//console.Wait();
			//cout << "Value of i is " << i << endl;
			//console.Signal();
			E1MailConsumer.Wait(); // produce message for mailbox
			E1Message = E1MessagePrevious - 990;
			E1MailProducer.Signal();
			arrayIncrementFlag = 1;
		}
		else {
			E1MailConsumer.Wait(); // produce message for mailbox
			E1Message = E1MessagePrevious;
			E1MailProducer.Signal();
		}
	}
	return 0;
}

int main(void) {
	// Elevator 1 child process
	CProcess p1("D:\\Documents\\CPEN333\\Assignments\\CPEN333Assignment1\\Debug\\Elevator1.exe",
		NORMAL_PRIORITY_CLASS,
		OWN_WINDOW,
		ACTIVE
	);
	// Elevator 2 child process
	CProcess p2("D:\\Documents\\CPEN333\\Assignments\\CPEN333Assignment1\\Debug\\Elevator2.exe",
		NORMAL_PRIORITY_CLASS,
		OWN_WINDOW,
		ACTIVE
	);
	// IO child process
	CProcess p3("D:\\Documents\\CPEN333\\Assignments\\CPEN333Assignment1\\Debug\\IO.exe",
		NORMAL_PRIORITY_CLASS,
		OWN_WINDOW,
		ACTIVE
	);

	CThread t1(Thread1, ACTIVE, NULL);
	CThread t2(Thread2, ACTIVE, NULL);

	// Rendezvous to start processes together
	r1.Wait();
	cout << "Dispatcher initializing..." << endl;

	while (1) {
		// Send mail to E1 through p1
		E1MailProducer.Wait();
		if (E1Message != 0) {
			p1.Post(E1Message);
		}
		E1Message = 0; // reset message after sent
		E1MailConsumer.Signal();
	}

	t1.WaitForThread();
	t2.WaitForThread();

	p1.WaitForProcess();
	p2.WaitForProcess();
	p3.WaitForProcess();

	cout << "Dispatcher complete..." << endl;
	return 0;
}