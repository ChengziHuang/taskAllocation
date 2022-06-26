#ifndef task_processor_h
#define task_processor_h

#include <string>
#include <vector>
#include <iostream>
#include <set>
#include <algorithm>
#include <functional>
#include <climits>
#include <cfloat>
#include <memory>
#include <list>
using namespace std;

class processor {
public:
	string name;
	double speed;

	processor(string name, double speed) {
		this->name = name;
		this->speed = speed;
	}
};

class task {
public:
	string name;
	string criticality;
	double deadline;
	double C_N;
	double C_O;
	double L_N;
	double L_O;
	double elastic_degree;

	task(task* t) {
		this->name = t->name;
		this->criticality = t->criticality;
		this->deadline = t->deadline;
		this->C_N = t->C_N;
		this->C_O = t->C_O;
		this->L_N = t->L_N;
		this->L_O = t->L_O;
		this->elastic_degree = t->elastic_degree;
	}

	task(string name, string criticality, double deadline, double C_N, double C_O, double L_N, double L_O, double elastic_degree) {
		this->name = name;
		this->criticality = criticality;
		this->deadline = deadline;
		this->C_N = C_N;
		this->C_O = C_O;
		this->L_N = L_N;
		this->L_O = L_O;
		this->elastic_degree = elastic_degree;
	}
};

class uniprocessor {
public:
	string name;
	double speed;
	vector<task*> host;

	double U_N;
	double U_O;

	uniprocessor(processor p) {
		this->name = p.name;
		this->speed = p.speed;
		this->U_N = 0.0;
		this->U_O = 0.0;
	}
	bool test_and_add(task* t);
	void print();
};

void uniprocessor::print() {
	cout << "Allocate ";
	for (int i = 0; i < host.size(); i++) {
		cout << host[i]->name << " ";
	}
	cout << "on the " << name << " with speed " << speed << ". The utilization on this processor is " << max(U_N, U_O) << "." << endl;
}


bool uniprocessor::test_and_add(task* t) {
	double new_U_N = U_N + t->C_N / (t->deadline * speed);
	double new_U_O = U_O + t->C_O / (t->deadline * speed * t->elastic_degree);
	if (new_U_N <= 0.75 && new_U_O <= 0.75) {
		host.push_back(t);
		U_N = new_U_N;
		U_O = new_U_O;
		return true;
	}
	return false;

}


class cluster {
public:
	list<processor> processor_list;
	double capacity;
	double uniformaity;

	cluster();
	cluster(cluster* c);
	void add(processor p);
	void del(processor p);
	void print();
	int size();
	double get_capacity();
	double get_uniformaity();
	bool find(processor target);
};

bool cluster::find(processor target) {
	for (list<processor>::iterator it = processor_list.begin(); it != processor_list.end(); ++it) {
		if (it->name == target.name) {
			return true;
		}
	}
	return false;
}

cluster::cluster(cluster* c) {
	this->capacity = c->capacity;
	this->uniformaity = c->uniformaity;
	this->processor_list = c->processor_list;
}


cluster::cluster() {
	capacity = 0.0;
	uniformaity = 0.0;
}

double cluster::get_capacity() {
	return capacity;
}

double cluster::get_uniformaity() {
	return uniformaity;
}

int cluster::size() {
	return processor_list.size();
}

void cluster::add(processor p) {
	list<processor>::iterator it = processor_list.begin();
	if (processor_list.size() == 0) {
		processor_list.insert(it, p);
	}
	else {

		for (; it != processor_list.end(); ++it) {
			if (p.speed > it->speed) {
				break;
			}
		}

		processor_list.insert(it, p);
	}

	capacity += p.speed;

	double lambda = 0.0;
	double s_x = 0.0;
	double lambda_tmp;
	for (it = processor_list.begin(); it != processor_list.end(); ++it) {
		s_x += it->speed;
		lambda_tmp = (capacity - s_x) / (it->speed);
		if (lambda_tmp > lambda)
			lambda = lambda_tmp;
	}
	uniformaity = lambda;
}

void cluster::del(processor p) {
	list<processor>::iterator it = processor_list.begin();

	for (; it != processor_list.end(); ++it) {
		if (it->name == p.name) {
			break;
		}
	}

	if (it == processor_list.end()) {
		return;
	}
	processor_list.erase(it);

	capacity -= p.speed;

	double lambda = 0.0;
	double s_x = 0.0;
	double lambda_tmp;
	for (it = processor_list.begin(); it != processor_list.end(); ++it) {
		s_x += it->speed;
		lambda_tmp = (capacity - s_x) / (it->speed);
		if (lambda_tmp > lambda)
			lambda = lambda_tmp;
	}
	uniformaity = lambda;
}

void cluster::print() {

	for (list<processor>::iterator it = processor_list.begin(); it != processor_list.end(); ++it) {
		cout << it->name << " " << it->speed << " ";
	}
	cout << endl;
}

class heavy_task{
public:
	string name;
	string criticality;
	double deadline;
	double C_N;
	double C_O;
	double L_N;
	double L_O;
	double elastic_degree;
	cluster* cluster_little; // the cluster assigned to LO-task at HI criticality mode, or HI-task at LO criticality mode 
	cluster* cluster_delta; // the delta_m

	heavy_task(heavy_task* t) {
		this->name = t->name;
		this->criticality = t->criticality;
		this->deadline = t->deadline;
		this->C_N = t->C_N;
		this->C_O = t->C_O;
		this->L_N = t->L_N;
		this->L_O = t->L_O;
		this->elastic_degree = t->elastic_degree;

		cluster_little = new cluster(t->cluster_little);
		cluster_delta = new cluster(t->cluster_delta);
		
	}
	heavy_task(string name, string criticality, double deadline, double C_N, double C_O, double L_N, double L_O, double elastic_degree) {
		this->name = name;
		this->criticality = criticality;
		this->deadline = deadline;
		this->C_N = C_N;
		this->C_O = C_O;
		this->L_N = L_N;
		this->L_O = L_O;
		this->elastic_degree = elastic_degree;
		cluster_little = new cluster();
		cluster_delta = new cluster();
	}

	
	double response_time();
	double response_time_low();
	void print();
};


double heavy_task::response_time() {
	if (cluster_little->size() == 0 || cluster_delta->size() == 0)
		return FLT_MAX;
	double time;
	if (criticality == "HI") {
		// merge c^N and c^O
		cluster cluster_true_O;

		for (list<processor>::iterator it = cluster_little->processor_list.begin(); it != cluster_little->processor_list.end(); ++it) {
			cluster_true_O.add(*it);
		}
		for (list<processor>::iterator it = cluster_delta->processor_list.begin(); it != cluster_delta->processor_list.end(); ++it) {
			cluster_true_O.add(*it);
		}
		double uniformaity_N = cluster_little->get_uniformaity();
		double capacity_N = cluster_little->get_capacity();

		double uniformaity_O = cluster_true_O.get_uniformaity();
		double capacity_O = cluster_true_O.get_capacity();
		
		time = (C_N + uniformaity_N * L_N) * (1 / capacity_N - 1 / capacity_O)
			+ (C_O + uniformaity_O * L_O) / capacity_O;
	}
	else {

		cluster cluster_true_N;

		for (list<processor>::iterator it = cluster_little->processor_list.begin(); it != cluster_little->processor_list.end(); ++it) {
			cluster_true_N.add(*it);
		}
		for (list<processor>::iterator it = cluster_delta->processor_list.begin(); it != cluster_delta->processor_list.end(); ++it) {
			cluster_true_N.add(*it);
		}

		double uniformaity_N = cluster_true_N.get_uniformaity();
		double capacity_N = cluster_true_N.get_capacity();

		double capaciy_O = cluster_little->get_capacity();

		double denominator;
		double choose1 = this->elastic_degree * capaciy_O;
		double choose2 = capacity_N;

		denominator = choose1 > choose2 ? choose2 : choose1;
		

		time = (C_N + L_N * uniformaity_N) / denominator;
	}
	return time;
}

double heavy_task::response_time_low() {
	if (cluster_little->size() == 0) {
		return FLT_MAX;
	}
	double time;
	double uniformaity_little = cluster_little->get_uniformaity();
	double capacity_N = cluster_little->get_capacity();
	
	time = (C_N + L_N * uniformaity_little) / capacity_N;
	return time;
}

void heavy_task::print() {
	cout << name << ": ";
	cout << criticality << endl;
	if (criticality == "HI") {

		cout << "Low criticality mode: ";
		cluster_little->print();
		cout << "High criticality mode(add): ";
		cluster_delta->print();
	}
	else {
		cout << "High criticality mode: ";
		cluster_little->print();
		cout << "LO criticality mode(add): ";
		cluster_delta->print();
	}
}

#endif