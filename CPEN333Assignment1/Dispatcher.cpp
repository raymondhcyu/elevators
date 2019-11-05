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

CSemaphore messagePacketProducer("MessagePacketProducer", 0);
CSemaphore messagePacketConsumer("MessagePacketConsumer", 1);
CSemaphore E1MailProducer("E1MailProducer", 0);
CSemaphore E1MailConsumer("E1MailConsumer", 1);

UINT __stdcall Thread1(void* args) {

	CTypedPipe <IODispatch> PipeIODispatch("PipelineIODispatch", 100); // room for 100 data

	while (1) {
		if (PipeIODispatch.TestForData() >= sizeof(pipeIOData) / 3) { // size of struct is 3
			PipeIODispatch.Read(&pipeIOData);

			std::cout << "Received " << pipeIOData.inputs << endl;
			if (pipeIOData.inputs[0] == 'e') {
				console.Wait();
				cout << "Program should exit now" << endl;
				console.Signal();
			}
			std::unordered_map<char, int> commandReference { {'u', 2}, {'d', 1} }; // GCOM magic
			
			// Up or down commands only
			messagePacketConsumer.Wait();
			messagePacket[0] = 1;
			messagePacket[1] = commandReference[pipeIOData.inputs[0]];
			messagePacket[2] = 1;
			messagePacket[3] = 0;
			messagePacket[4] = atoi(&pipeIOData.inputs[1]); // convert character to int

			console.Wait();
			cout << "Message packet content is ";
			for (auto& mpData : messagePacket) // GCOM magic
				cout << mpData;
			cout << endl;
			console.Signal();
			messagePacketProducer.Signal();
		}
	}
	return 0;
}

UINT __stdcall Thread2(void* args) {
	while (1) {
		messagePacketProducer.Wait(); // consume message from pipeline
		E1MailConsumer.Wait(); // produce message for mailbox

		// Convert message packet int array to int
		for (int i = 0; i < 5; i++) {
			E1Message *= 10;
			E1Message += messagePacket[i];
		}
		console.Wait();
		cout << "Elevator one message: " << E1Message << endl;
		console.Signal();

		E1MailProducer.Signal();
		messagePacketConsumer.Signal();
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
		E1MailProducer.Wait();
		if (E1Message != 0)
			p1.Post(E1Message);
		E1Message = 0; // reset message after sent
		cout << "Elevator 1 info: " << E1Monitor.getInfoDispatch() << endl;
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