#ifndef LEPT_JSON_H__
#define LEPT_JSON_H__

#include <stddef.h> /*size_t*/
typedef enum { LEPT_NULL, LEPT_FALSE, LEPT_TRUE, LEPT_NUMBER, LEPT_STRING, LEPT_ARRAY, LEPT_OBJECT } lept_type;

class lept_member;
class lept_value;

class lept_member {
private : 
	char* k; size_t klen;
	lept_value v;
public : 
	lept_member();
	~lept_member();
	const char* get_key_string();
	size_t get_key_length();
	lept_value* get_value();
};

class lept_value {
private: 
	lept_type type;
	union {
		struct { lept_member* m; size_t size; } o;
		struct { lept_value* e; size_t size } a;
		struct { char* s; size_t len; } s;
		double n;
	}u;
public : 
	lept_value();
	~lept_value();
	lept_value get_type();
	double get_number();

	const char* get_string();
	size_t get_string_length();

	size_t get_array_size();
	lept_value* get_array_element();

	size_t get_object_size();
	const char* get_object_key_string();
	size_t get_object_key_length();
	lept_value* get_object_value();
};

enum {
	LEPT_PARSE__OK = 0,
	LEPT_PARSE_EXPECT_VALUE,
	LEPT_PARSE_INVALID_VALUE,
	LEPT_PARSE_ROOT_NOT_SINGULAR, 
	LEPT_PARSE_NUMBER_TOO_BIG,
	LEPT_PARSE_MISS_QUOTATION_MARK, 
	LEPT_PARSE_INVALID_STRING_ESCAPSE,
	LEPT_PARSE_INVALID_STRING_CHAR,
	LEPT_PARSE_INVALID_UNICODE_HEX,
	LEPT_PARSE_INVALID_UNICODE_SURROGATE,
	LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET,
	LEPT_PARSE_MISS_KEY,
	LEPT_PARSE_MISS_COLON,
	LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET
};

