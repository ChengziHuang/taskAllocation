#include "allocation.h"

using namespace std;

//#define PRINT

int main(int argc, char* argv[])
{
	string path(argv[1]);

	vector<processor> processorset;
	vector<task*> taskset;
	// read
	tie(processorset, taskset) = read(path);
#ifdef PRINT
	for (int i = 0; i < processorset.size(); i++) {
		cout << processorset[i].name << " " << processorset[i].speed << endl;
	}

	for (int i = 0; i < taskset.size(); i++) {
		cout << taskset[i]->name << " " << taskset[i]->criticality << " " << taskset[i]->deadline << " " << taskset[i]->C_N << " "
			<< taskset[i]->C_O << " " << taskset[i]->L_N << " " << taskset[i]->L_O << endl;
	}
#endif
	
	// allocate light task
	vector<uniprocessor*> result1 = allocate_light_task(processorset, taskset);
	
	// get remain
	vector<heavy_task*> heavyset;
	tie(processorset, heavyset) = get_remain(processorset, taskset, result1);
#ifdef PRINT

	cout << "remain: " << endl;
	cout << "processorset size: " << processorset.size() << endl;
	for (int i = 0; i < processorset.size(); i++) {
		cout << processorset[i].name << " " << processorset[i].speed << " ";
	}
	cout << endl;
	for (int i = 0; i < heavyset.size(); i++) {
		cout << heavyset[i]->name << " ";
	}
	cout << endl;
#endif
	
	if (heavyset.size() == 0) {
		cout << "success" << endl;
		return 0;
	}

	if (heavyset.size() > 0 && processorset.size() == 0) {
		cout << "failure" << endl;
		return 0;
	}

	if (heavyset.size() == 1) {

		// simulated annealing need at least two tasks
		cluster tmp;
		for (int i = 0; i < processorset.size(); i++) {
			tmp.add(processorset[i]);
		}
		double uniformity = tmp.get_uniformaity();
		double capacity = tmp.get_capacity();

		double response_time = (taskset[0]->C_O + uniformity * taskset[0]->L_O) / capacity;
		if (response_time <= taskset[0]->deadline) {
			cout << "success" << endl;
			return 0;
		}
		else {
			cout << "failure" << endl;
			return 0;
		}

	}

	if (allocate_heavy_task(processorset, heavyset) == true) {
		cout << "success" << endl;
	}
	else {
		cout << "failure" << endl;
	}



	return 0;
}
