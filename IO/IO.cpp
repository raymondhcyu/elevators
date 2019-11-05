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
int passengerEnable = 0;

void dispatchPipeline(char* inputs);

//function to get filtered user input from keyboard
void getUserInput(char* input); 

//Animation functions
void animationElevator(int* status);
void animationBuildFloorNums(void);
void animationElevatorControl(int* status);
void animationCurrentFloor(int* status);
void animationBuildLabels(void);
void animationBuildShafts(void);
void animationCurrentDirection(int* status);
void animationClearPrevious(int* status);
void animationUpdateStatusBar(int* status);
void animationBuildFramework(void);



UINT __stdcall UpdateDisplay(void* args) {

	//create layout for animation
	animationBuildFramework();

	while (1) { // MAIN LOOP

		// Convert monitor update int to int array for processing
		int E1Update = E1Monitor.getInfoIO(); // wait for data?
		for (int i = 4; i >= 0; i--) {
			E1Status[i] = E1Update % 10;
			E1Update /= 10;
		}

		//Update Elevator status bar in realtime
		animationUpdateStatusBar(E1Status);

		//Animate elevator in relatime
		animationElevator(E1Status);

		//reset cursor to data entry position
		MOVE_CURSOR(24, 4); 
		
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

			if (input[1] == '+') {
				passengerEnable = 1;
			}

			if (input[1] == '-') {
				passengerEnable = 0;
			}

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

void animationElevator(int* status) {

	//clear previous animation
	animationClearPrevious(status);

	//Clear previous floor color
	animationBuildFloorNums();

	//Display Elevator & door control
	animationElevatorControl(status);

	//Indicate Floor Elevator is on
	animationCurrentFloor(status);

	//Indicate UP or DOWN
	animationCurrentDirection(status);

}

void animationBuildFloorNums(void) {
	console.Wait();

	for (int i = 0; i < NUM_FLOORS; i++) {

		MOVE_CURSOR(ANIMATION_TOPLEFT_X - 5, ANIMATION_TOPLEFT_Y + i);
		SetConsoleTextAttribute(hConsole, GRAY);
		cout << "F" << (NUM_FLOORS - 1) - i;
		SetConsoleTextAttribute(hConsole, WHITE);

	}

	console.Signal();
}

void animationClearPrevious(int* status) {
	console.Wait();

	for (int i = 0; i < NUM_FLOORS; i++) {
		if (status[0] == 1) {
			MOVE_CURSOR(E1_TOPLEFT_X, ANIMATION_TOPLEFT_Y + i);
		}
		if (status[0] == 2) {
			MOVE_CURSOR(E2_TOPLEFT_X, ANIMATION_TOPLEFT_Y + i);

		}
		cout << "       ";
	}

	console.Signal();
}

void animationElevatorControl(int* status) {
	console.Wait();

	int i = 0; 

	while (i < 2) {
		if (status[0] == 1) {
			MOVE_CURSOR(E1_TOPLEFT_X, ANIMATION_TOPLEFT_Y + NUM_FLOORS - 1 - status[4]);
		}
		if (status[0] == 2) {
			MOVE_CURSOR(E2_TOPLEFT_X, ANIMATION_TOPLEFT_Y + NUM_FLOORS - 1 - status[4]);
		}

		if (status[2] == 0) { //if out of service
			SetConsoleTextAttribute(hConsole, RED);
			cout << "[  X  ]";
			SetConsoleTextAttribute(hConsole, WHITE);
			i = 2;
		}
		if (status[2] == 1) { //if in service: display door open or closed

			if (status[3] == 0) {
				if (i == 0) {
					SetConsoleTextAttribute(hConsole, YELLOW);
					cout << "[ | | ]";
					Sleep(500);
					i++;
				}
				else {
					SetConsoleTextAttribute(hConsole, GREEN);
					cout << "[  |  ]";
					i++;
				}
			}
			if (status[3] == 1) {
				SetConsoleTextAttribute(hConsole, YELLOW);
				if (i == 0) {
					
					cout << "[ | | ]";
					Sleep(500);
					i++;
				}
				else {
					cout << "[|   |]";
					i++;
				}
			}
			SetConsoleTextAttribute(hConsole, WHITE);
		}
	}

	console.Signal();
}

void animationCurrentFloor(int* status) {
	console.Wait();

	MOVE_CURSOR(ANIMATION_TOPLEFT_X - 5, ANIMATION_TOPLEFT_Y + NUM_FLOORS - 1 - status[4]);
	SetConsoleTextAttribute(hConsole, WHITE);
	cout << "F" << status[4];

	console.Signal();
}

void animationBuildLabels(void) {
	console.Wait();

	MOVE_CURSOR(ANIMATION_TOPLEFT_X + 4, ANIMATION_TOPLEFT_Y - 1);
	cout << "E1";
	MOVE_CURSOR(ANIMATION_TOPLEFT_X + 12, ANIMATION_TOPLEFT_Y - 1);
	cout << "E2";

	console.Signal();
}

void animationBuildShafts(void) {
	console.Wait();

	for (int i = 0; i < NUM_FLOORS; i++) {


		MOVE_CURSOR(ANIMATION_TOPLEFT_X, ANIMATION_TOPLEFT_Y + i);
		cout << "|";
		MOVE_CURSOR(ANIMATION_TOPLEFT_X + 8, ANIMATION_TOPLEFT_Y + i);
		cout << "|";
		MOVE_CURSOR(ANIMATION_TOPLEFT_X + 16, ANIMATION_TOPLEFT_Y + i);
		cout << "|";

	}

	console.Signal();
}

void animationUpdateStatusBar(int* status) {
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

	console.Signal();
}

void animationCurrentDirection(int* status) {
	console.Wait();

	int offset = 0;
	char string = ' ';

	if (status[0] == 1) { //E1
		offset = 3;
	}
	if (status[0] == 2) { //E2
		offset = 11;
	}

	if (status[1] == 1) { //If going down
		string = 'DOWN';
	}
	if (status[1] == 2) { //If going up
		string = ' UP ';
	}
	MOVE_CURSOR(ANIMATION_TOPLEFT_X + offset, ANIMATION_TOPLEFT_Y - 2);
	SetConsoleTextAttribute(hConsole, YELLOW);
	cout << string;
	SetConsoleTextAttribute(hConsole, WHITE);

	console.Signal();
}

void animationBuildFramework(void) {
	//Build elevator shafts
	animationBuildShafts();
	//Build floor numbers
	animationBuildFloorNums();
	//Build Labels
	animationBuildLabels();
}

void animatePeople(int* status, int* people) {

	console.Wait();


	console.Signal();

}
