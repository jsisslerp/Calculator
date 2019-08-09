// Calculator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

#include <assert.h>

#include "Calculator.h"

typedef vector<pair<int, double> > CallSchedule;

// Definitions of all the dynamic map keys.

dynamic_map_key<double> DMK_COUPON("COUPON", 5);
dynamic_map_key<double> DMK_FACE_AMOUNT("FACE_AMOUNT", 1000000.0);
dynamic_map_key<double> DMK_CLEAN_PRICE("CLEAN_PRICE", 100.0);
dynamic_map_key<double> DMK_PV("PV", 0.0);
dynamic_map_key<double> DMK_LAST_RESET("LAST_RESET", 2.5);
dynamic_map_key<double> DMK_COUPON_PLUS_RESET("COUPON_PLUS_RESET", 0);
dynamic_map_key<Calculator*> DMK_CALCULATOR("CALCULATOR", 0);
dynamic_map_key<CallSchedule> DMK_CALL_SCHEDULE("CALL_SCHEDULE", CallSchedule());
dynamic_map_key<pair<int, double> > DMK_NEXT_CALL("NEXT_CALL", pair<int, double>());
dynamic_map_key<int> DMK_VALUE_DATE("VALUE_DATE", 20150601);

// Example polymorhpic strategy: coupon plus reset. 

Calculator::Calculator() :
	_coupon_plus_reset_strategy(new CouponPlusResetStrategy),
	_date_strategy(new DateStrategy),
	_valuation_strategy(new CleanPriceQuote),
	_cache() {
	_cache.set(DMK_CALCULATOR, this);
	_cache.set(DMK_COUPON_PLUS_RESET, &get_coupon_plus_reset);
	_cache.set(DMK_NEXT_CALL, &get_next_call);
	_cache.set(DMK_CLEAN_PRICE, &get_clean_price);
	_cache.set(DMK_PV, &get_pv);
}

// dynamic cache wrapper function definitions

double Calculator::get_coupon_plus_reset(const dynamic_map& m) {
	const Calculator* _this = m.get(DMK_CALCULATOR);
	return _this->_coupon_plus_reset_strategy->get_coupon_plus_reset(*_this);
}

pair<int, double> Calculator::get_next_call(const dynamic_map& m) {
	const Calculator* _this = m.get(DMK_CALCULATOR);
	return _this->_date_strategy->get_next_call(*_this);
}

double Calculator::get_clean_price(const dynamic_map& m) {
	const Calculator* _this = m.get(DMK_CALCULATOR);
	return _this->_valuation_strategy->get_clean_price(*_this);
}

double Calculator::get_pv(const dynamic_map& m) {
	const Calculator* _this = m.get(DMK_CALCULATOR);
	return _this->_valuation_strategy->get_pv(*_this);
}

ostream& operator << (ostream& out, const Calculator& c) {
	out << c._cache << endl;
	return out;
}

double CleanPriceQuote::get_clean_price(const Calculator& c) const {
	return c.get(DMK_CLEAN_PRICE);
}

double CleanPriceQuote::get_pv(const Calculator& c) const {
	return c.get(DMK_CLEAN_PRICE) * 0.01 * c.get(DMK_FACE_AMOUNT);
}

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

	calc.set(DMK_CLEAN_PRICE, 101.375);
	calc.set_valuation_strategy(new CleanPriceQuote);
	calc.set(DMK_FACE_AMOUNT, 1000000.0);
	cout << calc.get(DMK_PV) << endl;

	cout << calc << endl;
}
