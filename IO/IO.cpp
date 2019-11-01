#define _CRT_SECURE_NO_WARNINGS

#include "..\rt.h"
#include "..\resources.h"
#include <conio.h>
#include <stdio.h>

TheMonitorOne elevatorOneMonitor;
TheMonitorTwo elevatorTwoMonitor;
CRendezvous r1("CreationRendezvous", 4); // sync creation of 4x processes

struct IODispatch {
	char inputs[];
};

void dispatchPipeline(char* inputs);

int main() {
	// Rendezvous to start
	r1.Wait();
	cout << "IO initializing..." << endl;

	char input[] = {'\0', '\0'};
	cout << "Enter a valid command: "; // expecting "u2"
	while (input[0] != 'u') {
		input[0] = _getch();
	}
	while (input[1] != '2') {
		input[1] = _getch();
	}
	cout << "You entered " << input[0] << input[1] << endl;

	dispatchPipeline(input);
	cout << "Elevator 1 is on floor " << elevatorOneMonitor.getFloorIO() << "..." << endl;
	cout << "Elevator 2 is on floor " << elevatorTwoMonitor.getFloorIO() << "..." << endl;

	getchar();
	return 0;
}

void dispatchPipeline(char* userInput) {
	CTypedPipe <IODispatch> PipeIODispatch("PipelineIODispatch", 100);

	IODispatch dispatch;
	strcpy(dispatch.inputs, userInput); // copy input to struct, change to receive user input later

	PipeIODispatch.Write(&dispatch);
	cout << "IO wrote " << dispatch.inputs << " to dispatch pipeline!" << endl;
	cout << "Dispatch pipeline has " << PipeIODispatch.TestForData() << " units of data" << endl;
}