
#include <iostream>
#include <algorithm>
#include <unordered_map>

#include "MCMF.h"
#include "FFT.h"
#include "Polynomial.h"
#include "Mobilization.h"
#include "FileStream.h"
#include "Data_Generator.h"
using namespace std;


class Office_Manager
{
public:
	typedef long long Integer;
	const Integer EMPTY_INTEGER = 0;
	const Integer INF_INTEGER = 2147483647;

private:
	MCMFGraph<string, PriorityLevel, Personnel> _MCMF_Graph;	// minimum-cost maximum-flow with edge demands
	Integer _Source, _Sink, _Source_For_Lower, _Sink_For_Lower, _Begin_Office_Number, _Begin_Region_In_Number, _Begin_Region_Number, _Total_Edge_Demands;
	unordered_map<string, Integer> _Numbering;

	Integer _Number_Of_Office, _Number_Of_Region;
	vector<vector<string>> _Police_Info, _Region_Info;


public:
	Office_Manager() : _MCMF_Graph(MCMFGraph<string, PriorityLevel, Personnel>()), _Source(0), _Sink(0), _Source_For_Lower(0), _Sink_For_Lower(0), _Begin_Office_Number(0), _Begin_Region_In_Number(0), _Begin_Region_Number(0), _Total_Edge_Demands(0), _Numbering(unordered_map<string, Integer>()),
		_Number_Of_Office(0), _Number_Of_Region(0), _Police_Info(vector<vector<string>>()), _Region_Info(vector<vector<string>>()) {}
	Office_Manager(vector<vector<string>>& police_info, vector<vector<string>>& region_info) :Office_Manager() { update_info(police_info, region_info); }
	~Office_Manager() { _Numbering.clear(); }

private:
	// police_info: office name, number of people in the office, police region
	// region_info: region name, accident probability, accident scale, lower bound, upper bound
	void numbering_for_mcmf_graph()
	{
		_Numbering = unordered_map<string, Integer>();
		_Numbering.insert({ "source", _Source = _Numbering.size() });
		_Numbering.insert({ "sink", _Sink = _Numbering.size() });
		_Numbering.insert({ "source_for_lower", _Source_For_Lower = _Numbering.size() });
		_Numbering.insert({ "sink_for_lower", _Sink_For_Lower = _Numbering.size() });
		for (Integer i = 0; i < _Number_Of_Office; i++) _Numbering.insert({ _Police_Info[i][0], _Numbering.size() });
		for (Integer i = 0; i < _Number_Of_Region; i++) _Numbering.insert({ _Region_Info[i][0] + "in", _Numbering.size() });
		for (Integer i = 0; i < _Number_Of_Region; i++) _Numbering.insert({ _Region_Info[i][0], _Numbering.size() });
		_Begin_Office_Number = _Numbering[_Police_Info[0][0]]; 
		_Begin_Region_In_Number = _Numbering[_Region_Info[0][0] + "in"]; 
		_Begin_Region_Number = _Numbering[_Region_Info[0][0]];
	}

	void construct_mcmf_graph()
	{
		_MCMF_Graph = MCMFGraph<string, PriorityLevel, Personnel>(_Numbering.size(), PriorityLevel(), PriorityLevel(PriorityLevel::VALUETYPE::MAXIMUM), 0, INF_INTEGER);

		for (auto& _item : _Numbering) _MCMF_Graph[_item.second] = _item.first;

		_MCMF_Graph.add_directed_edge(_Sink, _Source, PriorityLevel(), INF_INTEGER);	// lower test

		for (Integer i = 0; i < _Number_Of_Office; i++) {
			_MCMF_Graph.add_directed_edge(_Source, _Numbering[_Police_Info[i][0]], PriorityLevel(), stoll(_Police_Info[i][1]));

			for (Integer j = 2, pi_size = _Police_Info[i].size(); j < pi_size; j++)
				if (_Numbering.find(_Police_Info[i][j]) != _Numbering.end()) _MCMF_Graph.add_directed_edge(_Numbering[_Police_Info[i][0]], _Numbering[_Police_Info[i][j] + "in"], PriorityLevel(), INF_INTEGER);
		}

		auto polynomial_generator = [&](Integer _size, Integer _idx, Integer _val) {
			auto _coeff = vector<Integer>(_size); _coeff[_idx] = _val;
			return Polynomial(_coeff);
		};

		for (Integer i = 0; i < _Number_Of_Region; i++) {
			_MCMF_Graph.add_directed_edge(_Numbering[_Region_Info[i][0] + "in"], _Numbering[_Region_Info[i][0]]
				, PriorityLevel(PriorityLevel::VALUETYPE::VARIABLE
					, Polynomial(polynomial_generator(PriorityLevel::ACCIDENT::PROBABILITY, stoll(_Region_Info[i][1]), 1))
					, Polynomial(polynomial_generator(PriorityLevel::ACCIDENT::SCALE, stoll(_Region_Info[i][2]), 1))), INF_INTEGER);
			_MCMF_Graph.add_directed_edge(_Numbering[_Region_Info[i][0]], _Sink, PriorityLevel(), stoll(_Region_Info[i][4]) - stoll(_Region_Info[i][3]));

			if (stoll(_Region_Info[i][3]) > 0) {
				_Total_Edge_Demands += stoll(_Region_Info[i][3]);
				_MCMF_Graph.add_directed_edge(_Source_For_Lower, _Sink, PriorityLevel(), stoll(_Region_Info[i][3]));
				_MCMF_Graph.add_directed_edge(_Numbering[_Region_Info[i][0]], _Sink_For_Lower, PriorityLevel(), stoll(_Region_Info[i][3]));
			}
		}
	}


public:
	void update_info(vector<vector<string>>& police_info, vector<vector<string>>& region_info)
	{
		_Police_Info = police_info;		
		_Region_Info = region_info;

		_Number_Of_Office = _Police_Info.size();
		_Number_Of_Region = _Region_Info.size();
		
		printf("initializing data..."); 
		numbering_for_mcmf_graph();	
		printf("\rinitializing data is completed!!\n");

		printf("configuring mcmf graph...");
		construct_mcmf_graph();
		printf("\rconfiguring mcmf graph is completed!!\n");
	}


	struct Allocation_Of_Office_Line
	{
		string _Office_Name;
		Integer _Number_Of_Officer;
		vector<pair<string, Integer>> _Regions;
		Allocation_Of_Office_Line() :_Office_Name(""), _Number_Of_Officer(0), _Regions(vector<pair<string, Integer>>()) {}
	};

	struct Allocation_Of_Regions_Line
	{
		string _Regions_Name;
		Integer _Number_Of_Officer;
		Allocation_Of_Regions_Line() :_Regions_Name(""), _Number_Of_Officer(0) {}
	};

	void allocation_of_police()
	{
		printf("processing mcmf...");
		auto _result = _MCMF_Graph.MCMF_with_Edge_Demands(_Source, _Sink, _Source_For_Lower, _Sink_For_Lower);
		printf("\rmcmf is completed!!\n");

		auto _result_of_office = vector<Allocation_Of_Office_Line>(_Number_Of_Office, Allocation_Of_Office_Line());
		auto _result_of_region = vector<Allocation_Of_Regions_Line>(_Number_Of_Region, Allocation_Of_Regions_Line());

		for (auto& _edge : _result._Graph[_Source]) {
			if (_Begin_Office_Number <= _edge->_Next_Vertex && _edge->_Next_Vertex < _Begin_Office_Number + _Number_Of_Office) 
			{
				Integer _office_num = _edge->_Next_Vertex - _Begin_Office_Number;
				_result_of_office[_office_num]._Office_Name = _Police_Info[_office_num][0];
				_result_of_office[_office_num]._Number_Of_Officer = _edge->_Curr_Flow;
			}
		}

		for (Integer _office_num = 0; _office_num < _Number_Of_Office; _office_num++) {
			for (auto& _edge : _result._Graph[_Begin_Office_Number + _office_num]) {
				if (_Begin_Region_In_Number <= _edge->_Next_Vertex && _edge->_Next_Vertex < _Begin_Region_In_Number + _Number_Of_Region) 
				{
					Integer _region_num = _edge->_Next_Vertex - _Begin_Region_In_Number;
					_result_of_office[_office_num]._Regions.push_back({ _Region_Info[_region_num][0], _edge->_Curr_Flow });
					_result_of_region[_region_num]._Number_Of_Officer += _edge->_Curr_Flow;
				}
			}
			sort(_result_of_office[_office_num]._Regions.begin(), _result_of_office[_office_num]._Regions.end());
		}

		for (Integer _region_num = 0; _region_num < _Number_Of_Region; _region_num++) _result_of_region[_region_num]._Regions_Name = _Region_Info[_region_num][0];



		cout << "\n---------------------------------------------------------------\n\n";
		cout << "Flow For Edge Demands: " << _result._Flow_For_Edge_Demands << ", [is meet the edge demands?: " << vector<string>{ "False", "True" } [_result._Flow_For_Edge_Demands == _Total_Edge_Demands] << "]" << "\n";
		cout << "Total Cost: " << _result._Total_Cost << "\n";
		cout << "Total Flow: " << _result._Total_Flow << "\n";

		cout << "\n---------------------------------------------------------------\n";
		for (Integer _office_num = 0; _office_num < _Number_Of_Office; _office_num++) {
			cout << "\n\"" << _result_of_office[_office_num]._Office_Name << "\"" << "에서 동원 가능한 인력은" << _Police_Info[_office_num][1] << "명 이고, " << "동원된 인력은 " << _result_of_office[_office_num]._Number_Of_Officer << "명 입니다." << "\n";
			for (auto& _region : _result_of_office[_office_num]._Regions) cout << " -> " << "\"" << _region.first << "\"" << "에 " << _region.second << "명 지원\n";
		}

		cout << "\n---------------------------------------------------------------\n\n";
		for (Integer _region_num = 0; _region_num < _Number_Of_Region; _region_num++) {
			cout << "\"" << _result_of_region[_region_num]._Regions_Name << "\"" << "에서 요구하는 최소 인력은 " << _Region_Info[_region_num][3] << "명 이고, " << "배치된 인력은 " << _result_of_region[_region_num]._Number_Of_Officer << "명 입니다." << "\n";
		}

		cout << "\n---------------------------------------------------------------\n\n";
	}
};


void create_virtaul_data()
{
	printf("\ncreating virtual data...");
	//Virtual_Data_Generator::create_virtual_data(2100, 170, 30, 8700, 240);
	Virtual_Data_Generator::create_virtual_data(9, 30, 6, 9, 10);
	printf("\rcreating virtual data is completed!!\n\n");
}



int main()
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL); cout.tie(NULL);

	//auto police_info = CSV_Manager::read_csv("police_info_lower.csv");
	//auto region_info = CSV_Manager::read_csv("region_info_lower.csv");

	//create_virtaul_data();
	auto police_info = CSV_Manager::read_csv("police_info_generator.csv");
	auto region_info = CSV_Manager::read_csv("region_info_generator.csv");

	Office_Manager office_manager(police_info, region_info);
	office_manager.allocation_of_police();

	return 0;
}


/*void MCMF_Test()
{
	int N, M, Source, Sink;

	cin >> N >> M;
	Source = N + M; Sink = N + M + 1;

	MCMFGraph<int, PriorityLevel, Personnel> flow_graph(Sink + 1, PriorityLevel(), PriorityLevel(PriorityLevel::VALUETYPE::MAXIMUM), 0, 1100000000);
	vector<vector<int>> cap(M, vector<int>(N)), cost(M, vector<int>(N));

	for (int i = 0, c; i < N; i++) { cin >> c; flow_graph.add_directed_edge(M + i, Sink, PriorityLevel(), c); }
	for (int i = 0, c; i < M; i++) { cin >> c; flow_graph.add_directed_edge(Source, i, PriorityLevel(), c); }

	for (int i = 0; i < M; i++)
		for (int j = 0; j < N; j++) cin >> cap[i][j];

	for (int i = 0; i < M; i++)
		for (int j = 0; j < N; j++) cin >> cost[i][j];

	for (int i = 0; i < M; i++)
		for (int j = 0; j < N; j++)
			flow_graph.add_directed_edge(i, M + j, PriorityLevel(PriorityLevel::VALUETYPE::VARIABLE
				, Polynomial(vector< Polynomial::Integer>{ cost[i][j], 0, 0, 0, 0 }, PriorityLevel::ACCIDENT::PROBABILITY)
				, Polynomial(vector< Polynomial::Integer>{ 1, 0, 0, 0, 0 }, PriorityLevel::ACCIDENT::SCALE))
				, cap[i][j]);

	cout << flow_graph << '\n';
	pair<PriorityLevel, Personnel> result = flow_graph.MCMF(Source, Sink).first;
	cout << result.second << '\n' << result.first << '\n';
	cout << flow_graph << '\n';
}*/