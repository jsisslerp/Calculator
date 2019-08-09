// -*- C++ -*

/*
 * Calculator provides... TODO:
 */

#ifndef _CALCULATOR
#define _CALCULATOR

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

#include "dynamic_map.h"

#define DMAP_FUNC_DECL(name, T) static T name(const dynamic_map& m)

class Calculator;

// Strategy declarations.

class ValuationStrategy {
public:
	virtual double get_clean_price(const Calculator& c) const = 0;
	virtual double get_pv(const Calculator& c) const = 0;
};

class CleanPriceQuote : public ValuationStrategy {
public:
	virtual double get_clean_price(const Calculator& c) const;
	virtual double get_pv(const Calculator& c) const;
};

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
	const ValuationStrategy* _valuation_strategy;

	dynamic_map _cache;

	// For each strategy virtual method, a private static wrapper function must be
	// defined whose boiler plate function is to extract the calculator and invoke
	// a strategy function.

	typedef pair<int, double> DateDouble;

	DMAP_FUNC_DECL(get_coupon_plus_reset, double);
	DMAP_FUNC_DECL(get_next_call, DateDouble);
	DMAP_FUNC_DECL(get_clean_price, double);
	DMAP_FUNC_DECL(get_pv, double);

public:
	Calculator();

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

	void set_valuation_strategy(const ValuationStrategy* strategy) {
		_valuation_strategy = strategy;
		_cache.clear();
	}

	friend ostream& operator << (ostream& out, const Calculator& c);
};

#endif // CALCULATOR