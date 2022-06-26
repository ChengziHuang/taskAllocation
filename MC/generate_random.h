#ifndef generated_random_h
#define generated_random_h
#include <iostream>
#include <chrono>
#include <random>

using namespace std;


inline int generate_random_number(int low, int upper) {
	random_device dev;

	mt19937 rng(dev() ^ (
		(std::mt19937::result_type)
		std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::system_clock::now().time_since_epoch()
			).count() +
		(std::mt19937::result_type)
		std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count()));
	uniform_int_distribution<mt19937::result_type> dist(low, upper);
	return dist(rng);
}

template<typename Iter, typename RandomGenerator>
inline Iter select_randomly(Iter start, Iter end, RandomGenerator* g) {
	uniform_int_distribution<> dis(0, distance(start, end) - 1);
	advance(start, dis(*g));
	return start;
}

template<typename Iter>
inline Iter select_randomly(Iter start, Iter end) {
	random_device rd;
	mt19937 gen(rd() ^ (
		(std::mt19937::result_type)
		std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::system_clock::now().time_since_epoch()
			).count() +
		(std::mt19937::result_type)
		std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count()) );
	return select_randomly(start, end, &gen);
}


inline double generate_random_01() {
	random_device rd;
	default_random_engine eng(rd() ^ (
		(std::mt19937::result_type)
		std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::system_clock::now().time_since_epoch()
			).count() +
		(std::mt19937::result_type)
		std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count()) );
	uniform_real_distribution<float> dist(0, 1);
	return dist(eng);
}
#endif // !generated_random_h