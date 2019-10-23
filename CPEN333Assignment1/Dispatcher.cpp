#include "D:\Documents\CPEN333\Assignments\CPEN333Assignment1\rt.h"
#include "..\\resources.h"

TheMonitor elevatorOneMonitor("Elevator One Monitor");
CRendezvous r1("CreationRendezvous", 4); // sync creation of 4x processes

void initializeProcesses();

struct IODispatch {
	int valCom; // command to move floor
};

int main(void) {
	cout << "Dispatcher running..." << endl;

	initializeProcesses();

	cout << "Dispatcher complete..." << endl;
	return 0;
}

void initializeProcesses() {
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

	// Rendezvous to start processes together
	r1.Wait();
	for (int i = 0; i < 10; i++) {
		cout << "Helloo " << i << " from dispatcher..." << endl;
		Sleep(50);
	}

	// Typed pipe for data relay
	int pipeIOData;
	CTypedPipe <int> PipeIODispatch("PipelineIODispatch", 100);
	while (1) {
		if (PipeIODispatch.TestForData() >= sizeof(pipeIOData) / 4) {
			PipeIODispatch.Read(&pipeIOData);
			cout << "Dispatcher read " << pipeIOData << " from IO..." << endl;
			break;
		}
	}

	cout << "Waiting for self-made child process 1 to terminate..." << endl;
	p1.WaitForProcess();
	cout << "Waiting for self-made child process 2 to terminate..." << endl;
	p2.WaitForProcess();
	cout << "Waiting for self-made child process 3 to terminate..." << endl;
	p3.WaitForProcess();
}