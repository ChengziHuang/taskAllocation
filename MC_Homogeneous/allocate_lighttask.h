#ifndef allocate_lighttask
#define allocate_lighttask

#include "io.h"

bool MC_partition(vector<task*> taskset, vector<uniprocessor*> processorset);

bool MC_partition(vector<task*> taskset, vector<uniprocessor*> processorset) {
	// Algorithm 4
	// Mixed-criticality scheduling on multiprocessors
	for (int i = 0; i < taskset.size(); i++) {
		if (taskset[i]->criticality == "HI") {
			int j = 0;

			while (true) {
				if (processorset[j]->test_and_add(taskset[i]) == true) {
					break;
				}
				j++;
				if (j >= processorset.size()) {
					return false;
				}
			}
		}
	}
	for (int i = 0; i < taskset.size(); i++) {
		if (taskset[i]->criticality == "LOW") {
			int j = 0;

			while (true) {
				if (processorset[j]->test_and_add(taskset[i]) == true) {
					break;
				}
				j++;
				if (j >= processorset.size()) {
					return false;
				}
			}
		}
	}
	return true;
}

#endif // !allocate_lighttask
