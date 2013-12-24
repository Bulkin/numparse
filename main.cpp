#include <iostream>

#include "dumb_lexer.h"
#include "operations.h"
#include "expgen.h"

using namespace std;

void test_lexer(string expr)
{
	auto lexer = DumbLexer(expr.begin(), expr.end());

	string::iterator cursor = expr.begin();
	while (cursor != expr.end()) {
		auto last_cursor = cursor;
		cursor = lexer.next_stop();
		cout << string(last_cursor, cursor) << ":" <<
			lexer.get_last_token_type() << endl;
	}
}

void test_opgen(string name)
{
	Op op = Op::make(name);
	cout << op.print() << ", " << op.get_priority() << endl;
}

void test_expgen(string expr)
{
	auto lexer = DumbLexer(expr.begin(), expr.end());
	ArithmeticObj_p e = make_expression(lexer);
	cout << "--------------------" << endl;
	cout << ">>> " << expr << endl;
	cout << e->print() << endl;
	cout << e->eval()->print() << endl;
	cout << e->print() << endl;
}

int main(int argc, char *argv[])
{
	// test_lexer("4+45.6*3.45/(.3545+-35)");
	// test_opgen("+");
	// test_opgen("_");

	test_expgen("1+(3+4)*2");
	test_expgen("1+(2+3)*(4-(5-6))");
	
	string test_expr(argv[1]);
	test_expgen(test_expr);

	return 0;
}
