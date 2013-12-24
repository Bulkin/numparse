#ifndef EXPGEN_H
#define EXPGEN_H

#include <string>
#include <sstream>
#include <functional>
#include <stdexcept>
#include <memory>

#include "dumb_lexer.h"
#include "operations.h"

class ArithmeticObj;

typedef std::unique_ptr<ArithmeticObj> ArithmeticObj_p;

class ArithmeticObj
{
public:
	enum type {
		NIL,
		NUM,
		EXP,
		THUNK,
		CONS
	};
	virtual ~ArithmeticObj() {}
	
	virtual int type() = 0;
	virtual ArithmeticObj_p eval(ArithmeticObj_p lhs = nullptr) = 0;
	virtual std::string print() = 0;
};

class NilObj : public ArithmeticObj
{
public:
	int type() { return NIL; }

	ArithmeticObj_p eval(ArithmeticObj_p lhs) { 
		return lhs; 
	}

	std::string print() { return ""; }
};

class ArithmeticObjGen : public ArithmeticObj
{
public:
	typedef std::function<ArithmeticObj_p(ArithmeticObj_p)> genfunc_t;

	ArithmeticObjGen(genfunc_t func): func(func), obj(nullptr) {}

	int type();
	ArithmeticObj_p eval(ArithmeticObj_p lhs);
	std::string print();
	
private:
	genfunc_t func;
	ArithmeticObj_p obj;
};

class NumObj : public ArithmeticObj
{
	Number value;
public:
	NumObj(Number value): value(value) {}

	int type() { return NUM; }

	ArithmeticObj_p eval(ArithmeticObj_p lhs) {
		return ArithmeticObj_p(new NumObj(value));
	}

	std::string print() {
		std::ostringstream out;
		out << value;
		return out.str();
	}

	static Number getVal(const ArithmeticObj_p &n);
};

class Cons : public ArithmeticObj
{
	ArithmeticObj_p head;
	ArithmeticObj_p tail;
	Cons() {}
public:
	Cons(ArithmeticObj_p head, ArithmeticObj_p tail = nullptr):
		head(std::move(head)),
		tail(std::move(tail)) {}

	int type() { return CONS; }

	ArithmeticObj_p eval(ArithmeticObj_p lhs) {
		if (tail != nullptr) 
			return tail->eval(head->eval(std::move(lhs)));
		else
			return head->eval(std::move(lhs));
	}

	std::string print() { 
		std::ostringstream out;
		if (tail == nullptr)
			out << "(" << head->print() << ")"; 
		else
			out << "(" << head->print() << tail->print() << ")";
		return out.str();
	}

	void setTail(ArithmeticObj_p tail) {
		this->tail = std::move(tail);
	}
};


class Exp : public ArithmeticObj
{
	Op op;
	ArithmeticObj_p rhs;

	Exp();
	Exp(const Exp &exp);
public:
	Exp(Op op, ArithmeticObj_p rhs): op(op),
	                                 rhs(std::move(rhs))
	{}
	                                          
	int type() { return EXP; }
             
	ArithmeticObj_p eval(ArithmeticObj_p lhs);

	std::string print() {
		return op.print() + rhs->print();
	}
};

ArithmeticObj_p make_expression(DumbLexer &lexer,
                                bool wrap = true);

#endif
