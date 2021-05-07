#include <iostream>

#include "maxdefense.hh"
#include "timer.hh"

using namespace std;

int main()
{
	int greedy_n = 5000;	// change this value to test diff sizes
	unique_ptr<ArmorVector> all_armor = load_armor_database("armor.csv");
	unique_ptr<ArmorVector> greedy_armors = filter_armor_vector(*all_armor, 1, 2500, greedy_n);
	Timer time;
	greedy_max_defense(*greedy_armors, 100);
	double elapsed = time.elapsed();
	cout << "Greedy size " << greedy_n << " Time: " << elapsed << endl;

	int exhaustive_n = 20;	// change this value to test diff sizes
	unique_ptr<ArmorVector> exhaustive_armors = filter_armor_vector(*all_armor, 1, 2500, exhaustive_n);
	exhaustive_max_defense(*exhaustive_armors, 100);
	elapsed = time.elapsed();
	cout << "Exhastive size " << exhaustive_n << " Time: " << elapsed << endl;
	return 0;
}
