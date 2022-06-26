#ifndef allocate_heavy_task_h
#define allocate_heavy_task_h

#include "generate_random.h"
#include "task_processor.h"
#include <vector>
#include <chrono>
#include <random>
#include <cmath>
#include <algorithm>
#include <memory>

using namespace std;

//#define PRINT


bool allocate_heavy_task(vector<processor> processorset, vector<heavy_task*> taskset);
bool decresing_heavy_utilisation(heavy_task* t1, heavy_task* t2);
bool dresing_processor_speed(processor p1, processor p2);
bool local_search(vector<heavy_task*> taskset, vector<int> index_HI, vector<int> index_LO, vector<double>& response_time_result, int count_max);
double temperature_function_heavy(int k, int k_max);
double check(vector<double> response_time_result, vector<heavy_task*> taskset);
double cost_function(double r, double deadline);
double my_function(double r, double deadline);

double cost_function(double r, double deadline) {
	double result = r - deadline;
	if (result < 0) {
		return 0.0;
	}
	else {
		return result;
	}
}

double my_function(double r, double deadline) {
	return abs(r - deadline);
}


double check(vector<double> response_time_result, vector<heavy_task*> taskset) {
	bool result = true;
	for (int i = 0; i < taskset.size(); i++) {
		if (response_time_result[i] > taskset[i]->deadline) {
			result = false;
			break;
		}
	}
	return result;
}


double temperature_function_heavy(int k, int k_max) {
	return 1.0 - (k + 1) / k_max;
}

bool dresing_processor_speed(processor p1, processor p2) {
	return p1.speed > p2.speed;
}

bool local_search(vector<heavy_task*> taskset, vector<int> index_HI, vector<int> index_LO, vector<double>& response_time_result, int count_max) {

	int count = 0;
	while (count < count_max) {
		int rule = generate_random_number(1, 5);
		if (rule == 1) {
			// HI_LO to HI_LO
			if (index_HI.size() <= 1) {
				continue;
			}
			else {
				int i = generate_random_number(0, index_HI.size() - 1);
				int j = generate_random_number(0, index_HI.size() - 1);

				if (i == j || taskset[index_HI[i]]->cluster_N->size() == 0) {
					continue;
				}
				else {
					//cout << rule << endl;
					int index_i = index_HI[i];
					int index_j = index_HI[j];

					processor target = *select_randomly(taskset[index_i]->cluster_N->processor_list.begin(), taskset[index_i]->cluster_N->processor_list.end());

					heavy_task* new_task_i = new heavy_task(taskset[index_i]);
					heavy_task* new_task_j = new heavy_task(taskset[index_j]);


					new_task_i->cluster_N->del(target);
					new_task_j->cluster_N->add(target);

					double r_i = response_time_result[index_i];
					double r_j = response_time_result[index_j];

					double new_r_i = new_task_i->response_time();
					double new_r_j = new_task_j->response_time();

					double delta = -cost_function(r_i, new_task_i->deadline) - cost_function(r_j, new_task_j->deadline)
						+ cost_function(new_r_i, new_task_i->deadline) + cost_function(new_r_j, new_task_j->deadline);
					if (delta < 0) {
						//accept
						response_time_result[index_i] = new_r_i;
						response_time_result[index_j] = new_r_j;
						taskset[index_i] = new_task_i;
						taskset[index_j] = new_task_j;
#ifdef PRINT
						cout << "LO cluster of ";
						cout << new_task_i->name;
						cout << " gives a processor ";
						cout << target.name << " " << target.speed;
						cout << " to LO cluster of ";
						cout << new_task_j->name << endl;

						for (int i = 0; i < taskset.size(); i++) {
							taskset[i]->print();
						}
#endif 
						if (check(response_time_result, taskset)) {
							return true;
						}
					}
					else {
						double x = delta / temperature_function_heavy(count, count_max);
						if (exp(x) > generate_random_01()) {
							response_time_result[index_i] = new_r_i;
							response_time_result[index_j] = new_r_j;
							taskset[index_i] = new_task_i;
							taskset[index_j] = new_task_j;
#ifdef PRINT
							cout << "LO cluster of ";
							cout << new_task_i->name;
							cout << " gives a processor ";
							cout << target.name << " " << target.speed;
							cout << " to LO cluster of ";
							cout << new_task_j->name << endl;

							for (int i = 0; i < taskset.size(); i++) {
								taskset[i]->print();
							}
#endif
							if (check(response_time_result, taskset)) {
								return true;
							}

						}
					}
					count++;
					continue;
				}
			}
		}

		if (rule == 2) {
			// HI_HI to HI_HI
			if (index_HI.size() <= 1) {
				continue;
			}
			else {
				int i = generate_random_number(0, index_HI.size() - 1);
				int j = generate_random_number(0, index_HI.size() - 1);

				if (i == j || taskset[index_HI[i]]->cluster_O->size() == 0) {
					continue;
				}
				else {
					//cout << rule << endl;
					int index_i = index_HI[i];
					int index_j = index_HI[j];

					processor target = *select_randomly(taskset[index_i]->cluster_O->processor_list.begin(), taskset[index_i]->cluster_O->processor_list.end());

					heavy_task* new_task_i = new heavy_task(taskset[index_i]);
					heavy_task* new_task_j = new heavy_task(taskset[index_j]);

					new_task_i->cluster_O->del(target);
					new_task_j->cluster_O->add(target);

					double r_i = response_time_result[index_i];
					double r_j = response_time_result[index_j];

					double new_r_i = new_task_i->response_time();
					double new_r_j = new_task_j->response_time();

					double delta = -cost_function(r_i, new_task_i->deadline) - cost_function(r_j, new_task_j->deadline)
						+ cost_function(new_r_i, new_task_i->deadline) + cost_function(new_r_j, new_task_j->deadline);
					if (delta < 0) {
						//accept
						response_time_result[index_i] = new_r_i;
						response_time_result[index_j] = new_r_j;
						taskset[index_i] = new_task_i;
						taskset[index_j] = new_task_j;
#ifdef PRINT
						cout << "HI cluster of ";
						cout << new_task_i->name;
						cout << " gives a processor ";
						cout << target.name << " " << target.speed;
						cout << " to HI cluster of ";
						cout << new_task_j->name << endl;

						for (int i = 0; i < taskset.size(); i++) {
							taskset[i]->print();
						}
#endif 
						if (check(response_time_result, taskset)) {
							return true;
						}
					}
					else {
						double x = delta / temperature_function_heavy(count, count_max);
						if (exp(x) > generate_random_01()) {
							response_time_result[index_i] = new_r_i;
							response_time_result[index_j] = new_r_j;
							taskset[index_i] = new_task_i;
							taskset[index_j] = new_task_j;
#ifdef PRINT
							cout << "HI cluster of ";
							cout << new_task_i->name;
							cout << " gives a processor ";
							cout << target.name << " " << target.speed;
							cout << " to HI cluster of ";
							cout << new_task_j->name << endl;

							for (int i = 0; i < taskset.size(); i++) {
								taskset[i]->print();
							}
#endif
							if (check(response_time_result, taskset)) {
								return true;
							}

						}
					}
					count++;
					continue;
				}
			}
		}

		if (rule == 3) {
			// LO_LO to LO_LO
			if (index_LO.size() <= 1) {
				continue;
			}
			else {
				int i = generate_random_number(0, index_LO.size() - 1);
				int j = generate_random_number(0, index_LO.size() - 1);

				if (i == j || taskset[index_LO[i]]->cluster_N->size() == 0) {
					continue;
				}
				else {
					//cout << rule << endl;
					int index_i = index_LO[i];
					int index_j = index_LO[j];

					processor target = *select_randomly(taskset[index_i]->cluster_N->processor_list.begin(), taskset[index_i]->cluster_N->processor_list.end());

					heavy_task* new_task_i = new heavy_task(taskset[index_i]);
					heavy_task* new_task_j = new heavy_task(taskset[index_j]);

					new_task_i->cluster_N->del(target);
					new_task_j->cluster_N->add(target);

					double r_i = response_time_result[index_i];
					double r_j = response_time_result[index_j];

					double new_r_i = new_task_i->response_time();
					double new_r_j = new_task_j->response_time();

					double delta = -cost_function(r_i, new_task_i->deadline) - cost_function(r_j, new_task_j->deadline)
						+ cost_function(new_r_i, new_task_i->deadline) + cost_function(new_r_j, new_task_j->deadline);
					if (delta < 0) {
						//accept
						response_time_result[index_i] = new_r_i;
						response_time_result[index_j] = new_r_j;
						taskset[index_i] = new_task_i;
						taskset[index_j] = new_task_j;
#ifdef PRINT
						cout << "LO cluster of ";
						cout << new_task_i->name;
						cout << " gives a processor ";
						cout << target.name << " " << target.speed;
						cout << " to LO cluster of ";
						cout << new_task_j->name << endl;

						for (int i = 0; i < taskset.size(); i++) {
							taskset[i]->print();
						}
#endif 
						if (check(response_time_result, taskset)) {
							return true;
						}
					}
					else {
						double x = delta / temperature_function_heavy(count, count_max);
						if (exp(x) > generate_random_01()) {
							response_time_result[index_i] = new_r_i;
							response_time_result[index_j] = new_r_j;
							taskset[index_i] = new_task_i;
							taskset[index_j] = new_task_j;
#ifdef PRINT
							cout << "LO cluster of ";
							cout << new_task_i->name;
							cout << " gives a processor ";
							cout << target.name << " " << target.speed;
							cout << " to LO cluster of ";
							cout << new_task_j->name << endl;

							for (int i = 0; i < taskset.size(); i++) {
								taskset[i]->print();
							}
#endif
							if (check(response_time_result, taskset)) {
								return true;
							}

						}
					}
					count++;
					continue;
				}
			}
		}

		if (rule == 4) {
			// HI_LO to LO_LO
			if (index_LO.size() == 0 || index_HI.size() == 0) {
				continue;
			}
			else {
				int i = generate_random_number(0, index_HI.size() - 1);
				int j = generate_random_number(0, index_LO.size() - 1);

				if (taskset[index_HI[i]]->cluster_N->size() == 0) {
					continue;
				}
				else {
					//cout << rule << endl;
					int index_i = index_HI[i];
					int index_j = index_LO[j];

					processor target = *select_randomly(taskset[index_i]->cluster_N->processor_list.begin(), taskset[index_i]->cluster_N->processor_list.end());

					heavy_task* new_task_i = new heavy_task(taskset[index_i]);
					heavy_task* new_task_j = new heavy_task(taskset[index_j]);

					int add_index = index_HI[generate_random_number(0, index_HI.size() - 1)];
					heavy_task* new_task_add = new heavy_task(taskset[add_index]);

					double r_i;
					double r_j;
					double r_add;

					double new_r_i;
					double new_r_j;
					double new_r_add;

					if (add_index == index_i) {
						new_task_i->cluster_N->del(target);
						new_task_j->cluster_N->add(target);
						new_task_i->cluster_O->add(target);

						r_i = response_time_result[index_i];
						r_j = response_time_result[index_j];

						new_r_i = new_task_i->response_time();
						new_r_j = new_task_j->response_time();
					}
					else {
						new_task_i->cluster_N->del(target);
						new_task_j->cluster_N->add(target);
						new_task_add->cluster_O->add(target);

						r_i = response_time_result[index_i];
						r_j = response_time_result[index_j];
						r_add = response_time_result[add_index];

						new_r_i = new_task_i->response_time();
						new_r_j = new_task_j->response_time();
						new_r_add = new_task_add->response_time();

					}

					double delta;
					if (add_index == index_i) {
						delta = -cost_function(r_i, new_task_i->deadline) - cost_function(r_j, new_task_j->deadline)
							+ cost_function(new_r_i, new_task_i->deadline) + cost_function(new_r_j, new_task_j->deadline);
					}
					else {
						delta = -cost_function(r_i, new_task_i->deadline) - cost_function(r_j, new_task_j->deadline) - cost_function(r_add, new_task_add->deadline)
							+ cost_function(new_r_i, new_task_i->deadline) + cost_function(new_r_j, new_task_j->deadline) + cost_function(new_r_add, new_task_add->deadline);
					}

					if (delta < 0) {
						//accept
						if (add_index == index_i) {
							response_time_result[index_i] = new_r_i;
							response_time_result[index_j] = new_r_j;

							taskset[index_i] = new_task_i;
							taskset[index_j] = new_task_j;
						}
						else {
							response_time_result[index_i] = new_r_i;
							response_time_result[index_j] = new_r_j;
							response_time_result[add_index] = new_r_add;

							taskset[index_i] = new_task_i;
							taskset[index_j] = new_task_j;
							taskset[add_index] = new_task_add;
						}
#ifdef PRINT
						cout << "LO cluster of ";
						cout << new_task_i->name;
						cout << " gives a processor ";
						cout << target.name << " " << target.speed;
						cout << " to LO cluster of ";
						cout << new_task_j->name;

						cout << ". HI cluster of ";
						if (add_index == index_i) {
							cout << new_task_i->name;
						}
						else {
							cout << new_task_add->name;
						}
						cout << " also get this processor" << endl;

						for (int i = 0; i < taskset.size(); i++) {
							taskset[i]->print();
						}
#endif 
						if (check(response_time_result, taskset)) {
							return true;
						}
					}
					else {
						double x = delta / temperature_function_heavy(count, count_max);
						if (exp(x) > generate_random_01()) {
							if (add_index == index_i) {
								response_time_result[index_i] = new_r_i;
								response_time_result[index_j] = new_r_j;

								taskset[index_i] = new_task_i;
								taskset[index_j] = new_task_j;
							}
							else {
								response_time_result[index_i] = new_r_i;
								response_time_result[index_j] = new_r_j;
								response_time_result[add_index] = new_r_add;

								taskset[index_i] = new_task_i;
								taskset[index_j] = new_task_j;
								taskset[add_index] = new_task_add;
							}
#ifdef PRINT
							cout << "LO cluster of ";
							cout << new_task_i->name;
							cout << " gives a processor ";
							cout << target.name << " " << target.speed;
							cout << " to LO cluster of ";
							cout << new_task_j->name;

							cout << ". HI cluster of ";
							if (add_index == index_i) {
								cout << new_task_i->name;
							}
							else {
								cout << new_task_add->name;
							}
							cout << " also get this processor" << endl;

							for (int i = 0; i < taskset.size(); i++) {
								taskset[i]->print();
							}
#endif 
							if (check(response_time_result, taskset)) {
								return true;
							}

						}
					}
					count++;
					continue;
				}
			}
		}

		if (rule == 5) {
			// LO_LO to HI_LO
			if (index_LO.size() == 0 || index_HI.size() == 0) {
				continue;
			}
			else {
				int i = generate_random_number(0, index_LO.size() - 1);
				int j = generate_random_number(0, index_HI.size() - 1);

				if (taskset[index_LO[i]]->cluster_N->size() == 0) {
					continue;
				}
				else {
					//cout << rule << endl;
					int index_i = index_LO[i];
					int index_j = index_HI[j];

					processor target = *select_randomly(taskset[index_i]->cluster_N->processor_list.begin(), taskset[index_i]->cluster_N->processor_list.end());

					heavy_task* new_task_i = new heavy_task(taskset[index_i]);
					heavy_task* new_task_j = new heavy_task(taskset[index_j]);


					// find the HI_HI, which contain the processor
					int del_index = -1;
					for (int i = 0; i < index_HI.size(); i++) {

						if (taskset[index_HI[i]]->cluster_O->find(target) == true) {
							del_index = index_HI[i];
							break;
						}

					}

					heavy_task* new_task_del = new heavy_task(taskset[del_index]);

					double r_i;
					double r_j;
					double r_del;

					double new_r_i;
					double new_r_j;
					double new_r_del;

					if (del_index == index_j) {
						new_task_i->cluster_N->del(target);
						new_task_j->cluster_N->add(target);
						new_task_j->cluster_O->del(target);

						r_i = response_time_result[index_i];
						r_j = response_time_result[index_j];

						new_r_i = new_task_i->response_time();
						new_r_j = new_task_j->response_time();
					}
					else {
						new_task_i->cluster_N->del(target);
						new_task_j->cluster_N->add(target);
						new_task_del->cluster_O->del(target);

						r_i = response_time_result[index_i];
						r_j = response_time_result[index_j];
						r_del = response_time_result[del_index];

						new_r_i = new_task_i->response_time();
						new_r_j = new_task_j->response_time();
						new_r_del = new_task_del->response_time();

					}

					double delta;
					if (del_index == index_j) {
						delta = -cost_function(r_i, new_task_i->deadline) - cost_function(r_j, new_task_j->deadline)
							+ cost_function(new_r_i, new_task_i->deadline) + cost_function(new_r_j, new_task_j->deadline);
					}
					else {
						delta = -cost_function(r_i, new_task_i->deadline) - cost_function(r_j, new_task_j->deadline) - cost_function(r_del, new_task_del->deadline)
							+ cost_function(new_r_i, new_task_i->deadline) + cost_function(new_r_j, new_task_j->deadline) + cost_function(new_r_del, new_task_del->deadline);
					}

					if (delta < 0) {
						//accept
						if (del_index == index_j) {
							response_time_result[index_i] = new_r_i;
							response_time_result[index_j] = new_r_j;

							taskset[index_i] = new_task_i;
							taskset[index_j] = new_task_j;
						}
						else {
							response_time_result[index_i] = new_r_i;
							response_time_result[index_j] = new_r_j;
							response_time_result[del_index] = new_r_del;

							taskset[index_i] = new_task_i;
							taskset[index_j] = new_task_j;
							taskset[del_index] = new_task_del;
						}
#ifdef PRINT
						cout << "LO cluster of ";
						cout << new_task_i->name;
						cout << " gives a processor ";
						cout << target.name << " " << target.speed;
						cout << " to LO cluster of ";
						cout << new_task_j->name;

						cout << ". HI cluster of ";
						if (del_index == index_j) {
							cout << new_task_j->name;
						}
						else {
							cout << new_task_del->name;
						}
						cout << " also delete this processor" << endl;

						for (int i = 0; i < taskset.size(); i++) {
							taskset[i]->print();
						}
#endif 
						if (check(response_time_result, taskset)) {
							return true;
						}
					}
					else {
						double x = delta / temperature_function_heavy(count, count_max);
						if (exp(x) > generate_random_01()) {
							if (del_index == index_j) {
								response_time_result[index_i] = new_r_i;
								response_time_result[index_j] = new_r_j;

								taskset[index_i] = new_task_i;
								taskset[index_j] = new_task_j;
							}
							else {
								response_time_result[index_i] = new_r_i;
								response_time_result[index_j] = new_r_j;
								response_time_result[del_index] = new_r_del;

								taskset[index_i] = new_task_i;
								taskset[index_j] = new_task_j;
								taskset[del_index] = new_task_del;
							}
#ifdef PRINT
							cout << "LO cluster of ";
							cout << new_task_i->name;
							cout << " gives a processor ";
							cout << target.name << " " << target.speed;
							cout << " to LO cluster of ";
							cout << new_task_j->name;

							cout << ". HI cluster of ";
							if (del_index == index_j) {
								cout << new_task_j->name;
							}
							else {
								cout << new_task_del->name;
							}
							cout << " also delete this processor" << endl;

							for (int i = 0; i < taskset.size(); i++) {
								taskset[i]->print();
							}
#endif 
							if (check(response_time_result, taskset)) {
								return true;
							}

						}
					}
					count++;
					continue;
				}
			}
		}


	}

	return false;
}


bool decresing_heavy_utilisation(heavy_task* t1, heavy_task* t2) {
	/*
	* The tasks are ordered by decreasing utilisation
	*/
	double u1 = t1->C_O / t1->deadline;
	double u2 = t2->C_O / t2->deadline;
	return u1 > u2;
}

bool allocate_heavy_task(vector<processor> processorset, vector<heavy_task*> taskset) {
	// initial solution
	sort(processorset.begin(), processorset.end(), dresing_processor_speed);
	sort(taskset.begin(), taskset.end(), decresing_heavy_utilisation);

	vector<int> index_HI;
	vector<int> index_LO;
	for (int i = 0; i < taskset.size(); i++) {
		if (taskset[i]->criticality == "HI") {
			index_HI.push_back(i);
		}
		else {
			index_LO.push_back(i);
		}
	}
	int j = 0;
	for (int i = 0; i < taskset.size(); i++) {
		while (j < processorset.size()) {
			taskset[i]->cluster_N->add(processorset[j]);

			if (taskset[i]->criticality == "LOW") {
				if (index_HI.size() != 0) {
					int random_index = index_HI[generate_random_number(0, index_HI.size() - 1)];
					taskset[random_index]->cluster_O->add(processorset[j]);
				}
			}
			j++;
			if (taskset[i]->response_time_low() <= taskset[i]->deadline) {
				break;
			}
		}
	}

	


	if (j < processorset.size()) {
#ifdef PRINT
		for (int i = 0; i < taskset.size(); i++) {
			taskset[i]->print();
		}
#endif 
		return true; // all tasks have been already schedulable now!
	}
#ifdef PRINT
	cout << "Initial solution:" << endl;
	for (int i = 0; i < taskset.size(); i++) {
		taskset[i]->print();
	}
#endif 

	vector<double> response_time_result;
	response_time_result.reserve(taskset.size());
	for (int i = 0; i < taskset.size(); i++) {
		response_time_result.push_back(taskset[i]->response_time());
	}
	bool result = false;
	result = check(response_time_result, taskset);
	if (result == true) {
		return true;
	}
	// local search
	int max_count = max((int)pow(2, processorset.size() / 4), (int)(8 * processorset.size()));
	result = local_search(taskset, index_HI, index_LO, response_time_result, max_count);
	return result;
}

#endif // !allocate_heavy_task_h