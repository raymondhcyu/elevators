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

int E1Status[5] = {};

void dispatchPipeline(char* inputs);
void displayUpdates();

UINT __stdcall Thread1(void* args) {
	//console.Wait();
	//MOVE_CURSOR(0, 7);
	//cout << "Elevator 1 info: " << E1Monitor.getInfoIO() << endl;
	//console.Signal();
	return 0;
}

int main() {
	// Rendezvous to start
	r1.Wait();
	cout << "Elevator 1\nDirection\t\tService Status\t\tDoor status\t\tFloor\n" << endl;

	CThread t1(Thread1, ACTIVE, NULL);

	while (1) {
		// Get user input (move to separate function later)
		char input[3] = {};

		console.Wait();
		MOVE_CURSOR(0, 4);
		cout << "Enter a valid command: ";
		while ((input[0] != 'u') && (input[0] != 'd') && (input[0] != 'e')) {
			input[0] = _getch();
		}
		while ((input[1] != '2') && (input[1] != 'e')) {
			input[1] = _getch();
		}
		cout << endl;
		console.Signal();

		dispatchPipeline(input);

		// Convert monitor update to data
		int E1Update = E1Monitor.getInfoIO();
		for (int i = 4; i >= 0; i--) {
			E1Status[i] = E1Update % 10;
			E1Update /= 10;
		}

		console.Wait();
		if (E1Status[0] == 1) {
			MOVE_CURSOR(0, 2);

			switch (E1Status[1]) {
			case 2:
				cout << "Up			";
				break;
			case 1:
				cout << "Down			";
				break;
			case 0:
				cout << "Not moving		";
				break;
			}

			switch (E1Status[2]) {
			case 0:
				cout << "Out of service	";
				break;
			case 1:
				cout << "In service		";
				break;
			}

			switch (E1Status[3]) {
			case 0:
				cout << "Closed			";
				break;
			case 1:
				cout << "Open			";
				break;
			}

			cout << E1Status[4];
		}
		console.Signal();
	}

	t1.WaitForThread();
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