#ifndef allocate_heavytask
#define allocate_heavytask

#include "io.h"
#include <cmath>
#include <iostream>

tuple<int, vector<task*>> MCFS(vector<task*> taskset) {
	// table II in "Mixed-Criticality Federated Scheduling for Parallel Real-Time Tasks"
	int number_LO = 0;
	int number_HI = 0;
	double tmp;
	
	vector<task*> light_task;
	double b = 2 + sqrt(2);
	double target = 1 / (b - 1);
	
	for (int i = 0; i < taskset.size(); i++) {
		if (taskset[i]->criticality == "LOW" && ((taskset[i]->C_N / taskset[i]->deadline) > 1)){
			
			if (taskset[i]->L_N >= taskset[i]->deadline) {
				return make_tuple(-1, taskset);
			}

			tmp = (taskset[i]->C_N - taskset[i]->L_N) / (taskset[i]->deadline - taskset[i]->L_N);
			number_LO += (int)ceil(tmp);
			/*
			cout << taskset[i]->name << ": it is a " << taskset[i]->criticality << " criticality task" << "." << endl;
			cout << "It need " << (int)ceil(tmp) << " processors at LO-criticality mode" << endl;
			*/
		} else {
			
			if (taskset[i]->criticality == "HI" && ((taskset[i]->C_N / taskset[i]->deadline) <= target) && (taskset[i]->C_O / taskset[i]->deadline) > 1) {

				double d_v = taskset[i]->deadline / (b - 1);
				if (taskset[i]->L_N >= d_v || taskset[i]->L_O >= taskset[i]->deadline - d_v || (taskset[i]->C_O - d_v - taskset[i]->L_O <= 0)) {
					return make_tuple(-1, taskset);
				}
				number_LO += 1;
				
				tmp = (taskset[i]->C_O - d_v - taskset[i]->L_O) / (taskset[i]->deadline - d_v - taskset[i]->L_O);
				number_HI += (int)ceil(tmp);
				/*
				cout << taskset[i]->name << ": it is a " << taskset[i]->criticality << "criticality task" << "." << endl;
				cout << "It need " << 1 << "processors at LO-criticality mode" << endl;
				cout << "It need " << (int)ceil(tmp) << "processors at HI-criticality mode" << endl;
				*/
			} else {
				if (taskset[i]->criticality == "HI" && (taskset[i]->C_N <= taskset[i]->C_O) && ((taskset[i]->C_N / taskset[i]->deadline) > target) && (taskset[i]->C_O / taskset[i]->deadline) > 1 ) {
					double d_v = 2 * taskset[i]->deadline / b;
					

					tmp = (taskset[i]->C_N - taskset[i]->L_N) / (taskset[i]->deadline - taskset[i]->L_N);
					int n_N = max((int)ceil(tmp), (int)ceil(taskset[i]->C_O / taskset[i]->deadline));
					number_LO += n_N;

					if (taskset[i]->L_N >= d_v || taskset[i]->L_O >= taskset[i]->deadline - d_v || (taskset[i]->C_O - d_v * n_N -taskset[i]->L_O <= 0)) {
						return make_tuple(-1, taskset);
					}

					tmp = (taskset[i]->C_O - n_N * d_v - taskset[i]->L_O) / (taskset[i]->deadline - d_v - taskset[i]->L_O);
					number_HI += max(n_N, (int)ceil(tmp));
					/*
					cout << taskset[i]->name << ": it is a " << taskset[i]->criticality << "criticality task" << "." << endl;
					cout << "It need " << n_N << " processors at LO-criticality mode" << endl;
					cout << "It need " << max(n_N, (int)ceil(tmp)) << " processors at HI-criticality mode" << endl;
					*/
				}
				
				else {
					/*
					cout << taskset[i]->name << ": it is a " << taskset[i]->criticality << "criticality task" << "." << endl;
					cout << "It is a light task" << endl;
					*/
					light_task.push_back(new task(taskset[i]));

				}
			}
		}	
	}
	int number = max(number_LO, number_HI);
	return make_tuple(number, light_task);
}

#endif