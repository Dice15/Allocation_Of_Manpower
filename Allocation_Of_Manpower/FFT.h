#pragma once

/****************************************************
*   Fast Fourier Transform (Cooley-Tukey Algorithm)
*   Time Complexity : o(NlogN)
*
*   Author : J.H Moon <jqkt15@naver.com>
*   Date : July 28, 2021
*****************************************************/


#include <vector>
#include <complex>
#include <cmath>
using namespace std;


class Fast_Fourier_Transform
{
public:
	typedef complex<double> Complex;
	typedef long long Integer;
	typedef double Double;
	typedef bool Boolean;


private:
	Integer _Num_Of_Term, _Max_Capacity;
	vector<Complex> _Polynomial_1, _Polynomial_2;


public:
	Fast_Fourier_Transform() :Fast_Fourier_Transform(0) {};
	Fast_Fourier_Transform(Integer _max_term) 	// reserving vector's capacity
		: _Num_Of_Term(_max_term) { _Max_Capacity = 2; while (_Max_Capacity < _max_term) _Max_Capacity <<= 1; _Polynomial_1.reserve(_Max_Capacity); _Polynomial_2.reserve(_Max_Capacity); }
	~Fast_Fourier_Transform() { _Polynomial_1.clear(); _Polynomial_2.clear(); }


private:
	void transform(vector<Complex>& _x, Boolean _is_inv = false)
	{
		Integer _x_size = _x.size();

		for (Integer i = 1, bit, j = 0; i < _x_size; i++) 
		{
			bit = (_x_size >> 1);
			while (j >= bit) { j -= bit; bit >>= 1; }
			j += bit;
			if (i < j) swap(_x[i], _x[j]);
		}

		vector<Complex> _roots(_x_size / 2);
		Double _angle = (_is_inv ? 2.0 : -2.0) * acos(-1) / _x_size;
		for (Integer i = 0; i < _x_size / 2; i++) _roots[i] = Complex(cos(_angle * i), sin(_angle * i));

		for (Integer sz = 2; sz <= _x_size; sz <<= 1) 
		{
			Integer low_sz = sz >> 1, j = _x_size / sz;

			for (Integer i = 0; i < _x_size; i += sz) 
			{
				for (Integer k = 0; k < low_sz; k++)
				{
					Complex even = _x[i + k], odd = _roots[j * k] * _x[i + k + low_sz];
					_x[i + k] = even + odd;
					_x[i + k + low_sz] = even - odd;
				}
			}
		}

		if (_is_inv) for (Integer i = 0; i < _x_size; i++) _x[i] /= _x_size;
	}


public:
	void FFT(vector<Complex>& _x) { transform(_x); }

	void IFFT(vector<Complex>& _X) { transform(_X, true); }

	string Fast_Multiply(string _num1, string _num2)
	{
		if (_num1.size() == 0 || _num2.size() == 0) return "0";

		Integer _max_size = 2, _max_term = 2 * max(_num1.size(), _num2.size()) - 1;
		while (_max_size < _max_term) _max_size <<= 1;

		_Polynomial_1.assign(_max_size, 0), _Polynomial_2.assign(_max_size, 0);
		for (Integer i = _num1.size() - 1, j = 0; i >= 0; i--, j++) _Polynomial_1[j] = (_num1[i] - '0');
		for (Integer i = _num2.size() - 1, j = 0; i >= 0; i--, j++) _Polynomial_2[j] = (_num2[i] - '0');

		FFT(_Polynomial_1); FFT(_Polynomial_2);
		for (Integer i = 0; i < _max_size; i++)  _Polynomial_1[i] *= _Polynomial_2[i];
		IFFT(_Polynomial_1);

		vector<Integer> _temp_res(_max_size + 1);
		for (Integer i = 0; i < _max_size; i++) _temp_res[i] = (Integer)round(real(_Polynomial_1[i]));
		for (Integer i = 0; i < _max_size; i++) { _temp_res[i + 1] += _temp_res[i] / 10; _temp_res[i] %= 10; }

		string _res;
		for (Integer i = _max_size - 1; i >= 0; i--) {
			if (_temp_res[i]) { for (Integer j = i; j >= 0; j--) _res += _temp_res[j] + '0'; break; }
		}
		return _res;
	}
};