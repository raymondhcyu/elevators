#include "D:\Documents\CPEN333\Assignments\CPEN333Assignment1\rt.h"

CRendezvous r1("CreationRendezvous", 3); // sync creation of 4x processes
CRendezvous r2("TerminationRendezvous", 3); // sync deletion of 4x processes

struct monitor {
	int moveStatus; // 1 up, -1 down, 0 stationary
	int doorStatus; // opening, closing, closed, open
	int floor; // which floor it is on; European convention
};

int main() {
	int i;
	for (i = 0; i < 10; i++) {
		cout << "Helloo " << i << " from elevator process 2..." << endl;
		Sleep(50);
	}

	Sleep(4000);
	r1.Wait();
	cout << "Elevator process 2 is done waiting" << endl;

	CDataPool dp2("Monitor2", sizeof(struct monitor));
	struct monitor* monitor2 = (struct monitor*)(dp2.LinkDataPool());

	cout << "Elevator 2 is on floor " << monitor2->floor << endl;

	getchar();
	return 0;
}