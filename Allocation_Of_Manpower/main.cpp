
#include <iostream>
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
	const Integer INF_INTEGER = 2147483647;

private:
	MCMFGraph<string, PriorityLevel, Personnel> _MCMF_Graph;	// minimum-cost maximum-flow

	Integer _Source, _Sink, _Number_Of_Office, _Number_Of_Region;
	unordered_map<string, Integer> _Numbering;


public:
	Office_Manager() :_Source(0), _Sink(0), _Number_Of_Office(0), _Number_Of_Region(0) {}
	Office_Manager(vector<vector<string>>& police_info, vector<vector<string>>& region_info) :Office_Manager() { update_info(police_info, region_info); }
	~Office_Manager() { _Numbering.clear(); }

private:
	// police_info: office name, number of people in the office, police region
	// region_info: region name, accident probability, accident scale, lower bound, upper bound
	void numbering_for_mcmf_graph(vector<vector<string>>& police_info, vector<vector<string>>& region_info)   
	{
		_Numbering = unordered_map<string, Integer>();
		_Numbering.insert({ "source", _Source = _Numbering.size() });
		for (Integer i = 0; i < _Number_Of_Office; i++) _Numbering.insert({ police_info[i][0], _Numbering.size() });
		for (Integer i = 0; i < _Number_Of_Region; i++) _Numbering.insert({ region_info[i][0], _Numbering.size() });
		_Numbering.insert({ "sink", _Sink = _Numbering.size() });
	}

	void construct_mcmf_graph(vector<vector<string>>& police_info, vector<vector<string>>& region_info)
	{
		_MCMF_Graph = MCMFGraph<string, PriorityLevel, Personnel>(_Sink + 1, PriorityLevel(), PriorityLevel(PriorityLevel::VALUETYPE::MAXIMUM), 0, INF_INTEGER);

		for (auto& _item : _Numbering) _MCMF_Graph[_item.second] = _item.first;

		for (Integer i = 0; i < _Number_Of_Office; i++) {
			_MCMF_Graph.add_directed_edge(_Source, _Numbering[police_info[i][0]], PriorityLevel(), stoll(police_info[i][1]));

			for (Integer j = 2, pi_size = police_info[i].size(); j < pi_size; j++)
				if (_Numbering.find(police_info[i][j]) != _Numbering.end()) _MCMF_Graph.add_directed_edge(_Numbering[police_info[i][0]], _Numbering[police_info[i][j]], PriorityLevel(), INF_INTEGER);
		}

		auto polynomial_generator = [&](Integer _size, Integer _idx, Integer _val) {
			auto _coeff = vector<Integer>(_size); _coeff[_idx] = _val;
			return Polynomial(_coeff);
		};

		for (Integer i = 0; i < _Number_Of_Region; i++) {
			_MCMF_Graph.add_directed_edge(_Numbering[region_info[i][0]], _Sink
				, PriorityLevel(PriorityLevel::VALUETYPE::VARIABLE
					, Polynomial(polynomial_generator(PriorityLevel::ACCIDENT::PROBABILITY, stoll(region_info[i][1]), 1))
					, Polynomial(polynomial_generator(PriorityLevel::ACCIDENT::SCALE, stoll(region_info[i][2]), 1)))
				, stoll(region_info[i][4]));
		}
	}


public:
	void update_info(vector<vector<string>>& police_info, vector<vector<string>>& region_info)
	{
		_Number_Of_Office = police_info.size(); 
		_Number_Of_Region = region_info.size();
		
		printf("initializing data..."); 
		numbering_for_mcmf_graph(police_info, region_info);	
		printf("\rinitializing data is completed!!\n");

		printf("configuring mcmf graph...");
		construct_mcmf_graph(police_info, region_info);
		printf("\rconfiguring mcmf graph is completed!!\n");
	}

	void allocation_of_police()
	{
		printf("processing mcmf...");
		auto _result = _MCMF_Graph.MCMF(_Source, _Sink);
		printf("\rmcmf is completed!!\n");

		auto _total_cost = _result.first.first;
		auto _total_flow = _result.first.second;
		auto _detail_flow = _result.second;

		cout << "\n===============================================================\n";
		cout << "Total Cost: " << _total_cost << "\n";
		cout << "Total Flow: " << _total_flow << "\n";
		for (Integer i = 0; i < _detail_flow.size(); i++)
			if (_detail_flow[i] > 0) cout << _MCMF_Graph[i] << "에 투입된 인원: " << _detail_flow[i] << '\n';
		cout << _MCMF_Graph;
		cout << "===============================================================\n";
	}
};


void create_virtaul_data()
{
	printf("\ncreating virtual data...");
	Virtual_Data_Generator::create_virtual_data(2100, 170, 30, 8700, 240);
	printf("\rcreating virtual data is completed!!\n\n");
}



int main()
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL); cout.tie(NULL);

	//auto police_info = CSV_Manager::read_csv("police_info_test.csv");
	//auto region_info = CSV_Manager::read_csv("region_info_test.csv");

	create_virtaul_data();
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