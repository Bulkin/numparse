#include "expgen.h"

#include <unordered_map>
#include <iostream>

using namespace std;

#define AO_p ArithmeticObj_p

namespace {
	enum token_category {
		OPERATION,
		NUMBER,
		PAREN_O,
		PAREN_C
	};
	
	const unordered_map<string, token_category> token_map = {
		{ "plus", OPERATION },
		{ "minus", OPERATION },
		{ "mult", OPERATION },
		{ "div", OPERATION },
		{ "natural", NUMBER },
		{ "int", NUMBER },
		{ "float", NUMBER },
		{ "paren_o", PAREN_O },
		{ "paren_c", PAREN_C }
	};

	ArithmeticObj_p make_thunk(DumbLexer &lexer)
	{
		return AO_p(new 
		            ArithmeticObjGen([&lexer](ArithmeticObj_p lhs){
				            return make_expression(lexer);
			            }));
	}

	ArithmeticObj_p make_number(string num)
	{
		istringstream in(num);
		Number n;
		in >> n;
		return AO_p(new NumObj(n));
	}

	bool check_next_op_priority(const Op &op, DumbLexer &lexer)
	{
		string next_op = lexer.peek_token();
		if (next_op != "") 
			if (Op::make(next_op).get_priority() > op.get_priority())
				return true;
		return false;
	}
	
	ArithmeticObj_p make_priority_selector(Op op, DumbLexer &lexer)
	{
		return AO_p(new
		            ArithmeticObjGen([op, &lexer](ArithmeticObj_p lhs){
				            if (check_next_op_priority(op, lexer))
					            return make_thunk(lexer);
				            else
					            return AO_p(new NilObj());
			            }));
	}
}

ArithmeticObj_p make_expression(DumbLexer &lexer,
                                bool wrap)
{
	string token = lexer.next_token();
	
	if (token == "") // no tokens left
		return ArithmeticObj_p(new NilObj());

	string tok_tp = lexer.get_last_token_type();

	//	cout << ":" << token << ":" << tok_tp << endl;
	
	ArithmeticObj_p head;
	ArithmeticObj_p tail = nullptr;

	if (token_map.at(tok_tp) == NUMBER) {
		head = make_number(token);

	} else if (token_map.at(tok_tp) == OPERATION) {
		Op op = Op::make(token);
		ArithmeticObj_p num = make_expression(lexer, false);

		// arithmetic expression rules are stupid
		// workarounds for operation priority handling
		switch (num->type()) {
		case ArithmeticObj::NUM:
			if (check_next_op_priority(op, lexer))
				num = ArithmeticObj_p(new Cons(move(num), make_thunk(lexer)));
			head = ArithmeticObj_p(new Exp(op, move(num)));
			break;
				
		case ArithmeticObj::CONS:
			head = AO_p(new Exp(op, 
			                    (AO_p(new Cons(move(num),
			                                   make_priority_selector(op, lexer))))));
			break;
				
		default:
			throw runtime_error("Expected value after operation");
		}

	} else if (token_map.at(tok_tp) == PAREN_O) {
		head = make_expression(lexer);

	} else if (token_map.at(tok_tp) == PAREN_C) {
		return ArithmeticObj_p(new NilObj());
	}
	
	if (wrap) {
		if (tail == nullptr)
			tail = move(make_thunk(lexer));
		return ArithmeticObj_p(new Cons(move(head), move(tail)));
	} else
		return head;
}

Number NumObj::getVal(const ArithmeticObj_p &n)
{
	if (n->type() == NUM) {
		return (dynamic_cast<NumObj*>(n.get()))->value;
	} else
		throw runtime_error("Not a number");
}

ArithmeticObj_p Exp::eval(ArithmeticObj_p lhs) 
{
	Number l, r;
	
	if (lhs == nullptr)
		if (op.print() == "-")
			lhs = ArithmeticObj_p(new NumObj(0));
		else
			throw std::runtime_error("Can't apply op to NULL");
	
	if (lhs->type() == NUM) {
		l = NumObj::getVal(lhs);
		r = NumObj::getVal(rhs->eval());
	} else
		throw runtime_error("Can't evaluate expression");

	return ArithmeticObj_p(new NumObj(op(l,r)));
	
}

int ArithmeticObjGen::type()
{ 
	if (obj == nullptr)
		return THUNK; 
	else
		return obj->type();
}

ArithmeticObj_p ArithmeticObjGen::eval(ArithmeticObj_p lhs) { 
	if (obj == nullptr) {
		obj = func(lhs->eval());
	}
	return obj->eval(std::move(lhs));
}

string ArithmeticObjGen::print() { 
	if (obj == nullptr)
		return string("<thunk>");
	else
		return obj->print();
}
