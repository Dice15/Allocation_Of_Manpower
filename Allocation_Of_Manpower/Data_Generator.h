#pragma once

#include <fstream>
#include <random>
#include <string>
#include <vector>
using namespace std;

template <typename dataType>
class Random_Integer_Generator
{
private:
	dataType rx, ry;
	random_device random_dev;
	mt19937 generate;
	uniform_int_distribution<dataType> random_data;

public:
	Random_Integer_Generator(dataType _rx, dataType _ry)
		:rx(_rx), ry(_ry) {
		generate = mt19937(random_dev());
		random_data = uniform_int_distribution<dataType>(_rx, _ry);
	}

	dataType get_random() { return random_data(generate); }
};

class Virtual_Data_Generator
{
public:
	typedef long long Integer;
	typedef bool Boolean;

public:
	static void create_virtual_data(Integer _police_info_num, Integer _officer_max_num, Integer _officer_region_max_num, Integer _region_info_num, Integer _require_officer_max_num)
	{

		ofstream _fout("police_info_generator.csv");
		if (_fout.is_open())
		{
			for (Integer i = 0; i < _police_info_num; i++)
			{
				string _line = "office" + to_string(i);
				_line += "," + to_string(Random_Integer_Generator<Integer>(_officer_max_num / 3, _officer_max_num).get_random());

				vector<Boolean> dup_check(Random_Integer_Generator<Integer>(_officer_region_max_num / 2, _officer_region_max_num).get_random(), false);
				for (Integer j = dup_check.size(); j > 0; j--)
				{
					Integer random_num = Random_Integer_Generator<Integer>(0, dup_check.size() - 1).get_random();
					while (dup_check[random_num]) random_num = Random_Integer_Generator<Integer>(0, dup_check.size() - 1).get_random();
					dup_check[random_num] = true;
					_line += ","; _line += "region" + to_string(random_num);
				}
				_line += "\n";
				_fout << _line;			
			}
			_fout.close();
		}

		_fout.open("region_info_generator.csv");
		if (_fout.is_open())
		{
			for (Integer i = 0; i < _region_info_num; i++)
			{
				string _line = "region" + to_string(i);
				Integer _require = Random_Integer_Generator<Integer>(0, _require_officer_max_num).get_random();
				_line += "," + to_string(Random_Integer_Generator<Integer>(0, 4).get_random());
				_line += "," + to_string(Random_Integer_Generator<Integer>(0, 4).get_random());
				_line += "," + to_string(Random_Integer_Generator<Integer>(0, _require / 3).get_random());
				_line += "," + to_string(_require);
				_line += "\n";
				_fout << _line;
			}
			_fout.close();
		}
	}
};