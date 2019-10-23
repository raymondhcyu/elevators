#include "..\rt.h"
#include "..\resources.h"

TheMonitor elevatorTwoMonitor;
CRendezvous r1("CreationRendezvous", 4); // sync creation of 4x processes

struct IODispatch {
	int valCom; // command to move floor
};

void dispatchPipeline();

int main() {
	// Rendezvous to start
	//r1.Wait();
	cout << "IO is done waiting" << endl;

	int i;
	for (i = 0; i < 10; i++) {
		cout << "Helloo " << i << " from IO..." << endl;
		Sleep(50);
	}

	//dispatchPipeline();

	cout << "elevator 2 is on floor " << elevatorTwoMonitor.getFloor() << "..." << endl;

	getchar();
	return 0;
}

void dispatchPipeline() {
	CTypedPipe <int> PipeIODispatch("PipelineIODispatch", 100);
	IODispatch dispatch;
	dispatch.valCom = 99; // change to receive user input later
	PipeIODispatch.Write(&dispatch.valCom);
	cout << "IO wrote " << dispatch.valCom << " to dispatch pipeline!" << endl;
	cout << "Dispatch pipeline has " << PipeIODispatch.TestForData() << " units of data" << endl;
}