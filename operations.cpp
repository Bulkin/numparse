#include "operations.h"

#include <stdexcept>

using namespace std;

const static Op ops[] = {
	Op("+", Op::ADDITION, [](Number l, Number r) { return l + r; }),
	Op("-", Op::ADDITION, [](Number l, Number r) { return l - r; }),
	Op("*", Op::MULTIPLICATION, [](Number l, Number r) { return l * r; }),
	Op("/", Op::MULTIPLICATION, [](Number l, Number r) { return l / r; }),
};

const Op nop = Op("", Op::NOP, [](Number l, Number r) { return 0; });

Op Op::make(string name)
{
	for (auto &i : ops) {
		if (i.name == name)
			return i;
	}
	return nop;
}
