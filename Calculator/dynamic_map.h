// -*- C++ -*-

/*
 * dynamic_map provides a generic interface for managing a cache of blah blah blah...
 */

#ifndef _DYNAMIC_MAP
#define _DYNAMIC_MAP

#include <string>
#include <map>
using namespace std;

template <class T>
class dynamic_map_key {
private:
	const string _name;
	const T      _default_value;

public:
	dynamic_map_key(string name, T default_value) :
		_name(name),
		_default_value(default_value) {
	}

	string   get_name()          const { return _name; }
	const T& get_default_value() const { return _default_value; }
};

class dynamic_map {
public:
	dynamic_map() :
		_data_map(),
		_function_map(),
		_return_map() {
	}

	template <class T>
	void set(dynamic_map_key<T>& key, T data) {
		_set_data(key.get_name(), new T(data));
		_return_map.clear();
	}

	template <class T>
	T get(dynamic_map_key<T>& key) const {
		if (_has_data(key.get_name()))
			return *(T*)_get_data(key.get_name());
		else if (_has_return(key.get_name()))
			return *(T*)_get_return(key.get_name());
		else if (_has_function(key.get_name())) {
			T(*function)(const dynamic_map&) = reinterpret_cast<T(*)(const dynamic_map&)>(_get_function(key.get_name()));
			T returned = function(*this);
			const_cast<dynamic_map*>(this)->_set_return(key.get_name(), new T(returned));
			return returned;
		}
		else
			return key.get_default_value();
	}

	template <class T>
	bool has(dynamic_map_key<T>& key) const {
		return _has_data(key.get_name()) || _has_function(key.get_name());
	}

	template <class T>
	void set(dynamic_map_key<T>& key, T(*function)(const dynamic_map&)) {
		_set_function(key.get_name(), reinterpret_cast<void*>(function));
		_return_map.clear();
	}

	void clear() {
		_return_map.clear();
	}

private:
	map<string, void*> _data_map, _function_map, _return_map;

	void  _set_data(string key, void* data);
	void* _get_data(string key) const;
	bool  _has_data(string key) const;

	void  _set_function(string key, void* function);
	void* _get_function(string key) const;
	bool  _has_function(string key) const;

	void  _set_return(string key, void* data);
	void* _get_return(string key) const;
	bool  _has_return(string key) const;
};

#endif // _DYNAMIC_MAP
