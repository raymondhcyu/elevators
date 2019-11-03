#include "..\rt.h"
#include "..\resources.h"
#include <unordered_map>

TheMonitorOne elevatorOneMonitor;
TheMonitorTwo elevatorTwoMonitor;
CRendezvous r1("CreationRendezvous", 4); // sync creation of 4x processes

char exitProgram[] = { 'e', 'e', '\0' };

struct IODispatch {
	char inputs[3];
};

IODispatch pipeIOData;
int messagePacket[5] = {}; // message packet interpretted from IO
int elevatorOneMessage = 0; // message packet to be sent to E1 mailbox

CSemaphore messagePacketProducer("MessagePacketProducer", 0);
CSemaphore messagePacketConsumer("MessagePacketConsumer", 1);

UINT __stdcall Thread1(void* args) {

	CTypedPipe <IODispatch> PipeIODispatch("PipelineIODispatch", 100); // room for 100 data

	while (1) {
		if (PipeIODispatch.TestForData() >= sizeof(pipeIOData) / 3) { // size of struct is 3
			PipeIODispatch.Read(&pipeIOData);

			std::cout << "Received " << pipeIOData.inputs << endl;
			std::unordered_map<char, int> commandReference { {'u', 2}, {'d', 1} }; // GCOM magic

			messagePacketConsumer.Wait();
			messagePacket[0] = 1;
			messagePacket[1] = commandReference[pipeIOData.inputs[0]];
			messagePacket[2] = 1;
			messagePacket[3] = 0;
			messagePacket[4] = atoi(&pipeIOData.inputs[1]); // convert character to int
			messagePacketProducer.Signal();

			console.Wait();
			cout << "Message packet content is ";
			for (auto& mpData : messagePacket) // GCOM magic
				cout << mpData;
			cout << endl;
			console.Signal();
			break;
		}
	}
	return 0;
}

UINT __stdcall Thread2(void* args) {
	messagePacketProducer.Wait();
	// Convert message packet int array to int
	for (int i = 0; i < 5; i++) {
		elevatorOneMessage *= 10;
		elevatorOneMessage += messagePacket[i];
	}
	messagePacketConsumer.Signal();

	// *** Need separate semaphores for elevator one message to send to elevator mailbox

	console.Wait();
	cout << "Elevator one message is: " << elevatorOneMessage << endl;
	console.Signal();
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

	console.Wait();
	cout << "Elevator 1 is on floor " << elevatorOneMonitor.getFloorDispatch() << "..." << endl;
	cout << "Elevator 2 is on floor " << elevatorTwoMonitor.getFloorDispatch() << "..." << endl;
	console.Signal();

	t1.WaitForThread();
	t2.WaitForThread();

	p1.WaitForProcess();
	p2.WaitForProcess();
	p3.WaitForProcess();

	cout << "Dispatcher complete..." << endl;
	return 0;
}