#pragma once

/*********************************************************************************
*   Minimum Cost Maximum Flow with Edge Demands Flow
*   Time Complexity : o((V * E * f) + (V^2 * E))
* 
*	MCMF(SPFA + Edmonds Karp), Maximum Flow with Edge Demands(Dinic's Algorithm)
*
*   Author : J.H Moon <jqkt15@naver.com>
*   Date : August 19, 2020
**********************************************************************************/


#include <vector>
#include <string>
#include <queue>
using namespace std;


template <typename ItemType, typename CostType, typename FlowType>
class MCMFGraph
{
public: 
	typedef long long Integer;
	typedef bool Boolean;

	struct Edge {
		Integer _Next_Vertex; CostType _Cost; FlowType _Capacity, _Curr_Flow;
		Edge* _Rev_Edge;
	
		Edge(Integer _next_vertex, CostType _cost, FlowType _capacity, FlowType _curr_flow) :_Next_Vertex(_next_vertex), _Cost(_cost), _Capacity(_capacity), _Curr_Flow(_curr_flow), _Rev_Edge(nullptr) {}

		void update_flow(FlowType _flowed) { _Curr_Flow += _flowed;  _Rev_Edge->_Curr_Flow -= _flowed; }
		FlowType remain_flow() { return _Capacity - _Curr_Flow; }
	};

	struct Result_Of_MCMF {
		CostType _Total_Cost; FlowType _Total_Flow, _Flow_For_Edge_Demands;
		vector<vector<Edge*>> _Graph;
		Result_Of_MCMF(CostType _total_cost, FlowType _total_flow, FlowType _flow_for_edge_demands, vector<vector<Edge*>> _graph) :_Total_Cost(_total_cost), _Total_Flow(_total_flow), _Flow_For_Edge_Demands(_flow_for_edge_demands), _Graph(_graph) {}
	};


private:
	Integer _Graph_Size;
	CostType _Empty_Cost, _INF_Cost;
	FlowType _Empty_Flow, _INF_Flow;

	vector<ItemType> _Values;
	vector<vector<Edge*>> _Graph;


public:
	MCMFGraph() {}
	MCMFGraph(Integer _graph_size, CostType _empty_cost, CostType _inf_cost, FlowType _empty_flow, FlowType _inf_flow)
		:_Graph_Size(_graph_size), _Empty_Cost(_empty_cost), _INF_Cost(_inf_cost), _Empty_Flow(_empty_flow), _INF_Flow(_inf_flow), _Values(vector<ItemType>(_graph_size)), _Graph(vector<vector<Edge*>>(_Graph_Size, vector<Edge*>())) {}
	~MCMFGraph() { _Values.clear(); _Graph.clear(); }

	ItemType& operator[](Integer _vidx) { return _Values[_vidx]; }	// access vertex data by index

	void add_directed_edge(Integer _u, Integer _v, CostType _cost, FlowType _capacity) {
		_Graph[_u].push_back(new Edge(_v, _cost, _capacity, _Empty_Flow));
		_Graph[_v].push_back(new Edge(_u, -_cost, 0, _Empty_Flow));
		_Graph[_u].back()->_Rev_Edge = _Graph[_v].back();
		_Graph[_v].back()->_Rev_Edge = _Graph[_u].back();
	}

	void add_undirected_edge(Integer _u, Integer _v, CostType _cost, FlowType _capacity) {
		add_directed_edge(_u, _v, _capacity, _cost);
		add_directed_edge(_v, _u, _capacity, _cost);
	}


private:	// MCMF: SPFA + Edmonds Karp
	Boolean SPFA(Integer _source, Integer _sink, vector<CostType>& _cost, vector<Boolean>& _inque, vector<Integer>& _rev_path, vector<Edge*>& _use_edge)
	{
		queue<Integer> _que;
		_cost[_source] = _Empty_Cost; _que.push(_source); _inque[_source] = true;

		while (!_que.empty())
		{
			Integer _curr = _que.front(); _que.pop(); _inque[_curr] = false;

			for (auto& _edge : _Graph[_curr])
			{
				Integer _next = _edge->_Next_Vertex;

				if (_edge->remain_flow() > _Empty_Flow && _cost[_next] > _cost[_curr] + _edge->_Cost)
				{
					_cost[_next] = _cost[_curr] + _edge->_Cost;
					_rev_path[_next] = _curr;
					_use_edge[_next] = _edge;

					if (!_inque[_next]) { _que.push(_next); _inque[_next] = true; }
				}
			}
		}
		return (_rev_path[_sink] != -1);
	}

	pair<CostType, FlowType> Edmonds_Karp(Integer _source, Integer _sink, vector<CostType> _cost, vector<Boolean> _inque, vector<Integer> _rev_path, vector<Edge*> _use_edge)
	{
		CostType _total_cost = _Empty_Cost; FlowType _total_flow = _Empty_Flow;

		while (1)
		{
			fill(_cost.begin(), _cost.end(), _INF_Cost);
			fill(_inque.begin(), _inque.end(), false);
			fill(_rev_path.begin(), _rev_path.end(), -1);

			if (!SPFA(_source, _sink, _cost, _inque, _rev_path, _use_edge)) break;  // if there is no more increasing path for source -> sink then break

			FlowType _can_flow = _INF_Flow;  // find the minimum flow from the path then let it flow
			for (Integer i = _sink; i != _source; i = _rev_path[i]) _can_flow = min(_can_flow, _use_edge[i]->remain_flow());
			for (Integer i = _sink; i != _source; i = _rev_path[i]) { _total_cost += (_use_edge[i]->_Cost * _can_flow); _use_edge[i]->update_flow(_can_flow); }
			_total_flow += _can_flow;
		}
		return { _total_cost, _total_flow };  // return minmum cost and maximum flow
	}


private:	// Maxflow: Dinic's Algorithm
	bool construct_level_graph(Integer _source, Integer _sink, vector<Integer>& _level)  // construct level graph with bfs
	{
		fill(_level.begin(), _level.end(), -1); 
		queue<int> _que; 
		_level[_source] = 0; _que.push(_source);

		while (!_que.empty())
		{
			Integer _curr_node = _que.front(); _que.pop();

			for (auto& _edge : _Graph[_curr_node])
			{
				Integer _next = _edge->_Next_Vertex;

				if (_level[_next] == -1 && _edge->remain_flow() > _Empty_Flow) {
					_level[_next] = _level[_curr_node] + 1;
					_que.push(_next);
				}
			}
		}
		return (_level[_sink] != -1);
	}

	FlowType find_blocking_flow(Integer _curr_node, Integer _sink, FlowType _curr_flow, vector<Integer>& _level, vector<Integer>& _work)  // find blocking flow with dfs then let it flow
	{
		if (_curr_node == _sink) return _curr_flow;

		for (Integer& i = _work[_curr_node]; i < _Graph[_curr_node].size(); i++)
		{
			Edge* _edge = _Graph[_curr_node][i];
			Integer _next = _edge->_Next_Vertex;

			if (_level[_next] == _level[_curr_node] + 1 && _edge->remain_flow() > _Empty_Flow)
			{
				FlowType _min_Flow = find_blocking_flow(_next, _sink, min(_curr_flow, _edge->remain_flow()), _level, _work);

				if (_min_Flow > _Empty_Flow) {
					_edge->update_flow(_min_Flow);
					return _min_Flow;
				}
			}
		}
		return _Empty_Flow;
	}

	FlowType Dinic(Integer _source, Integer _sink, vector<Integer> _level, vector<Integer> _work)
	{
		FlowType _total_flow = _Empty_Flow;

		while (construct_level_graph(_source, _sink, _level))  // S에서 T까지 도달 할 수 있는 레벨 그래프가 만들어진 경우
		{
			fill(_work.begin(), _work.end(), 0);

			while (1) {
				FlowType _can_flow = find_blocking_flow(_source, _sink, _INF_Flow, _level, _work);
				if (_can_flow == 0) break;  // 레벨 그래프에서 더 이상 차단 유량을 찾을 수 없다면 break
				_total_flow += _can_flow;
			}
		}
		return _total_flow;
	}


public:
	Result_Of_MCMF MCMF_with_Edge_Demands(Integer _source, Integer _sink, Integer _source_for_lower, Integer _sink_for_lower)
	{
		auto _res_of_dinic = Dinic(_source_for_lower, _sink_for_lower, vector<Integer>(_Graph_Size, -1), vector<Integer>(_Graph_Size, 0));
		auto _res_of_mcmf = Edmonds_Karp(_source, _sink, vector<CostType>(_Graph_Size), vector<Boolean>(_Graph_Size), vector<Integer>(_Graph_Size), vector<Edge*>(_Graph_Size, nullptr));
		return Result_Of_MCMF(_res_of_mcmf.first, _res_of_mcmf.second, _res_of_dinic, _Graph);
	}

	/*pair<pair<CostType, FlowType>, vector<FlowType>> MCMF_with_Edge_Demands(Integer _source, Integer _sink, Integer _source_for_lower, Integer _sink_for_lower, FlowType _total_lower_bound_flow)
	{
		FlowType _max_flow_for_lower_bound = Dinic(_source_for_lower, _sink_for_lower, vector<Integer>(_Graph_Size, -1), vector<Integer>(_Graph_Size, 0));
		if (_max_flow_for_lower_bound < _total_lower_bound_flow) return	{ {-1,-1}, vector<FlowType>() };

		pair<CostType, FlowType> _mcmf_result = Edmonds_Karp(_source, _sink, vector<CostType>(_Graph_Size), vector<Boolean>(_Graph_Size), vector<Integer>(_Graph_Size), vector<Edge*>(_Graph_Size, nullptr));
		vector<FlowType> _vertex_to_sink(_Graph_Size);

		for (Integer i = 0; i < _Graph_Size; i++) {
			for (auto& edge : _Graph[i]) _vertex_to_sink[i] += (edge->_Next_Vertex == _sink ? edge->_Curr_Flow : _Empty_Flow);
		}

		return { _mcmf_result,_vertex_to_sink };
	}*/

	// print format
	friend ostream& operator << (std::ostream& _os, MCMFGraph _mcmf_graph) { _os << _mcmf_graph.get_graph_s(); return (_os); }
	string get_graph_s() { return ""; }
};