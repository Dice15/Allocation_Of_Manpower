#pragma once

#include <string>
#include <vector>
using namespace std;


class Polynomial
{
public:
	typedef long long Integer;
	typedef bool Boolean;

private:
	Integer _Num_Of_Term, _X_Value;
	vector<Integer> _Coefficients;

public:
	Polynomial() :Polynomial(0, 0) {}
	Polynomial(Integer _num_of_term) :_Num_Of_Term(_num_of_term), _X_Value(_num_of_term + 1), _Coefficients(vector<Integer>(_num_of_term)) {}
	Polynomial(Integer _num_of_term, Integer _x_value) :_Num_Of_Term(_num_of_term), _X_Value(_x_value), _Coefficients(vector<Integer>(_num_of_term)) {}
	Polynomial(vector<Integer> _coefficients) :_Num_Of_Term(_coefficients.size()), _X_Value(_coefficients.size() + 1), _Coefficients(_coefficients) {}
	Polynomial(vector<Integer> _coefficients, Integer _x_value) :_Num_Of_Term(_coefficients.size()), _X_Value(_x_value), _Coefficients(_coefficients) {}
	~Polynomial() { _Coefficients.clear(); }

	// binary operator: both polynomial's number of terms and x-value are always same
	Polynomial operator+(Polynomial& const _other) {
		Polynomial _result(_Num_Of_Term, _X_Value);
		for (int i = 0; i < _Num_Of_Term; i++) _result._Coefficients[i] = _Coefficients[i] + _other._Coefficients[i];
		return _result;
	}
	Polynomial operator-(Polynomial& const _other) {
		Polynomial _result(_Num_Of_Term, _X_Value);
		for (int i = 0; i < _Num_Of_Term; i++) _result._Coefficients[i] = _Coefficients[i] - _other._Coefficients[i];
		return _result;
	}
	Polynomial operator*(Integer const _integer) {
		Polynomial _result(_Num_Of_Term, _X_Value);
		for (int i = 0; i < _Num_Of_Term; i++) _result._Coefficients[i] = _Coefficients[i] * _integer;
		return _result;
	}
	Polynomial operator/(Integer const _integer) {
		Polynomial _result(_Num_Of_Term, _X_Value);
		for (int i = 0; i < _Num_Of_Term; i++) _result._Coefficients[i] = _Coefficients[i] / _integer;
		return _result;
	}
	Polynomial& operator+=(Polynomial& const _other) {
		for (int i = 0; i < _Num_Of_Term; i++) _Coefficients[i] += _other._Coefficients[i];
		return *this;
	}
	Polynomial& operator-=(Polynomial& const _other) {
		for (int i = 0; i < _Num_Of_Term; i++) _Coefficients[i] -= _other._Coefficients[i];
		return *this;
	}
	Polynomial& operator*=(Integer const _integer) {
		for (int i = 0; i < _Num_Of_Term; i++) _Coefficients[i] *= _integer;
		return *this;
	}
	Polynomial& operator/=(Integer const _integer) {
		for (int i = 0; i < _Num_Of_Term; i++) _Coefficients[i] /= _integer;
		return *this;
	}

	// unary operator
	Polynomial operator-() {
		Polynomial _result(_Num_Of_Term, _X_Value);
		for (int i = 0; i < _Num_Of_Term; i++) _result._Coefficients[i] = -_Coefficients[i];
		return _result;
	}

	Integer get_value() {
		Integer _result = 0;
		for (Integer _nth = _Num_Of_Term - 1; _nth > 0; --_nth) _result = (_result + _Coefficients[_nth]) * _X_Value;
		return _result + _Coefficients[0];
	}
	string get_value_s() { return to_string(get_value()); }
};