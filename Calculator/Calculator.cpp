// Calculator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
using namespace std;

#include "dynamic_map.h"

class Calculator;

// Definitions of all the dynamic map keys.

dynamic_map_key<double> DMK_COUPON("COUPON", 5);
dynamic_map_key<double> DMK_LAST_RESET("LAST_RESET", 2.5);
dynamic_map_key<double> DMK_COUPON_PLUS_RESET("COUPON_PLUS_RESET", 0);
dynamic_map_key<Calculator*> DMK_CALCULATOR("CALCULATOR", 0);

// Example polymorhpic strategy: coupon plus reset. 

class CouponPlusResetStrategy {
public:
	virtual double get_coupon_plus_reset(const dynamic_map& m) const {
		return m.get(DMK_COUPON) + m.get(DMK_LAST_RESET);
	}
};

class NormalizedCouponPlusResetStrategy : public CouponPlusResetStrategy {
public:
	virtual double get_coupon_plus_reset(const dynamic_map& m) const {
		return (m.get(DMK_COUPON) + m.get(DMK_LAST_RESET)) * 0.01;
	}
};

// Calculator class has fields to hold a pointer to each strategy and a dynamic map cache.

class Calculator {
private:
	const CouponPlusResetStrategy* _coupon_plus_reset_strategy;
	dynamic_map _cache;

	// For each strategy virtual method, a private static wrapper function must be
	// defined whose boiler plate function is to extract the calculator .

	static double get_coupon_plus_reset(const dynamic_map& m) {
		Calculator* _this = m.get(DMK_CALCULATOR);
		return _this->_coupon_plus_reset_strategy->get_coupon_plus_reset(_this->_cache);
	}

public:
	Calculator() :
		_coupon_plus_reset_strategy(0),
		_cache() {
		_cache.set(DMK_CALCULATOR, this);
		_cache.set(DMK_COUPON_PLUS_RESET, &get_coupon_plus_reset);
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
}
