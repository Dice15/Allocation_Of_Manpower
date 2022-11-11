#pragma once

#include <fstream>
#include <vector>
#include <string>
#include <regex>
using namespace std;

class CSV_Manager
{
public:
	static vector<string> split_string_buffer(string _text, regex _format = regex(R"()"))   // split by regex
	{
		sregex_token_iterator iter(_text.begin(), _text.end(), _format, -1), end;
		return vector<string>(iter, end);
	}

	static vector<vector<string>> read_csv(string _file_name, regex _format = regex(R"([\W]{1,})"))		// read csv
	{
		vector<vector<string>> _file_data; 
		string _string_buffer;

		fstream _fs(_file_name);
		while (!_fs.eof()) {
			getline(_fs, _string_buffer);
			//cout << _string_buffer << '\n';
			auto _splited_line = split_string_buffer(_string_buffer, _format);
			if (_splited_line.size() > 0 && _splited_line[0] != "") _file_data.push_back(_splited_line);
		}
		_fs.close();

		return _file_data;
	}
};