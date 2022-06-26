// MC_Homogeneous.cpp : Defines the entry point for the application.
//

#include "MC_Homogeneous.h"

using namespace std;

int main(int argc, char* argv[])
{
	string path(argv[1]);

	int processor_number;
	vector<task*> taskset;
	tie(processor_number, taskset) = read(path);
	int heavy_number;
	
	vector<task*> light_task;
	tie(heavy_number, light_task) = MCFS(taskset);
	
	if (heavy_number == -1) {
		cout << "failure" << endl;
		return 0;  // unschedulable
	}

	int remain = processor_number - heavy_number;
	if (remain < 0 || (remain == 0 && light_task.size() != 0)) {
		cout << "failure" << endl;
		return 0; // unschedulable
	}
	
	vector<uniprocessor*> processorset;
	processorset.reserve(remain);

	for (int i = 0; i < remain; i++) {
		processorset.push_back(new uniprocessor());
	}

	if (MC_partition(light_task, processorset) == true) {
		cout << "success" << endl;
		return 0;
	}
	else {
		cout << "failure" << endl;
		return 0; // unschedulable
	}
}
