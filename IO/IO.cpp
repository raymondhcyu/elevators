#define _CRT_SECURE_NO_WARNINGS

#include "..\rt.h"
#include "..\resources.h"
#include <conio.h>
#include <stdio.h>

TheMonitorOne E1Monitor;
TheMonitorTwo E2Monitor;
CRendezvous r1("CreationRendezvous", 4); // sync creation of 4x processes

struct IODispatch {
	char inputs[3];
};

void dispatchPipeline(char* inputs);
void displayUpdates();

int main() {
	// Rendezvous to start
	r1.Wait();
	cout << "IO initializing..." << endl;

	cout << "\nElevator 1\nDoors\t\tFloor\t\tService status\t\tOther Status\n" << endl;

	while (1) {
		// Get user input (move to separate function later)
		char input[3] = {};
		cout << "Enter a valid command: ";
		while ((input[0] != 'u') && (input[0] != 'd') && (input[0] != 'e')) {
			input[0] = _getch();
		}
		while ((input[1] != '2') && (input[1] != 'e')) {
			input[1] = _getch();
		}
		cout << endl;
		dispatchPipeline(input);
		displayUpdates();
	}
	return 0;
}

void dispatchPipeline(char* userInput) {
	CTypedPipe <IODispatch> PipeIODispatch("PipelineIODispatch", 100);

	IODispatch dispatch;
	strcpy(dispatch.inputs, userInput); // copy input to struct, change to receive user input later

	PipeIODispatch.Write(&dispatch);
	//cout << "Dispatch pipeline has " << PipeIODispatch.TestForData() << " units of data" << endl;
	//cout << "IO wrote " << dispatch.inputs << " to dispatch pipeline!" << endl;
}

void displayUpdates(void) {
	cout << "Elevator 1 info: " << E1Monitor.getInfoIO() << endl;

	//MOVE_CURSOR(0, 5);
	//cout << "\t\t" << E1Monitor.getFloorIO() << endl;
}