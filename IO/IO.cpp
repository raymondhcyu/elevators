#define _CRT_SECURE_NO_WARNINGS

#include "..\rt.h"
#include "..\resources.h"
#include <conio.h>
#include <stdio.h>

TheMonitorOne E1Monitor;
TheMonitorTwo E2Monitor;
CRendezvous r1("CreationRendezvous", 4); // sync creation of 4x processes

CSemaphore inputB4output1("inputB4output1", 0);
CSemaphore inputB4output2("inputB4output2", 1);

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //to change console colors etc 

struct IODispatch {
	char inputs[3];
};

int E1Status[5] = {};

void dispatchPipeline(char* inputs);

//function to get filtered user input from keyboard
void getUserInput(char* input); 

//Animation function
void animateElevator(int* status);

UINT __stdcall UpdateDisplay(void* args) {

	//create layout for animation
	console.Wait();

	//Build elevator shafts
	for (int i = 0; i <= 10; i++) {
			
			MOVE_CURSOR(ANIMATION_TOPLEFT_X, ANIMATION_TOPLEFT_Y + i);
			cout << "|";
			MOVE_CURSOR(ANIMATION_TOPLEFT_X + 8, ANIMATION_TOPLEFT_Y + i);
			cout << "|";
			MOVE_CURSOR(ANIMATION_TOPLEFT_X + 16, ANIMATION_TOPLEFT_Y + i);
			cout << "|";

		}

	//Build floor numbers
	for (int i = 0; i <= 10; i++) {

		MOVE_CURSOR(ANIMATION_TOPLEFT_X-5, ANIMATION_TOPLEFT_Y + i);
		cout << "F" << 10 - i;

	}

	//Build Labels
	MOVE_CURSOR(ANIMATION_TOPLEFT_X + 4, ANIMATION_TOPLEFT_Y - 1);
	cout << "E1";
	MOVE_CURSOR(ANIMATION_TOPLEFT_X + 12, ANIMATION_TOPLEFT_Y - 1);
	cout << "E2";

	console.Signal();

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

		animateElevator(E1Status);

		MOVE_CURSOR(24, 4); // hotfix to move cursor back to input
		console.Signal();
	}
	return 0;
}

UINT __stdcall UserInput(void* args) {
	while (1) {
		inputB4output2.Wait();
		getUserInput((char*) args);
		inputB4output1.Signal();
	}
	return 0;
}


int main() {
	// Rendezvous to start
	r1.Wait();
	cout << "Elevator 1\nDirection\t\tService Status\t\tDoor status\t\tFloor\n" << endl;
	
	char input[3] = {};

	CThread t1(UpdateDisplay, ACTIVE, NULL);
	CThread t2(UserInput, ACTIVE, input);

	while (1) {

		inputB4output1.Wait();
		dispatchPipeline(input);
		inputB4output2.Signal();
		
	}

	t1.WaitForThread();
	t2.WaitForThread();

	return 0;
}


void dispatchPipeline(char* userInput) {
	CTypedPipe <IODispatch> PipeIODispatch("PipelineIODispatch", 100);

	IODispatch dispatch;
	strcpy(dispatch.inputs, userInput); // copy input to struct, change to receive user input later

	PipeIODispatch.Write(&dispatch);
}

void getUserInput(char* input) {

	console.Wait();
	MOVE_CURSOR(0, 4);
	cout << "Enter a valid command:\t     ";
	MOVE_CURSOR(0, 4);
	cout << "Enter a valid command:\t"; //reprint to get curser back to being next to semicolon
	MOVE_CURSOR(0, 5);
	cout << "Previous command: " << input[0] << input[1]; //reprint to get curser back to being next to semicolon
	console.Signal();

	input[0] = input[1] = ' '; //clear old inputs

	while ((input[0] != 'u') && (input[0] != 'd') && (input[0] != 'e') && (input[0] != '1') && (input[0] != '2') && (input[0] != '-') && (input[0] != '+')) {
		input[0] = _getch();
	}
	console.Wait();
	cout << input[0];
	console.Signal();

	if ((input[0] == 'u') || (input[0] == 'd') || (input[0] == '1') || (input[0] == '2')) {
		int floor = 99;
		while (floor > 9 || floor < 0) { //ALT METHOD: while((input[1] != '0') || (input[1] != '1') || (input[1] != '2') || (input[1] != '3') || .........

			input[1] = _getch();

			if (atoi(&input[1]) == 0) { //any non numeric character will return 0 via atoi which can be confused with floor number 0
				if (input[1] == '0') {
					floor = atoi(&input[1]); // convert character to int
				}
			}
			else {
				floor = atoi(&input[1]); // convert character to int
			}
		}
		console.Wait();
		cout << input[1];
		console.Signal();
	}

	if ((input[0] == '+') || (input[0] == '-')) {
		while ((input[1] != '2') && (input[1] != '1')) {
			input[1] = _getch();
		}
		console.Wait();
		cout << input[1];
		console.Signal();
	}

	if (input[0] == 'e') {
		while (input[1] != 'e') {
			input[1] = _getch();
		}
		console.Wait();
		cout << input[1];
		console.Signal();
	}

	Sleep(500); //Delay time for user input to display
}

void animateElevator(int* status) {

	console.Wait();

	//clear previous animation
		for (int i = 0; i <= 10; i++) {
			if (status[0] == 1) {
				MOVE_CURSOR(E1_TOPLEFT_X, ANIMATION_TOPLEFT_Y + i);
			}
			if (status[0] == 2) {
				MOVE_CURSOR(E2_TOPLEFT_X, ANIMATION_TOPLEFT_Y + i);

			}
			cout << "       ";
		}

		//Display Elevator
		if (status[0] == 1) {
			MOVE_CURSOR(E1_TOPLEFT_X, ANIMATION_TOPLEFT_Y + 10- status[4]);
		}
		if (status[0] == 2) {
			MOVE_CURSOR(E2_TOPLEFT_X, ANIMATION_TOPLEFT_Y + 10- status[4]);
		}

		if (status[2] == 0) { //if out of service
			SetConsoleTextAttribute(hConsole, RED);
			cout << "[  X  ]";
			SetConsoleTextAttribute(hConsole, WHITE);
		}
		if (status[2] == 1) { //if in service: display door open or closed
			SetConsoleTextAttribute(hConsole, GREEN);
			if (status[3] == 0) {
				cout << "[  |  ]";
			}
			if (status[3] == 1) {
				cout << "[|   |]";
			}
			SetConsoleTextAttribute(hConsole, WHITE);
		}

	console.Signal();
}
