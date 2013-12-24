#ifndef DUMB_LEXER_H
#define DUMB_LEXER_H

#include <functional>
#include <string>
#include <vector>
#include <utility>

typedef std::function<std::string::iterator(std::string::iterator start,
                                            std::string::iterator end)>
stream_consumer_func;

class DumbLexer;

class stream_consumer
{
	stream_consumer_func func;
	bool maybe;
	stream_consumer();
public:
	stream_consumer(stream_consumer_func func, 
	                bool maybe = false): 
		func(func), maybe(maybe) {};
	std::string::iterator operator() (const std::string::iterator &start,
	                                  const std::string::iterator &end) const { 
		return func(start, end);
	}

	bool is_maybe() const { return maybe; }
};

typedef std::function<stream_consumer(DumbLexer *lexer)> stream_consumer_gen;
stream_consumer_gen operator& (const stream_consumer_gen &lhs,
                               const stream_consumer_gen &rhs);

class DumbLexer
{
	typedef std::vector<std::pair<std::string, stream_consumer>>
		symbol_table_t;
	typedef std::vector<std::pair<std::string, stream_consumer_gen>>
		symbol_init_table_t;

	symbol_table_t symbol_table;
	symbol_init_table_t default_init_table;

	std::string::iterator cursor;
	std::string::iterator end;
	std::string last_token_type;

	DumbLexer();
	
	stream_consumer find_symbol(const std::string &name);
	std::string::iterator find_match();
public:
	DumbLexer(std::string::iterator start, std::string::iterator end,
	          symbol_init_table_t *symbol_init_table = NULL);

	std::string::iterator next_stop();
	std::string::iterator get_cursor();
	std::string next_token();
	std::string peek_token();
	std::string get_last_token_type() const;

	static stream_consumer_gen match_char(char c);
	static stream_consumer_gen match_range(char s, char e);
	static stream_consumer_gen multiple(std::string name);
	static stream_consumer_gen single(std::string name);
	static stream_consumer_gen maybe(std::string name);
	static stream_consumer_gen f_and(const stream_consumer_gen &lhs, 
	                                 const stream_consumer_gen &rhs);

private:
	static void init_default_table(symbol_init_table_t &table) {
		table = {
			{"digit", match_range('0', '9')},
			{"minus", match_char('-')},
			{"plus", match_char('+')},
			{"mult", match_char('*')},
			{"div", match_char('/')},
			{"decpoint", match_char('.')},
			{"paren_o", match_char('(')},
			{"paren_c", match_char(')')},
			{"natural", multiple("digit")},
			{"float", (maybe("natural") & single("decpoint")
			           & single("natural"))}
		};
	}
};

#endif
