#pragma once 

#include <stddef.h>
#include <string> 
#include <vector> 
#include <map> 
enum class lept_type { LEPT_NULL, LEPT_FALSE, LEPT_TRUE, LEPT_NUMBER, LEPT_STRING, LEPT_ARRAY, LEPT_OBJECT };

class lept_value;

class lept_value {
private: 
	union u{
		double n;
		std::string s;
		std::vector<lept_value> arr;
		std::map<std::string, lept_value> obj;
		u() { };
	};
	lept_type type;

public : 
	lept_value() ;
	~lept_value();

	int parse(std::string json);
	
	void free(); 

	void set_null(); 

	lept_type get_type() {
		return type;
	};
	void set_type(lept_type type) {
		this->type = type;
	};

	bool get_boolean(); 
	void set_boolean(int b);

	double get_number(); 
	void set_number(double n);
#if 0 
	const std::string get_string();
	size_t get_string_length();
	void set_string(std::string, size_t len);

	size_t get_array_size();
	lept_value get_array_element(size_t index);

	size_t get_object_size();
	std::string get_object_key(size_t index); 
	size_t get_object_key_length(size_t index); 
	lept_value get_object_value(size_t index);
#endif 
};

enum  {
	LEPT_PARSE_OK = 0,
	LEPT_PARSE_EXPECT_VALUE,
	LEPT_PARSE_INVALID_VALUE,
	LEPT_PARSE_ROOT_NOT_SINGULAR,
	LEPT_PARSE_NUMBER_TOO_BIG,
	LEPT_PARSE_MISS_QUOTATION_MARK,
	LEPT_PARSE_INVALID_STRING_ESCAPE,
	LEPT_PARSE_INVALID_STRING_CHAR,
	LEPT_PARSE_INVALID_UNICODE_HEX,
	LEPT_PARSE_INVALID_UNICODE_SURROGATE,
	LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET,
	LEPT_PARSE_MISS_KEY,
	LEPT_PARSE_MISS_COLON, 
	LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET
};