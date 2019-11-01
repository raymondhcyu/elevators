#include "..\rt.h"
#include "..\resources.h"

TheMonitorOne elevatorOneMonitor;
TheMonitorTwo elevatorTwoMonitor;
CRendezvous r1("CreationRendezvous", 4); // sync creation of 4x processes

struct IODispatch {
	char inputs[3];
};

IODispatch pipeIOData;

UINT __stdcall Thread1(void* args) {
	CTypedPipe <IODispatch> PipeIODispatch("PipelineIODispatch", 100); // room for 100 data
	while (1) {
		if (PipeIODispatch.TestForData() >= sizeof(pipeIOData) / 2) {
			PipeIODispatch.Read(&pipeIOData);
			console.Wait();
			cout << "Dispatcher read " << pipeIOData.inputs << " from IO..." << endl;
			console.Signal();
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

	console.Wait();
	cout << "Elevator 1 is on floor " << elevatorOneMonitor.getFloorDispatch() << "..." << endl;
	cout << "Elevator 2 is on floor " << elevatorTwoMonitor.getFloorDispatch() << "..." << endl;
	console.Signal();

	t1.WaitForThread();

	p1.WaitForProcess();
	p2.WaitForProcess();
	p3.WaitForProcess();

	cout << "Dispatcher complete..." << endl;
	return 0;
}