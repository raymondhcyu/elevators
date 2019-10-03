#include "D:\Documents\CPEN333\Assignments\CPEN333Assignment1\rt.h"

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

	CDataPool dp2("Monitor2", sizeof(struct monitor));
	struct monitor* monitor2 = (struct monitor*)(dp2.LinkDataPool());

	cout << "Elevator 2 is on floor " << monitor2->floor << endl;

	getchar();
	return 0;
}