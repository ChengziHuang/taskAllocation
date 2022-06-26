#ifndef allocation_h
#define allocation_h

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <tuple>
#include <set>
#include "task_processor.h"
#include "allocate_light_task.h"
#include "allocate_heavy_task.h"

tuple<vector<processor>, vector<task*>> read(string pathFile);
tuple<vector<processor>, vector<heavy_task*>> get_remain(vector<processor> processorset, vector<task*> taskset, vector<uniprocessor*> p_success);

tuple<vector<processor>, vector<task*>> read(string pathFile) {
	ifstream infile;
	infile.open(pathFile);
	int number_processor, number_task;
	infile >> number_processor >> number_task;

	vector<task*> taskset;
	vector<processor> processorset;

	taskset.reserve(number_task);
	processorset.reserve(number_processor);
	string name;
	double speed;

	for (int i = 0; i < number_processor; ++i) {
		infile >> name;
		infile >> speed;
		processor p(name, speed);
		processorset.push_back(p);
	}

	
	string criticality;
	double deadline, C_N, C_O, L_N, L_O, elastic_degree;
	for (int i = 0; i < number_task; ++i) {
		infile >> name >> criticality >> deadline >> C_N >> C_O >> L_N >> L_O >> elastic_degree;
		taskset.push_back(new task(name, criticality, deadline, C_N, C_O, L_N, L_O, elastic_degree));
	}
	infile.close();
	return make_tuple(processorset, taskset);
}
 

tuple<vector<processor>, vector<heavy_task*>> get_remain(vector<processor> processorset, vector<task*> taskset, vector<uniprocessor*> p_success) {
	if (p_success.size() == 0) {
		vector<heavy_task*> heavyset;
		for (int i = 0; i < taskset.size(); i++) {
			heavyset.push_back(new heavy_task(taskset[i]->name, taskset[i]->criticality, taskset[i]->deadline, taskset[i]->C_N, taskset[i]->C_O, taskset[i]->L_N, taskset[i]->L_O, taskset[i]->elastic_degree));
		}

		return make_tuple(processorset, heavyset);
	}

	int new_number_processor = processorset.size() - p_success.size();
	vector<processor> p_remain;
	p_remain.reserve(new_number_processor);

	set<int> del_p_index;
	set<int> del_t_index;

	for (int i = 0; i < p_success.size(); i++) {

		for (int j = 0; j < p_success[i]->host.size(); j++) {
			for (int k = 0; k < taskset.size(); k++) {
				if (taskset[k]->name == p_success[i]->host[j]->name) {
					del_t_index.insert(k);
					break;
				}
			}
		}

		for (int j = 0; j < processorset.size(); j++) {
			if (del_p_index.find(j) == del_p_index.end() && processorset[j].name == p_success[i]->name) {
				del_p_index.insert(j);
				break;
			}
		}

	}

	int new_number_task = taskset.size() - del_t_index.size();
	vector<heavy_task*> taskset_remain;
	taskset_remain.reserve(new_number_task);

	for (int i = 0; i < processorset.size(); i++) {
		if (del_p_index.find(i) == del_p_index.end()) {
			p_remain.push_back(processorset[i]);
		}
	}

	for (int j = 0; j < taskset.size(); j++) {
		if (del_t_index.find(j) == del_t_index.end()) {
			//taskset_remain.push_back(new heavy_task(taskset[j]));
			taskset_remain.push_back(new heavy_task(taskset[j]->name, taskset[j]->criticality, taskset[j]->deadline, taskset[j]->C_N, taskset[j]->C_O, taskset[j]->L_N, taskset[j]->L_O, taskset[j]->elastic_degree));
		}
	}

	return make_tuple(p_remain, taskset_remain);
}

#endif