#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <functional>
#include <string>

typedef double Number;

class Op
{
public:
	typedef std::function<Number(Number, Number)> func_t;
	enum priority {
		NOP,
		ADDITION,
		MULTIPLICATION
	};

	Op(std::string name, priority priority, func_t func): name(name),
	                                                      priority(priority),
	                                                      func(func) {}
	
	Number perform(const Number &lhs, const Number &rhs) {
		return func(lhs, rhs);
	}

	Number operator()(const Number &lhs, const Number &rhs) {
		return perform(lhs,	rhs); 
	}

	std::string print() const { return name; }

	int get_priority() const { return priority; }
	
	static Op make(std::string name);
private:
	func_t func;
	std::string name;
	priority priority;
};

extern const Op nop;

#endif
