#ifndef alocate_light_task_h
#define alocate_light_task_h

//#define PRINT_F
//#define PRINT

#include "task_processor.h"
#include "generate_random.h"
#include <algorithm>
#include <vector>
#include <random>       
#include <chrono>
#include <tuple>
#include <cmath>

using namespace std;

bool decresing_utilisation(task* t1, task* t2);
bool incresing_utilisation(task* t1, task* t2);
bool decreasing_criticality(task* t1, task* t2);
bool increasing_criticality(task* t1, task* t2);
bool increasing_order(uniprocessor* p1, uniprocessor* p2);
bool decreasing_order(uniprocessor* p1, uniprocessor* p2);
bool best_fit(uniprocessor* p1, uniprocessor* p2);
bool worst_fit(uniprocessor* p1, uniprocessor* p2);
bool dresing_speed(double s1, double s2);
double quality_of_partition(vector<uniprocessor*> p_success);
tuple<vector<uniprocessor*>, double> partitioned_framework(vector<processor> processorset, vector<task*> taskset);
double temperature_function(int k, int k_max);
vector<uniprocessor*> allocate_light_task(vector<processor> processorset, vector<task*> taskset);


bool dresing_speed(double s1, double s2) {
	return s1 > s2;
}


bool decresing_utilisation(task* t1, task* t2) {
	/*
	* The tasks are ordered by decreasing utilisation
	*/
	double u1 = t1->C_O / t1->deadline;
	double u2 = t2->C_O / t2->deadline;
	return u1 > u2;
}


bool incresing_utilisation(task* t1, task* t2) {
	/*
	* The tasks are ordered by increasing utilisation
	*/
	double u1 = t1->C_O / t1->deadline;
	double u2 = t2->C_O / t2->deadline;
	return u1 < u2;
}


bool decreasing_criticality(task* t1, task* t2) {
	/*
	* The tasks are ordered according to decreasing criticality level.
	* In the case of equal criticality levels, decreasing utilisation isused.
	*/
	if (t1->criticality == "HI" && t2->criticality == "LOW") {
		return true;
	}
	else {
		if (t1->criticality == "LOW" && t2->criticality == "HI") {
			return false;
		}
		else {
			double u1 = t1->C_O / t1->deadline;
			double u2 = t2->C_O / t2->deadline;
			return u1 > u2;
		}
	}
}


bool increasing_criticality(task* t1, task* t2) {
	/*
	* The tasks are ordered according to the increasingcriticality level.
	* In case of equal criticality levels, increasing utilisation is used.
	*/
	if (t1->criticality == "HI" && t2->criticality == "LOW") {
		return false;
	}
	else {
		if (t1->criticality == "LOW" && t2->criticality == "HI") {
			return true;
		}
		else {
			double u1 = t1->C_O / t1->deadline;
			double u2 = t2->C_O / t2->deadline;
			return u1 < u2;
		}
	}
}


bool increasing_order(uniprocessor* p1, uniprocessor* p2) {
	return p1->speed < p2->speed;
}


bool decreasing_order(uniprocessor* p1, uniprocessor* p2) {
	return p1->speed > p2->speed;
}


bool best_fit(uniprocessor* p1, uniprocessor* p2) {
	return p1->U_O > p2->U_O;
}

bool worst_fit(uniprocessor* p1, uniprocessor* p2) {
	return p1->U_O < p2->U_O;
}


double quality_of_partition(vector<uniprocessor*> p_success) {
	double tmp = 0.0;
	int number_task = 0;
	for (int i = 0; i < p_success.size(); i++) {
		tmp = tmp + (max(p_success[i]->U_N, p_success[i]->U_O) / 0.75);
		number_task += p_success[i]->host.size();
	}
	return tmp / p_success.size() * number_task;
}


double temperature_function(int k, int k_max) {
	return 1.0 - (k + 1) / k_max;
}

tuple<vector<uniprocessor*>, double> partitioned_framework(vector<processor> processorset, vector<task*> taskset) {
	bool (*sort_task[])(task*, task*) = { decresing_utilisation, incresing_utilisation , decreasing_criticality , increasing_criticality };
	bool (*sort_processor[])(uniprocessor*, uniprocessor*) = { increasing_order, decreasing_order };
	bool (*sort_processor1[])(uniprocessor*, uniprocessor*) = { best_fit, worst_fit };
	double quality = 0.0;

	vector<uniprocessor*> p_success;
	vector<uniprocessor*> tmp_processor;
	p_success.reserve(processorset.size());

	if (processorset.size() == 0) {
		return make_tuple(p_success, 0.0);
	}



	for (int i = 0; i < 4; i++) {
		sort(taskset.begin(), taskset.end(), sort_task[i]);
		for (int j = 0; j < 2; j++) {
			// for each policy

			vector<uniprocessor*> attempt;
			attempt.reserve(processorset.size());
			for (int p = 0; p < processorset.size(); p++) {
				attempt.push_back(new uniprocessor(processorset[p]));
			}
			sort(attempt.begin(), attempt.end(), sort_processor[j]);

			for (int t = 0; t < taskset.size(); t++) {
				for (int p = 0; p < processorset.size(); p++) {
					bool test = attempt[p]->test_and_add(taskset[t]);
					if (test == true) {
						break;
					}
				}
			}

			tmp_processor.clear();
			for (int p = 0; p < processorset.size(); p++) {
				if (attempt[p]->host.size() != 0) {
					tmp_processor.push_back(attempt[p]);
				}
			}

			double qop = quality_of_partition(tmp_processor);
			if (qop > quality) {
				quality = qop;
				p_success = tmp_processor;
			}
		}

		// best fit and worst fit
		for (int j = 0; j < 2; j++) {
			vector<uniprocessor*> attempt;
			attempt.reserve(processorset.size());
			for (int p = 0; p < processorset.size(); p++) {
				attempt.push_back(new uniprocessor(processorset[p]));
			}

			for (int t = 0; t < taskset.size(); t++) {
				sort(attempt.begin(), attempt.end(), sort_processor1[j]);
				for (int p = 0; p < processorset.size(); p++) {
					bool test = attempt[p]->test_and_add(taskset[t]);
					if (test == true) {
						break;
					}
				}
			}

			tmp_processor.clear();
			for (int p = 0; p < processorset.size(); p++) {
				if (attempt[p]->host.size() != 0) {
					tmp_processor.push_back(attempt[p]);
				}
			}
			double qop = quality_of_partition(tmp_processor);
			if (qop > quality) {
				quality = qop;
				p_success = tmp_processor;
			}
		}
	}

	// rand
	unsigned seed = chrono::system_clock::now().time_since_epoch().count();
	default_random_engine e(seed);
	shuffle(taskset.begin(), taskset.end(), e);

	for (int j = 0; j < 2; j++) {
		vector<uniprocessor*> attempt;
		attempt.reserve(processorset.size());
		for (int p = 0; p < processorset.size(); p++) {
			attempt.push_back(new uniprocessor(processorset[p]));
		}
		sort(attempt.begin(), attempt.end(), sort_processor[j]);

		for (int t = 0; t < taskset.size(); t++) {
			for (int p = 0; p < processorset.size(); p++) {
				bool test = attempt[p]->test_and_add(taskset[t]);
				if (test == true) {
					break;
				}
			}
		}

		tmp_processor.clear();
		for (int p = 0; p < processorset.size(); p++) {
			if (attempt[p]->host.size() != 0) {
				tmp_processor.push_back(attempt[p]);
			}
		}

		double qop = quality_of_partition(tmp_processor);
		if (qop > quality) {
			quality = qop;
			p_success = tmp_processor;
		}
	}

	// best fit and worst fit
	for (int j = 0; j < 2; j++) {
		vector<uniprocessor*> attempt;
		attempt.reserve(processorset.size());
		for (int p = 0; p < processorset.size(); p++) {
			attempt.push_back(new uniprocessor(processorset[p]));
		}

		for (int t = 0; t < taskset.size(); t++) {
			sort(attempt.begin(), attempt.end(), sort_processor1[j]);
			for (int p = 0; p < processorset.size(); p++) {
				bool test = attempt[p]->test_and_add(taskset[t]);
				if (test == true) {
					break;
				}
			}
		}

		tmp_processor.clear();
		for (int p = 0; p < processorset.size(); p++) {
			if (attempt[p]->host.size() != 0) {
				tmp_processor.push_back(attempt[p]);
			}
		}

		double qop = quality_of_partition(tmp_processor);
		if (qop > quality) {
			quality = qop;
			p_success = tmp_processor;
		}
	}

	
	return make_tuple(p_success, quality);
}


vector<uniprocessor*> allocate_light_task(vector<processor> processorset, vector<task*> taskset) {
	vector<processor> use(processorset);
	vector<processor> unused;
	vector<uniprocessor*> p_success;
	double quality;
	int test_number = max((int)pow(2, processorset.size() - 2), (int)processorset.size() * 2);

#ifdef PRINT
	cout << test_number << endl;
#endif
	// initial solution
	tie(p_success, quality) = partitioned_framework(use, taskset);
	double best_quality = quality;

#ifdef PRINT
	cout << "The processor set used for partitoned scheduling: ";
	for (int i = 0; i < use.size(); i++) {
		cout << use[i].name << " "<< use[i].speed << " ";
	}
	cout << endl;
	cout << "Size: ";
	cout << use.size() << endl;
	cout << "Quality: ";
	cout << quality << endl;
#endif 

	int count = 0;
	while (count < test_number){
		int rule = generate_random_number(0, 1);
		if (rule == 1) {
			// add processor
			if (unused.size() == 0) {
				continue;
			}
			vector<processor>::iterator index = select_randomly(unused.begin(), unused.end());
			vector<processor> neighbor(use);
			neighbor.push_back(*index);

			vector<uniprocessor*> p_local;
			double quality_local;
			tie(p_local, quality_local) = partitioned_framework(neighbor, taskset);

			if (quality_local > quality) {
				
				if (quality_local > best_quality) {
					p_success = p_local;
					best_quality = quality_local;
				}
				quality = quality_local;

				use = neighbor;
				unused.erase(index);

#ifdef PRINT
				cout << "The processor set used for partitoned scheduling: ";
				for (int i = 0; i < use.size(); i++) {
					cout << use[i].name << " " << use[i].speed << " ";
				}
				cout << endl;
				cout << "Size: ";
				cout << use.size() << endl;
				cout << "Quality: ";
				cout << quality << endl;
#endif 

			}
			else {
				double x = (quality_local - quality) / temperature_function(count, test_number);
				if (exp(x) >= generate_random_01()) {
					quality = quality_local;

					use = neighbor;
					unused.erase(index);

#ifdef PRINT
					cout << "With a small probablity, accept a bad solution.\nThe processor set used for partitoned scheduling: ";
					for (int i = 0; i < use.size(); i++) {
						cout << use[i].name << " " << use[i].speed << " ";
					}
					cout << endl;
					cout << "Size: ";
					cout << use.size() << endl;
					cout << "Quality: ";
					cout << quality << endl;
#endif 



				}
			}
			count++;
		}
		else {
			// delete processor
			if (use.size() == 0) {
				continue;
			}

			
			vector<processor> neighbor(use);
			vector<processor>::iterator index = select_randomly(neighbor.begin(), neighbor.end());
			processor tmp = *index;
			neighbor.erase(index);

			vector<uniprocessor*> p_local;
			double quality_local;
			tie(p_local, quality_local) = partitioned_framework(neighbor, taskset);

			if (quality_local > quality) {
				if (quality_local > best_quality) {
					p_success = p_local;
					best_quality = quality_local;
				}
				quality = quality_local;

				use = neighbor;
				unused.push_back(tmp);
#ifdef PRINT
				cout << "The processor set used for partitoned scheduling: ";
				for (int i = 0; i < use.size(); i++) {
					cout << use[i].name << " " << use[i].speed << " ";
				}
				cout << endl;
				cout << "Size: ";
				cout << use.size() << endl;
				cout << "Quality:" << endl;
				cout << quality << endl;
#endif 
			}
			else {
				double x = (quality_local - quality) / temperature_function(count, test_number);
				if (exp(x) >= generate_random_01()) {
					quality = quality_local;

					use = neighbor;
					unused.push_back(tmp);
#ifdef PRINT
					cout << "With a small probablity, accept a bad solution.\nThe processor set used for partitoned scheduling: ";
					for (int i = 0; i < use.size(); i++) {
						cout << use[i].name << " " << use[i].speed << " ";
					}
					cout << endl;
					cout << "Size: ";
					cout << use.size() << endl;
					cout << "Quality:" << endl;
					cout << quality << endl;
#endif 
				}
			}
			count++;
		}
	}

#ifdef PRINT_F
	cout << "Final quality: ";
	cout << quality_of_partition(p_success) << endl;
	for (int i = 0; i < p_success.size(); i++) {
		p_success[i]->print();
	}
#endif 

	return p_success;
}

#endif // partitioned_scheduling_h
