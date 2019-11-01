#include "..\rt.h"
#include "..\resources.h"

TheMonitorOne elevatorOneMonitor;
TheMonitorTwo elevatorTwoMonitor;
CRendezvous r1("CreationRendezvous", 4); // sync creation of 4x processes

int pipeIOData;

struct IODispatch {
	int valCom; // command to move floor
};

UINT __stdcall Thread1(void* args) {
	CTypedPipe <int> PipeIODispatch("PipelineIODispatch", 100); // room for 100 ints
	while (1) {
		if (PipeIODispatch.TestForData() >= sizeof(pipeIOData) / 4) {
			PipeIODispatch.Read(&pipeIOData);
			cout << "Dispatcher read " << pipeIOData << " from IO..." << endl;
			break;
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

	// Rendezvous to start processes together
	r1.Wait();
	cout << "Dispatcher initializing..." << endl;

	cout << "Elevator 1 is on floor " << elevatorOneMonitor.getFloorDispatch() << "..." << endl;
	cout << "Elevator 2 is on floor " << elevatorTwoMonitor.getFloorDispatch() << "..." << endl;

	t1.WaitForThread();

	p1.WaitForProcess();
	p2.WaitForProcess();
	p3.WaitForProcess();

	cout << "Dispatcher complete..." << endl;
	return 0;
}