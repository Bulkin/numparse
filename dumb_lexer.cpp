#include "dumb_lexer.h"
#include <boost/range/adaptor/reversed.hpp>

#include <stdexcept>

using namespace std;

stream_consumer_gen DumbLexer::match_char(char c)
{
	return [c](DumbLexer *lexer) {
		return stream_consumer([c](string::iterator start,
		                           string::iterator end) 
		                       {
			                       if (*start == c)
				                       return ++start;
			                       else
				                       return start;
		                       });
	};
}

stream_consumer_gen DumbLexer::match_range(char s, char e)
{
	return [s,e](DumbLexer *lexer) {
		return stream_consumer([s, e](string::iterator start,
		                              string::iterator end)
		                       {
			                       if (*start >= s &&
			                           *start <= e)
				                       return ++start;
			                       else
				                       return start;
		                       });
	};
}
		
stream_consumer_gen DumbLexer::multiple(string name)
{
	return [name](DumbLexer *lexer) {
		auto sc = lexer->find_symbol(name);
		return stream_consumer([sc](string::iterator start,
		                            string::iterator end)
		                       {
			                       auto tmp = start;
			                       while (tmp != end) {
				                       tmp = sc(tmp, end);
				                       if (tmp == start)
					                       break;
				                       start = tmp;
			                       }
			                       return tmp;
		                       });
	};
}

stream_consumer_gen DumbLexer::single(string name)
{
	return [name](DumbLexer *lexer) {
		auto sc = lexer->find_symbol(name);
		return stream_consumer([sc](string::iterator start,
		                            string::iterator end)
		                       {
			                       return sc(start, end);
		                       });
	};
}

stream_consumer_gen DumbLexer::maybe(string name)
{
	return [name](DumbLexer *lexer) {
		auto sc = lexer->find_symbol(name);
		return stream_consumer([sc](string::iterator start,
		                            string::iterator end)
		                       {
			                       return sc(start, end);
		                       },
		                       true);
	};
}

stream_consumer_gen DumbLexer::f_and(const stream_consumer_gen &lhs,
                                     const stream_consumer_gen &rhs)
{
	return [lhs, rhs](DumbLexer *lexer) {
		auto l = lhs(lexer);
		auto r = rhs(lexer);
		return stream_consumer([l, r](string::iterator start,
		                              string::iterator end)
		                       {
			                       auto tmp = start;
			                       tmp = l(tmp, end);
			                       if ((tmp == start) && !l.is_maybe())
				                       return start;
			                       
			                       auto tmp2 = tmp;
			                       tmp2 = r(tmp2, end);
			                       if ((tmp2 == tmp) && !r.is_maybe())
				                       return start;
			                       
			                       return tmp2;
		                       });
	};
}

stream_consumer_gen operator& (const stream_consumer_gen &lhs,
                               const stream_consumer_gen &rhs)
{
	return DumbLexer::f_and(lhs, rhs);
}


stream_consumer DumbLexer::find_symbol(const std::string &name)
{
	for (const auto &i : symbol_table) 
		if (name == i.first)
			return i.second;
	throw runtime_error("symbol not found");
}

string::iterator DumbLexer::find_match()
{
	for (const auto &i : boost::adaptors::reverse(symbol_table)) {
		auto tmp = i.second(cursor, end);
		if (tmp != cursor) {
			last_token_type = i.first;
			return tmp;
		}
	}
	throw runtime_error("couldn't match token");
}

DumbLexer::DumbLexer(std::string::iterator start, std::string::iterator end,
                     symbol_init_table_t *symbol_init_table) :
	cursor(start),
	end(end)
{
	symbol_init_table_t *init_table;
	if (symbol_init_table)
		init_table = symbol_init_table;
	else {
		init_table = &default_init_table;
		if (default_init_table.size() == 0)
			init_default_table(default_init_table);
	}
	symbol_table.reserve(init_table->size());
	for (auto &i : *init_table)
		symbol_table.push_back(make_pair(i.first, i.second(this)));
}

string::iterator DumbLexer::next_stop()
{
	if (cursor != end)
		cursor = find_match();
	return cursor;
}

string DumbLexer::next_token()
{
	string::iterator start = cursor;
	string::iterator next = next_stop();

	if (start >= next)
		return "";
	else
		return string(start, next);
}

string DumbLexer::peek_token()
{
	string::iterator start = cursor;
	string::iterator next;

	if (cursor == end)
		next = end;
	else
		next = find_match();

	if (start >= next)
		return "";
	else
		return string(start, next);
}

string DumbLexer::get_last_token_type() const
{
	return last_token_type;
}

string::iterator DumbLexer::get_cursor()
{
	return cursor;
}
