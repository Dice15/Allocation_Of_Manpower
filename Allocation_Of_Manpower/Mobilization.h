#pragma once

#include <string>
#include <vector>
#include <iostream>
#include "FFT.h"
#include "Polynomial.h"
using namespace std;


typedef long long Personnel;

class PriorityLevel
{
public:
	typedef long long Integer;
	typedef bool Boolean;
	enum VALUETYPE { VARIABLE, MINIMUM, MAXIMUM };
	enum ACCIDENT { PROBABILITY = 5, SCALE = 5 };


private:
	Fast_Fourier_Transform _FFT;	// fast fourier transform for polynomial's operation

	Integer _State;					// variable or const(const value min, max)
	Polynomial _Accident_Probability, _Accident_Scale;


public:
	PriorityLevel() :_State(VARIABLE), _Accident_Probability(Polynomial(PROBABILITY)), _Accident_Scale(Polynomial(SCALE)) {}
	PriorityLevel(Integer _state) :_State(_state), _Accident_Probability(Polynomial(PROBABILITY)), _Accident_Scale(Polynomial(SCALE)) {}
	PriorityLevel(Integer _state, Polynomial _probability, Polynomial _scale) :_State(_state), _Accident_Probability(_probability), _Accident_Scale(_scale) {}

	// binary operator
	Boolean operator<(PriorityLevel _other) {
		if (_State == VARIABLE && _other._State == VARIABLE) {
			string _res1 = _FFT.Fast_Multiply(_Accident_Probability.get_value_s(), _Accident_Scale.get_value_s());
			string _res2 = _FFT.Fast_Multiply(_other._Accident_Probability.get_value_s(), _other._Accident_Scale.get_value_s());
			return _res1.size() == _res2.size() ? _res1 < _res2 : _res1.size() < _res2.size();
		}
		return _State == VARIABLE ? _other._State == MAXIMUM : _other._State == VARIABLE ? _State == MINIMUM : _State < _other._State;
	}
	Boolean operator<=(PriorityLevel _other) {
		if (_State == VARIABLE && _other._State == VARIABLE) {
			string _res1 = _FFT.Fast_Multiply(_Accident_Probability.get_value_s(), _Accident_Scale.get_value_s());
			string _res2 = _FFT.Fast_Multiply(_other._Accident_Probability.get_value_s(), _other._Accident_Scale.get_value_s());
			return _res1.size() == _res2.size() ? _res1 <= _res2 : _res1.size() <= _res2.size();
		}
		return _State == VARIABLE ? _other._State == MAXIMUM : _other._State == VARIABLE ? _State == MINIMUM : _State <= _other._State;
	}
	Boolean operator>(PriorityLevel _other) {
		if (_State == VARIABLE && _other._State == VARIABLE) {
			string _res1 = _FFT.Fast_Multiply(_Accident_Probability.get_value_s(), _Accident_Scale.get_value_s());
			string _res2 = _FFT.Fast_Multiply(_other._Accident_Probability.get_value_s(), _other._Accident_Scale.get_value_s());
			return _res1.size() == _res2.size() ? _res1 > _res2 : _res1.size() > _res2.size();
		}
		return _State == VARIABLE ? _other._State == MINIMUM : _other._State == VARIABLE ? _State == MAXIMUM : _State > _other._State;
	}
	Boolean operator>=(PriorityLevel _other) {
		if (_State == VARIABLE && _other._State == VARIABLE) {
			string _res1 = _FFT.Fast_Multiply(_Accident_Probability.get_value_s(), _Accident_Scale.get_value_s());
			string _res2 = _FFT.Fast_Multiply(_other._Accident_Probability.get_value_s(), _other._Accident_Scale.get_value_s());
			return _res1.size() == _res2.size() ? _res1 >= _res2 : _res1.size() >= _res2.size();
		}
		return _State == VARIABLE ? _other._State == MINIMUM : _other._State == VARIABLE ? _State == MAXIMUM : _State >= _other._State;
	}
	Boolean operator==(PriorityLevel _other) {
		if (_State == VARIABLE && _other._State == VARIABLE) {
			string _res1 = _FFT.Fast_Multiply(_Accident_Probability.get_value_s(), _Accident_Scale.get_value_s());
			string _res2 = _FFT.Fast_Multiply(_other._Accident_Probability.get_value_s(), _other._Accident_Scale.get_value_s());
			return _res1 == _res2;
		}
		return _State == _other._State;
	}
	PriorityLevel operator+(PriorityLevel _other) {
		return _State == VARIABLE && _other._State == VARIABLE ? PriorityLevel(VARIABLE, _Accident_Probability + _other._Accident_Probability, _Accident_Scale + _other._Accident_Scale)
			: _State == VARIABLE ? PriorityLevel(_other._State, Polynomial(PROBABILITY), Polynomial(SCALE))
			: _other._State == VARIABLE ? PriorityLevel(_State, Polynomial(PROBABILITY), Polynomial(SCALE))
			: _State == _other._State ? PriorityLevel(_State, Polynomial(PROBABILITY), Polynomial(SCALE)) : PriorityLevel();
	}
	PriorityLevel operator-(PriorityLevel _other) {
		return _State == VARIABLE && _other._State == VARIABLE ? PriorityLevel(VARIABLE, _Accident_Probability - _other._Accident_Probability, _Accident_Scale - _other._Accident_Scale)
			: _State == VARIABLE ? PriorityLevel(_other._State == MINIMUM ? MAXIMUM : MINIMUM, Polynomial(PROBABILITY), Polynomial(SCALE))
			: _other._State == VARIABLE ? PriorityLevel(_State, Polynomial(PROBABILITY), Polynomial(SCALE))
			: _State == _other._State ? PriorityLevel() : PriorityLevel(_State, Polynomial(PROBABILITY), Polynomial(SCALE));
	}
	PriorityLevel operator*(Integer const _integer) {
		return _State == VARIABLE ? PriorityLevel(VARIABLE, _Accident_Probability * _integer, _Accident_Scale * _integer) : PriorityLevel(_State, Polynomial(PROBABILITY), Polynomial(SCALE));
	}
	PriorityLevel& operator+=(PriorityLevel _other) {

		if (_State == VARIABLE && _other._State == VARIABLE) { _Accident_Probability += _other._Accident_Probability; _Accident_Scale += _other._Accident_Scale; }
		else if (_State == VARIABLE) { _State = _other._State; _Accident_Probability = Polynomial(PROBABILITY); _Accident_Scale = Polynomial(SCALE); }
		else if (_other._State == VARIABLE) {}
		else if (_State == _other._State) {}
		else _State = VARIABLE;
		return *this;
	}
	PriorityLevel& operator-=(PriorityLevel _other) {

		if (_State == VARIABLE && _other._State == VARIABLE) { _Accident_Probability -= _other._Accident_Probability; _Accident_Scale -= _other._Accident_Scale; }
		else if (_State == VARIABLE) { _State = _other._State == MINIMUM ? MAXIMUM : MINIMUM; _Accident_Probability = Polynomial(PROBABILITY); _Accident_Scale = Polynomial(SCALE); }
		else if (_other._State == VARIABLE) {}
		else if (_State == _other._State) _State = VARIABLE;
		else {}
		return *this;
	}
	PriorityLevel& operator*=(Integer const _integer) {
		if (_State == VARIABLE) { _Accident_Probability *= 3; _Accident_Scale *= _integer; }
		return *this;
	}
	PriorityLevel& operator/=(Integer const _integer) {
		if (_State == VARIABLE) { _Accident_Probability /= 3; _Accident_Scale /= _integer; }
		return *this;
	}

	// unary operator
	PriorityLevel operator-() {
		return _State == VARIABLE ? PriorityLevel(VARIABLE, -_Accident_Probability, -_Accident_Scale) : PriorityLevel(_State == MINIMUM ? MAXIMUM : MINIMUM, Polynomial(PROBABILITY), Polynomial(SCALE));
	}

	// print format
	friend ostream& operator << (std::ostream& out, PriorityLevel p) { cout << p.get_value_s(); return (out); }
	string get_value_s() { return _State == VARIABLE ? _FFT.Fast_Multiply(_Accident_Probability.get_value_s(), _Accident_Scale.get_value_s()) : _State == MINIMUM ? "-INF" : "INF"; }
};
