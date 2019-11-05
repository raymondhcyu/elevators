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

UINT __stdcall Thread1(void* args) {
	while (1) {
		// Convert monitor update int to int array for processing
		int E1Update = E1Monitor.getInfoIO(); // wait for data?

		for (int i = 4; i >= 0; i--) {
			E1Status[i] = E1Update % 10;
			E1Update /= 10;
		}

		console.Wait();
		if (E1Status[0] == 1) { // 12104
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

		MOVE_CURSOR(24, 4); // hotfix to move cursor back to input
		console.Signal();
	}
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
		console.Signal();
		// @ JASON DO THIS ***********************************************************
		// Take more inputs like -1 -2 +1 +2 ee
		// Do not accept inputs like 'de' or 'ue'
		// Display correct character inputs if possible instead of uuuuuuu2
		while ((input[0] != 'u') && (input[0] != 'd') && (input[0] != 'e')) {
			input[0] = _getch();
		}
		while ((input[1] != '2') && (input[1] != '1') && (input[1] != '0') && (input[1] != 'e')) {
			input[1] = _getch();
		}

		dispatchPipeline(input);
	}

	t1.WaitForThread();
	return 0;
}

void dispatchPipeline(char* userInput) {
	CTypedPipe <IODispatch> PipeIODispatch("PipelineIODispatch", 100);

	IODispatch dispatch;
	strcpy(dispatch.inputs, userInput); // copy input to struct, change to receive user input later

	PipeIODispatch.Write(&dispatch);
}