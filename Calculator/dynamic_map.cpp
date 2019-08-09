#include "dynamic_map.h"

#include <cmath>
using namespace std;

void dynamic_map::_set_data(string key, void* data) {
	_data_map[key] = data;
	_return_map.clear();
}

void* dynamic_map::_get_data(string key) const {
	auto it = _data_map.find(key);
	if (it != _data_map.end())
		return it->second;
	throw runtime_error(string("dynamic_map::_get_data: missing data for key: ") + key);
}

bool dynamic_map::_has_data(string key) const {
	return _data_map.find(key) != _data_map.end();
}

bool dynamic_map::_has_function(string key) const {
	return _function_map.find(key) != _function_map.end();
}

void* dynamic_map::_get_function(string key) const {
	auto it = _function_map.find(key);
	if (it != _function_map.end())
		return it->second;
	throw runtime_error(string("dynamic_map::_get_function: missing function for key: ") + key);
}

void dynamic_map::_set_function(string key, void* function) {
	_function_map[key] = function;
	_return_map.clear();
}

void dynamic_map::_set_return(string key, void* data) {
	_return_map[key] = data;
}

void* dynamic_map::_get_return(string key) const {
	auto it = _return_map.find(key);
	if (it != _return_map.end())
		return it->second;
	throw runtime_error(string("dynamic_map::_get_return: missing function result for key: ") + key);
}

bool dynamic_map::_has_return(string key) const {
	return _return_map.find(key) != _return_map.end();
}

ostream& operator<<(ostream& os, const dynamic_map& m) {
	os << "dynamic_map (" << &m << "):\n";

	os << "\t_data_map:" << endl;
	for (auto it = m._data_map.begin(); it != m._data_map.end(); it++) {
		os << "\t\t" << it->first << ":\t" << it->second << endl;
	}

	os << "\t_function_map:" << endl;
	for (auto it = m._function_map.begin(); it != m._function_map.end(); it++) {
		os << "\t\t" << it->first << ":\t" << it->second << endl;
	}

	os << "\t_return_map:" << endl;
	for (auto it = m._return_map.begin(); it != m._return_map.end(); it++) {
		os << "\t\t" << it->first << ":\t" << it->second << endl;
	}

	return os;
}
