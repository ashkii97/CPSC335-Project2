///////////////////////////////////////////////////////////////////////////////
// maxdefense_test.cc
//
// Unit tests for maxdefense.hh
//
///////////////////////////////////////////////////////////////////////////////


#include <cassert>
#include <sstream>


#include "maxdefense.hh"
#include "rubrictest.hh"


int main()
{
	Rubric rubric;

	ArmorVector trivial_armors;
	trivial_armors.push_back(std::shared_ptr<ArmorItem>(new ArmorItem("test helmet", 100.0, 20.0)));
	trivial_armors.push_back(std::shared_ptr<ArmorItem>(new ArmorItem("test boots", 40.0, 5.0)));

	auto all_armors = load_armor_database("armor.csv");
	assert( all_armors );

	auto filtered_armors = filter_armor_vector(*all_armors, 1, 2500, all_armors->size());

	//
	rubric.criterion(
		"load_armor_database still works", 2,
		[&]()
		{
			TEST_TRUE("non-null", all_armors);
			TEST_EQUAL("size", 8064, all_armors->size());
		}
	);

	//
	rubric.criterion(
		"filter_armor_vector", 2,
		[&]()
		{
			auto
				three = filter_armor_vector(*all_armors, 100, 500, 3),
				ten = filter_armor_vector(*all_armors, 100, 500, 10);
			TEST_TRUE("non-null", three);
			TEST_TRUE("non-null", ten);
			TEST_EQUAL("total_size", 3, three->size());
			TEST_EQUAL("total_size", 10, ten->size());
			TEST_EQUAL("contents", "used high-quality mystical human chest plate", (*ten)[0]->description());
			TEST_EQUAL("contents", "deteriorating poor quality enchanted human shield", (*ten)[9]->description());
			for (int i = 0; i < 3; i++) {
				TEST_EQUAL("contents", (*three)[i]->description(), (*ten)[i]->description());
			}
		}
	);

	//
	rubric.criterion(
		"greedy_max_defense trivial cases", 2,
		[&]()
		{
			std::unique_ptr<ArmorVector> soln;

			soln = greedy_max_defense(trivial_armors, 10);
			TEST_TRUE("non-null", soln);
			TEST_TRUE("empty solution", soln->empty());

			soln = greedy_max_defense(trivial_armors, 100);
			TEST_TRUE("non-null", soln);
			TEST_EQUAL("helmet only", 1, soln->size());
			TEST_EQUAL("helmet only", "test helmet", (*soln)[0]->description());

			soln = greedy_max_defense(trivial_armors, 99);
			TEST_TRUE("non-null", soln);
			TEST_EQUAL("boots only", 1, soln->size());
			TEST_EQUAL("boots only", "test boots", (*soln)[0]->description());

			soln = greedy_max_defense(trivial_armors, 150);
			TEST_TRUE("non-null", soln);
			TEST_EQUAL("helmet and boots", 2, soln->size());
			TEST_EQUAL("helmet and boots", "test helmet", (*soln)[0]->description());
			TEST_EQUAL("helmet and boots", "test boots", (*soln)[1]->description());

		}
	);

	//
	rubric.criterion(
		"greedy_max_defense correctness", 4,
		[&]()
		{
			std::unique_ptr<ArmorVector> soln_small, soln_large;

			soln_small = greedy_max_defense(*filtered_armors, 500),
			soln_large = greedy_max_defense(*filtered_armors, 5000);

			//print_armor_vector(*soln_small);
			//print_armor_vector(*soln_large);

			TEST_TRUE("non-null", soln_small);
			TEST_TRUE("non-null", soln_large);

			TEST_FALSE("non-empty", soln_small->empty());
			TEST_FALSE("non-empty", soln_large->empty());

			double
				cost_small, defense_small,
				cost_large, defense_large
				;
			sum_armor_vector(*soln_small, cost_small, defense_small);
			sum_armor_vector(*soln_large, cost_large, defense_large);

			//	Precision
			cost_small	= std::round( cost_small	* 100 ) / 100;
			defense_small	= std::round( defense_small	* 100 ) / 100;
			cost_large	= std::round( cost_large	* 100 ) / 100;
			defense_large	= std::round( defense_large	* 100 ) / 100;

			TEST_EQUAL("Small solution cost",	481.48,	cost_small);
			TEST_EQUAL("Small solution defense",	950.19,	defense_small);
			TEST_EQUAL("Large solution cost",	4990.35,	cost_large);
			TEST_EQUAL("Large solution defense",	9209.82,	defense_large);
		}
	);

	//
	rubric.criterion(
		"exhaustive_max_defense trivial cases", 2,
		[&]()
		{
			std::unique_ptr<ArmorVector> soln;

			soln = exhaustive_max_defense(trivial_armors, 10);
			TEST_TRUE("non-null", soln);
			TEST_TRUE("empty solution", soln->empty());

			soln = exhaustive_max_defense(trivial_armors, 100);
			TEST_TRUE("non-null", soln);
			TEST_EQUAL("helmet only", 1, soln->size());
			TEST_EQUAL("helmet only", "test helmet", (*soln)[0]->description());

			soln = exhaustive_max_defense(trivial_armors, 99);
			TEST_TRUE("non-null", soln);
			TEST_EQUAL("boots only", 1, soln->size());
			TEST_EQUAL("boots only", "test boots", (*soln)[0]->description());

			soln = exhaustive_max_defense(trivial_armors, 150);
			TEST_TRUE("non-null", soln);
			TEST_EQUAL("helmet and boots", 2, soln->size());
			TEST_EQUAL("helmet and boots", "test helmet", (*soln)[0]->description());
			TEST_EQUAL("helmet and boots", "test boots", (*soln)[1]->description());
		}
	);

	//
	rubric.criterion(
		"exhaustive_max_defense correctness", 4,
		[&]()
		{
			std::vector<double> optimal_defense_totals =
			{
				500,		1033.05,	1100,	1600,	1600,
				1600,		1900,		2100,	2300,	2300,
				2300,		2300,		2400,	2400,	2400,
				2400,		2400,		2400,	2400,	2400
			};

			for ( int optimal_index = 0; optimal_index < optimal_defense_totals.size(); optimal_index++ )
			{
				int n = optimal_index + 1;
				double expected_defense = optimal_defense_totals[optimal_index];

				auto small_armors = filter_armor_vector(*filtered_armors, 1, 2000, n);
				TEST_TRUE("non-null", small_armors);

				auto solution = exhaustive_max_defense(*small_armors, 2000);
				TEST_TRUE("non-null", solution);

				double actual_cost, actual_defense;
				sum_armor_vector(*solution, actual_cost, actual_defense);

				// Round
				expected_defense	= std::round( expected_defense	/ 100.0) * 100;
				actual_defense		= std::round( actual_defense	/ 100.0) * 100;

				std::stringstream ss;
				ss
					<< "exhaustive search n = " << n << " (optimal index = " << optimal_index << ")"
					<< ", expected defense = " << expected_defense
					<< " but algorithm found = " << actual_defense
					;
				TEST_EQUAL(ss.str(), expected_defense, actual_defense);

				auto greedy_solution = greedy_max_defense(*small_armors, 2000);
				double greedy_actual_cost, greedy_actual_defense;
				sum_armor_vector(*solution, greedy_actual_cost, greedy_actual_defense);
				greedy_actual_defense	= std::round( greedy_actual_defense	/ 100.0) * 100;
				TEST_EQUAL("Exhaustive and greedy get the same answer", actual_defense, greedy_actual_defense);
			}
		}
	);

	return rubric.run();
}
