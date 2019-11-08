#define _CRT_SECURE_NO_WARNINGS

#include "..\rt.h"
#include "..\resources.h"
#include <conio.h>
#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <fstream>

TheMonitorOne E1Monitor;
TheMonitorTwo E2Monitor;

CRendezvous r1("CreationRendezvous", 4); // sync creation of 4x processes

CSemaphore inputB4output1("inputB4output1", 0);
CSemaphore inputB4output2("inputB4output2", 1);

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //to change console colors etc 

struct IODispatch {
	char inputs[3];
};

struct IOPassenger {
	char inputs[3];
};

IOPassenger pipePassengerData;

CTypedPipe <IOPassenger> PipeIOPassenger("PipelineIOPassenger", 100); // room for 100 data

int E1Status[5] = {};
int E2Status[5] = {};
int passengerEnable = 0;

//Pipeline
int dispatchPipeline(char* inputs);

//function to get filtered user input from keyboard
int getUserInput(char* input); 

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

	int previousE1Packet[5] = {0,0,0,0,0};
	int previousE2Packet[5] = {0,0,0,0,0};
	int flagE1 = 0;
	int flagE2 = 0;

	//create layout for animation
	animationBuildFramework();

	while (1) { // MAIN LOOP

		// Convert monitor update int to int array for processing
		int E1Update = E1Monitor.getInfoIO(); // wait for data?
		int E2Update = E2Monitor.getInfoIO(); // wait for data? //ERROR HERE - stuck waiting for E2 Data...
		for (int i = 4; i >= 0; i--) {
			E1Status[i] = E1Update % 10;
			E1Update /= 10;
			E2Status[i] = E2Update % 10;
			E2Update /= 10;
		}

		//check for change in packet
		for (int i = 4; i >= 0; i--) {
			if (previousE1Packet[i] != E1Status[i]) {
				flagE1 = 1;
			}
			if (previousE2Packet[i] != E2Status[i]) {
				flagE2 = 1;
			}
		}

		//execute animation if new packet
		if (flagE1 == 1) {

			console.Wait();

			//Update Elevator status bar in realtime
			animationUpdateStatusBar(E1Status);

			//Animate elevator in relatime
			animationElevator(E1Status);

			console.Signal();

			//Terminate thread if E stop pressed
			if (E1Status[2] == 9) {
				animationUpdateStatusBar(E2Status);
				animationElevator(E2Status);
				flagE1 = 0;
				break;
			}

			//reset flag
			flagE1 = 0;

			//reset cursor to data entry position
			MOVE_CURSOR(24, INPUT_TOPLEFT_Y); 

		}

		if (flagE2 == 1) {

			console.Wait();

			//Update Elevator status bar in realtime
			animationUpdateStatusBar(E2Status);

			//Animate elevator in relatime
			animationElevator(E2Status);

			console.Signal();

			//Terminate thread if E stop pressed
			if (E2Status[2] == 9) {
				animationUpdateStatusBar(E1Status);
				animationElevator(E1Status);
				flagE2 = 0;
				break;
			}

			//reset flag
			flagE2 = 0;

			//reset cursor to data entry position
			MOVE_CURSOR(24, INPUT_TOPLEFT_Y);

		}

		//Copy current packet into previous packet for comparison next loop
		for (int i = 4; i >= 0; i--) {
			previousE1Packet[i] = E1Status[i];
			previousE2Packet[i] = E2Status[i];
		}
		
	}

	return 0;
}

UINT __stdcall UserInput(void* args) {

	int flag = 0; //E Stop flag to end thread

	while (1) {
		inputB4output2.Wait();
		flag = getUserInput((char*)args);
		inputB4output1.Signal();

		if(flag == 9) {
			break;
		}

	}

	return 0;
}


int main() {
	// Rendezvous to start
	r1.Wait();
	cout << "Elevator\tDirection\tService Status\t\tDoor status\t\tFloor" << endl;
	
	char input[3] = {};
	char inputPassenger[3] = {};

	int flag = 0; //E Stop flag to end thread

	CThread t1(UpdateDisplay, ACTIVE, NULL);
	CThread t2(UserInput, ACTIVE, input);

	while (1) {

		//check for passenger input and send that if so, otherwise send normal user input data
		if (PipeIOPassenger.TestForData() >= sizeof(pipePassengerData) / 3) { // size of struct is 3
			PipeIOPassenger.Read(&pipePassengerData);
			strcpy(inputPassenger, pipePassengerData.inputs);
			inputB4output1.Wait();
			flag = dispatchPipeline(inputPassenger);
			inputB4output2.Signal();
		}
		else {
			inputB4output1.Wait();
			flag = dispatchPipeline(input);
			inputB4output2.Signal();
		}

		//E Stop to terminate main() and close IO
		if (flag == 9) {
			break;
		}
		
	}

	t1.WaitForThread();
	t2.WaitForThread();

	getchar();

	return 0;
}


int dispatchPipeline(char* userInput) {
	CTypedPipe <IODispatch> PipeIODispatch("PipelineIODispatch", 100);

	IODispatch dispatch;
	strcpy(dispatch.inputs, userInput); // copy input to struct, change to receive user input later

	PipeIODispatch.Write(&dispatch);

	if (userInput[1] == 'e')
	{
		return 9;
	}
	else {
		return 0;
	}
}

int getUserInput(char* input) {

	console.Wait();
	MOVE_CURSOR(0, INPUT_TOPLEFT_Y);
	cout << "Enter a valid command:\t     ";
	MOVE_CURSOR(0, INPUT_TOPLEFT_Y);
	cout << "Enter a valid command:\t"; //reprint to get curser back to being next to semicolon
	MOVE_CURSOR(0, INPUT_TOPLEFT_Y + 1);
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

	Sleep(750); //Delay time for user input to display


	//terminate thread if E STOP
	if (input[1] == 'e') {
		return 9;
	}
	else {
		return 0;
	}
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

	int i = 0; 
	int flag = 0;

	while (i < 2) {
		
		if (status[2] == 0) { //out of service
			i = 2;
		}

		if (status[0] == 1) {
			MOVE_CURSOR(E1_TOPLEFT_X, ANIMATION_TOPLEFT_Y + NUM_FLOORS - 1 - status[4]);
		}
		if (status[0] == 2) {
			MOVE_CURSOR(E2_TOPLEFT_X, ANIMATION_TOPLEFT_Y + NUM_FLOORS - 1 - status[4]);
		}
		
		if (status[2] == 9) { //E STOP
			SetConsoleTextAttribute(hConsole, RED);
			console.Wait();
			cout << "[  X  ]";
			console.Signal();
			SetConsoleTextAttribute(hConsole, WHITE);
			i = 2;
		}

		if (status[2] == 1) { //if in service: display door open or closed

			if (status[3] == 0) { //door closed
				if (i == 0 && status[1] == 0) { //checks also if elevator stopped

					SetConsoleTextAttribute(hConsole, YELLOW);
					console.Wait();
					cout << "[ | | ]";
					console.Signal();
					Sleep(ELEVATOR_DOOR_DELAY);
					i++;
				}
				else {
					SetConsoleTextAttribute(hConsole, YELLOW);
					console.Wait();
					cout << "[  |  ]";
					console.Signal();
					i++;
				}
			}
			if (status[3] == 1) { //door open
				SetConsoleTextAttribute(hConsole, YELLOW);
				if (i == 0 && status[1] == 0) {
					
					console.Wait();
					cout << "[ | | ]";
					console.Signal();
					Sleep(ELEVATOR_DOOR_DELAY);
					i++;
				}
				else {
					SetConsoleTextAttribute(hConsole, GREEN);
					console.Wait();
					cout << "[|   |]";
					console.Signal();
					i++;

					//Play sound
					PlaySound(TEXT("..\\sounds\\bell1.wav"), NULL, SND_ASYNC);

				}
			}
			
		}
		
		SetConsoleTextAttribute(hConsole, WHITE);
	}
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

	//Set row to write status
	if (status[0] == 1) { //E1
		MOVE_CURSOR(0, status[0]);
		cout << "E1		";
	}
	if (status[0] == 2) { //E2
		MOVE_CURSOR(0, status[0]); 
		cout << "E2		";
	}

		switch (status[1]) {
		case 2:
			cout << "Up		";
			break;
		case 1:
			cout << "Down		";
			break;
		case 0:
			cout << "Not moving	";
			break;
		}

		switch (status[2]) {
		case 0:
			cout << "Out of service	";
			break;
		case 1:
			cout << "In service		";
			break;
		case 9: 
			SetConsoleTextAttribute(hConsole, RED);
			cout << "E-STOP			";
			SetConsoleTextAttribute(hConsole, WHITE);
			break;
		}
		
		switch (status[3]) {
		case 0:
			cout << "Closed			";
			break;
		case 1:
			cout << "Open			";
			break;
		}

		cout << status[4];
	

	console.Signal();
}

void animationCurrentDirection(int* status) {
	console.Wait();

	int offset = 0;
	int color = 0;
	string string = "";

	if (status[0] == 1) { //E1
		offset = 0;
	}
	if (status[0] == 2) { //E2
		offset = 8;
	}

	if (status[1] == 1) { //If going down
		string = "   DOWN   ";
		color = YELLOW;
	}
	if (status[1] == 2) { //If going up
		string = "    UP    ";
		color = GREEN;
	}
	if (status[1] == 0) { //If stopped
		string = "          ";
	}
	if (status[2] == 0) {
		string = "NO SERVICE";
		color = RED;
	}
	if (status[2] == 9) {
		string = "~E-STOP~ ";
		color = RED;
	}

	//Write text
	MOVE_CURSOR(ANIMATION_TOPLEFT_X + offset, ANIMATION_TOPLEFT_Y - 2);
	SetConsoleTextAttribute(hConsole, color);
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
