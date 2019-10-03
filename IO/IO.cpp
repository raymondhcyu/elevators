#include "D:\Documents\CPEN333\Assignments\CPEN333Assignment1\rt.h"

struct monitor {
	int moveStatus; // 1 up, -1 down, 0 stationary
	int doorStatus; // opening, closing, closed, open
	int floor; // which floor it is on; European convention
};

int main() {
	int i;
	for (i = 0; i < 10; i++) {
		cout << "Helloo " << i << " from IO..." << endl;
		Sleep(50);
	}
	CDataPool dp1("Monitor1", sizeof(struct monitor));
	CDataPool dp2("Monitor2", sizeof(struct monitor));

	struct monitor* monitor1 = (struct monitor*)(dp1.LinkDataPool());
	struct monitor* monitor2 = (struct monitor*)(dp2.LinkDataPool());

	cout << "From IO, elevator 1 is on floor " << monitor1->floor << endl;
	cout << "From IO, elevator 2 is on floor " << monitor2->floor << endl;

	getchar();
	return 0;
}