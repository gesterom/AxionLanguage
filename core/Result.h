#pragma once

#include "TODO.h"

#pragma warning(push)
#pragma warning(disable : 26495)
template<typename R, typename E>
class Result {
	bool isGood;
	union Value {
		R res;
		E err;
		Value() {}
		~Value() {} // Destructor to call destructors of R and E if needed
	};
	Value val;
public:
	
	Result(const Result<R, E>& other) {
		if (other.isGood) {
			this->isGood = true;
			this->val.res = other.val.res;
		}
		else {
			this->isGood = false;
			this->val.err = other.val.err;
		}
	}
	//Result<R, E>& operator=(const Result<R, E>& other) {
	//	if (other.isGood) {
	//		this->isGood = true;
	//		this->val.res = other.val.res;
	//	}
	//	else {
	//		this->isGood = false;
	//		this->val.err = other.val.err;
	//	}
	//	return *this;
	//}

	Result(R r) : isGood(true) {
		new(&val.res) R(r);
	}
	Result(E e) : isGood(false) {
		new(&val.err) E(e);
	}
	operator bool() const {
		return isGood;
	}

	// Implicit cast to R
	operator R() {
		ASSERT(isGood, "implicit cast to R when bad");
		return val.res;
	}

	// Implicit cast to E
	operator E() {
		ASSERT(not isGood, "implicit cast to E when good");
		return val.err;
	}
	operator const R() const {
		ASSERT(isGood, "implicit cast to R when bad");
		return val.res;
	}

	// Implicit cast to E
	operator const E() const {
		ASSERT(not isGood, "implicit cast to E when good");
		return val.err;
	}
	~Result() {
		if (isGood) {
			val.res.~R(); // Manually call the destructor of R if isGood
		}
		else {
			val.err.~E(); // Manually call the destructor of E if not isGood
		}
	}
};
#pragma warning(pop)