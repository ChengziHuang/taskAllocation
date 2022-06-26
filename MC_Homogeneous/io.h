#ifndef io_h
#define io_h

#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <fstream>
#include <algorithm>
using namespace std;

class task {
public:
	string name;
	string criticality;
	double deadline;
	double C_N;
	double C_O;
	double L_N;
	double L_O;

	task(string name, string criticality, double deadline, double C_N, double C_O, double L_N, double L_O) {
		this->name = name;
		this->criticality = criticality;
		this->deadline = deadline;
		this->C_N = C_N;
		this->C_O = C_O;
		this->L_N = L_N;
		this->L_O = L_O;
	}

	task(task* t) {
		this->name = t->name;
		this->criticality = t->criticality;
		this->deadline = t->deadline;
		this->C_N = t->C_N;
		this->C_O = t->C_O;
		this->L_N = t->L_N;
		this->L_O = t->L_O;
	}
};

tuple<int, vector<task*>> read(string pathFile);

tuple<int, vector<task*>> read(string pathFile) {
	ifstream infile;
	infile.open(pathFile);
	int number_processor, number_task;
	infile >> number_processor >> number_task;

	vector<task*> taskset;

	double speed_i;
	for (int i = 0; i < number_processor; i++) {
		infile >> speed_i;
	}

	taskset.reserve(number_task);

	string name;
	string criticality;
	double deadline, C_N, C_O, L_N, L_O;
	double elastic_degree;
	for (int i = 0; i < number_task; ++i) {
		infile >> name >> criticality >> deadline >> C_N >> C_O >> L_N >> L_O >> elastic_degree;
		taskset.push_back(new task(name, criticality, deadline, C_N, C_O, L_N, L_O));
	}
	infile.close();
	return make_tuple(number_processor, taskset);
}


class uniprocessor {
public:
	
	vector<task*> host;

	double U_N;
	double U_O;

	uniprocessor() {
		this->U_N = 0.0;
		this->U_O = 0.0;
	}
	bool test_and_add(task* t);
};




bool uniprocessor::test_and_add(task* t) {

	if (t->criticality == "HI") {
		double new_U_N = U_N + t->C_N / (t->deadline);
		double new_U_O = U_O + t->C_O / (t->deadline);
		if (new_U_N <= 0.75 && new_U_O <= 0.75) {
			host.push_back(t);
			U_N = new_U_N;
			U_O = new_U_O;
			return true;
		}
	}
	else {
		double new_U_N = U_N + t->C_N / (t->deadline);
		if (new_U_N <= 0.75) {
			host.push_back(t);
			U_N = new_U_N;
			return true;
		}
	}
	return false;

}

#endif