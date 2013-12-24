#include <unique_ptr>

class Exp;
class Op;
class Func;
class Var;

class ArithmeticEvaluator {
	unique_ptr<Exp> expression;
public:
	ArithmeticEvaluator()
}
