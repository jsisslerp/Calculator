// Calculator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

#include <assert.h>

#include "dynamic_map.h"

typedef vector<pair<int, double> > CallSchedule;

class Calculator;

// Definitions of all the dynamic map keys.

dynamic_map_key<double> DMK_COUPON("COUPON", 5);
dynamic_map_key<double> DMK_LAST_RESET("LAST_RESET", 2.5);
dynamic_map_key<double> DMK_COUPON_PLUS_RESET("COUPON_PLUS_RESET", 0);
dynamic_map_key<Calculator*> DMK_CALCULATOR("CALCULATOR", 0);
dynamic_map_key<CallSchedule> DMK_CALL_SCHEDULE("CALL_SCHEDULE", CallSchedule());
dynamic_map_key<pair<int, double> > DMK_NEXT_CALL("NEXT_CALL", pair<int, double>());
dynamic_map_key<int> DMK_VALUE_DATE("VALUE_DATE", 20150601);

// Example polymorhpic strategy: coupon plus reset. 

class CouponPlusResetStrategy {
public:
	virtual double get_coupon_plus_reset(const Calculator& c) const;
};

class NormalizedCouponPlusResetStrategy : public CouponPlusResetStrategy {
public:
	virtual double get_coupon_plus_reset(const Calculator& c) const;
};

class DateStrategy {
public:
	virtual pair<int, double> get_next_call(const Calculator& c) const;
};

// Calculator class has fields to hold a pointer to each strategy and a dynamic map cache.

class Calculator {
private:
	const CouponPlusResetStrategy* _coupon_plus_reset_strategy;
	const DateStrategy* _date_strategy;

	dynamic_map _cache;

	// For each strategy virtual method, a private static wrapper function must be
	// defined whose boiler plate function is to extract the calculator and invoke
	// a strategy function.

	static double get_coupon_plus_reset(const dynamic_map& m) {
		const Calculator* _this = m.get(DMK_CALCULATOR);
		return _this->_coupon_plus_reset_strategy->get_coupon_plus_reset(*_this);
	}

	static pair<int, double> get_next_call(const dynamic_map& m) {
		const Calculator* _this = m.get(DMK_CALCULATOR);
		return _this->_date_strategy->get_next_call(*_this);
	}

public:
	Calculator() : 
		_coupon_plus_reset_strategy(new CouponPlusResetStrategy),
		_date_strategy(new DateStrategy),
		_cache() {
		_cache.set(DMK_CALCULATOR, this);
		_cache.set(DMK_COUPON_PLUS_RESET, &get_coupon_plus_reset);
		_cache.set(DMK_NEXT_CALL, &get_next_call);
	}

	template <class T>
	void set(dynamic_map_key<T>& key, T data) {
		_cache.set(key, data);
	}

	template <class T>
	T get(dynamic_map_key<T>& key) const {
		return _cache.get(key);
	}

	template <class T>
	bool has(dynamic_map_key<T>& key) const {
		return _cache.has(key);
	}

	void set_coupon_plus_reset_strategy(const CouponPlusResetStrategy* strategy) {
		_coupon_plus_reset_strategy = strategy;
		_cache.clear();
	}
};

double CouponPlusResetStrategy::get_coupon_plus_reset(const Calculator& c) const {
	return c.get(DMK_COUPON) + c.get(DMK_LAST_RESET);
}

double NormalizedCouponPlusResetStrategy::get_coupon_plus_reset(const Calculator& c) const {
	return CouponPlusResetStrategy::get_coupon_plus_reset(c) * 0.01;
}

pair<int, double> DateStrategy::get_next_call(const Calculator& c) const {
	const CallSchedule& cs = c.get(DMK_CALL_SCHEDULE);
	int vd = c.get(DMK_VALUE_DATE);
	auto it = find_if(cs.begin(), cs.end(), [vd](pair<int, double> p) { return p.first > vd; });
	return *it;
}

int main()
{
	Calculator calc;

	// Initialize "static" data and strategies.
	calc.set_coupon_plus_reset_strategy(new CouponPlusResetStrategy);
	calc.set(DMK_COUPON, 10.5);
	calc.set(DMK_LAST_RESET, 2.5);

	// Output result computed by strategy, e.g. 13.0
	cout << calc.get(DMK_COUPON_PLUS_RESET) << endl;

	// Change the strategy and data.
	calc.set_coupon_plus_reset_strategy(new NormalizedCouponPlusResetStrategy);
	calc.set(DMK_COUPON, 5.0);

	// Output result computed by strategy, e.g. .075
	cout << calc.get(DMK_COUPON_PLUS_RESET) << endl;

	// Complex data.
	CallSchedule cs;
	cs.push_back(pair<int, double>(20150101, 102.0));
	cs.push_back(pair<int, double>(20160101, 101.0));

	calc.set(DMK_CALL_SCHEDULE, cs);
	assert(calc.get(DMK_CALL_SCHEDULE) == cs);

	calc.set(DMK_VALUE_DATE, 20150601);
	assert(calc.get(DMK_NEXT_CALL).first == 20160101);
}
