// -*- C++ -*

/*
 * dynamic_map provides a generic interface for managing a cache of typed data and functions. Map keys are instances of the generic
 * class dynamic_map_key which are labeled with a unique string and are parameterized by a single type T. The dynamic_map interface
 * consists of the classic "get, set, has" functions. Both data and functions may be written to the map. When reading from the map, the
 * internal "data" cache is examined first, followed by the "return" cache which holds values returned from cached functions, and lastly
 * the function cache. A function cache hit results in the function being evaluated and it's result saved to the "return" cache. If no
 * data or functions are cached under a given key, then the key's default value is returned. Any set on a dynamic_map will cause the 
 * function return cache to be cleared to ensure that any possible dependencies on the changed data will be re-evaluated. 
 */

#ifndef _DYNAMIC_MAP
#define _DYNAMIC_MAP

#include <string>
#include <map>
#include <iostream>
using namespace std;

template <class T>
class dynamic_map_key {
private:
	const string _name;
	const T      _default_value;

public:
	dynamic_map_key(string name, const T& default_value) :
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
	void set(const dynamic_map_key<T>& key, const T& data) {
		_set_data(key.get_name(), new T(data));
		_return_map.clear();
	}

	template <class T>
	T get(const dynamic_map_key<T>& key) const {
		if (_has_data(key.get_name()))
			return *reinterpret_cast<T*>(_get_data(key.get_name()));
		else if (_has_return(key.get_name()))
			return *reinterpret_cast<T*>(_get_return(key.get_name()));
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
	bool has(const dynamic_map_key<T>& key) const {
		return _has(key.get_name());
	}

	template <class T>
	void set(const dynamic_map_key<T>& key, T(*function)(const dynamic_map&)) {
		_set_function(key.get_name(), reinterpret_cast<void*>(function));
		_return_map.clear();
	}

	void clear() {
		_return_map.clear();
	}

	friend std::ostream& operator<<(std::ostream& os, const dynamic_map& m);

private:
	map<string, void*> _data_map, _function_map, _return_map;

	bool _has(string key) const {
		return _has_data(key) || _has_return(key) || _has_function(key);
	}

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
