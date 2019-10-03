#include "D:\Documents\CPEN333\Assignments\CPEN333Assignment1\rt.h"

void initializeProcesses();
void initializeMonitors();

struct monitor {
	int moveStatus; // 1 up, -1 down, 0 stationary
	int doorStatus; // opening, closing, closed, open
	int floor; // which floor it is on; European convention
};

int main(void) {
	cout << "Dispatcher running..." << endl;

	initializeMonitors();

	initializeProcesses();

	cout << "Dispatcher complete..." << endl;
	return 0;
}

void initializeProcesses() {
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
	// Elevator 3 child process
	CProcess p3("D:\\Documents\\CPEN333\\Assignments\\CPEN333Assignment1\\Debug\\IO.exe",
		NORMAL_PRIORITY_CLASS,
		OWN_WINDOW,
		ACTIVE
	);

	cout << "Elevator 1, 2, and IO activated..." << endl;
	cout << "Waiting for self-made child process 1 to terminate..." << endl;
	p1.WaitForProcess();
	cout << "Waiting for self-made child process 2 to terminate..." << endl;
	p2.WaitForProcess();
	cout << "Waiting for self-made child process 3 to terminate..." << endl;
	p3.WaitForProcess();
}

void initializeMonitors() {
	CDataPool dp1("Monitor1", sizeof(struct monitor));
	CDataPool dp2("Monitor2", sizeof(struct monitor));

	struct monitor* monitor1 = (struct monitor*)(dp1.LinkDataPool());
	struct monitor* monitor2 = (struct monitor*)(dp2.LinkDataPool());

	cout << "Parent datapool linked at address " << monitor1 << endl;

	monitor1->floor = 0;
	monitor1->moveStatus = 0;
	monitor1->doorStatus = 0;
	monitor2->floor = 0;
	monitor2->moveStatus = 0;
	monitor2->doorStatus = 0;

	getchar();
}