#include "D:\Documents\CPEN333\Assignments\CPEN333Assignment1\rt.h"

CRendezvous r1("CreationRendezvous", 3); // sync creation of 4x processes
CRendezvous r2("TerminationRendezvous", 3); // sync deletion of 4x processes

struct monitor {
	int moveStatus; // 1 up, -1 down, 0 stationary
	int doorStatus; // opening, closing, closed, open
	int floor; // which floor it is on; European convention
};

struct IODispatch {
	int valCom; // command to move floor
};

void monitorDatapool();
void dispatchPipeline();

int main() {
	int i;
	for (i = 0; i < 10; i++) {
		cout << "Helloo " << i << " from IO..." << endl;
		Sleep(50);
	}

	Sleep(3500);
	r1.Wait();
	cout << "IO is done waiting" << endl;

	monitorDatapool();
	dispatchPipeline();

	getchar();
	return 0;
}

void monitorDatapool() {
	CDataPool dp1("Monitor1", sizeof(struct monitor));
	CDataPool dp2("Monitor2", sizeof(struct monitor));

	struct monitor* monitor1 = (struct monitor*)(dp1.LinkDataPool());
	struct monitor* monitor2 = (struct monitor*)(dp2.LinkDataPool());

	cout << "From IO, elevator 1 is on floor " << monitor1->floor << endl;
	cout << "From IO, elevator 2 is on floor " << monitor2->floor << endl;
}

void dispatchPipeline() {
	CTypedPipe <int> PipeIODispatch("PipelineIODispatch", 100);
	IODispatch dispatch;
	dispatch.valCom = 99; // change to receive user input later
	PipeIODispatch.Write(&dispatch.valCom);
	cout << "IO wrote " << dispatch.valCom << " to dispatch pipeline!" << endl;
	cout << "Dispatch pipeline has " << PipeIODispatch.TestForData() << " units of data" << endl;
}