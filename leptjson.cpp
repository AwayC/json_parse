#include "leptjson.h"
#include <assert.h> 
#include <math.h> 
#include <string> 
#include <vector> 
#include <errno.h> 


#define ISDIGIT(ch) ((ch) >= '0' && (ch) <= '9') 
#define ISDIGIT1TO9(ch) ((ch) >= '1' && (ch) <= '9') 

class lept_context {
public : 
	std::string json; 
	size_t ptr;
	size_t size, top;
	std::vector<char> stk;
	
	void parse_whitespace();
	int parse_value(lept_value* v); 
	int parse_literal(lept_value* v, std::string literal , lept_type type); 
	int parse_number(lept_value* v); 
	lept_context() { ptr = top = size = 0; };
};

void lept_context::parse_whitespace() {
	size_t tmp = this->ptr; 
	while (json[tmp] == ' ' || json[tmp] == '\t' || json[tmp] == '\n' || json[tmp] == '\r')
		tmp++;
	ptr = tmp;
}


int lept_context::parse_literal(lept_value* v, std::string literal, lept_type type) {
	assert(ptr < json.size() && json[ptr] == literal[0]);
	int i;
	for (i = 0; i < literal.size(); i++) {
		if (json[i + ptr] != literal[i])
			return LEPT_PARSE_INVALID_VALUE;
	}
	ptr += i; 
	v->set_type(type); 
	return LEPT_PARSE_OK;
}


int lept_context::parse_number(lept_value* v) {
	size_t tmp = ptr;
	if (json[tmp] == '-') tmp++;
	if (json[tmp] == '0') tmp++;
	else {
		if (!ISDIGIT1TO9(json[tmp])) return LEPT_PARSE_INVALID_VALUE;
		for (tmp++; ISDIGIT(json[tmp]); tmp++);
	}
	if (json[tmp] == '.') {
		tmp++;
		if (!ISDIGIT(json[tmp])) return LEPT_PARSE_INVALID_VALUE;
		for (tmp++; ISDIGIT(json[tmp]); tmp++); 
	}
	if (json[tmp] == 'e' || json[tmp] == 'E') {
		tmp++;
		if (json[tmp] == '+' || json[tmp] == '-') tmp++;
		if (!ISDIGIT(json[tmp])) return LEPT_PARSE_INVALID_VALUE; 
		for (tmp++; ISDIGIT(json[tmp]); tmp++);
	}
	errno = 0;
	double num = stod(json.substr(ptr, tmp - ptr)); 
	if (errno == ERANGE && (num == HUGE_VAL || num == -HUGE_VAL))
		return LEPT_PARSE_NUMBER_TOO_BIG;
	v->set_number(num); 
	v->set_type(lept_type::LEPT_NUMBER); 
	ptr = tmp;
	return LEPT_PARSE_OK;
}

int lept_context::parse_value(lept_value* v) {
	switch (this->json[ptr]) {
	case 't': return this->parse_literal(v, "true", lept_type::LEPT_TRUE);
	case 'f': return this->parse_literal(v, "false", lept_type::LEPT_FALSE);
	case 'n': return this->parse_literal(v, "null", lept_type::LEPT_NULL);
	case '\0': return LEPT_PARSE_EXPECT_VALUE;
	default: return parse_number(v); 
	}
}

/**********************************  lept_value  **************************************/

int lept_value::parse(std::string json) {
	lept_context c;
	int ret;
	c.json = json + '\0';
	c.size = c.top = 0;
	this->free();
	c.parse_whitespace(); 
	if (ret = c.parse_value(this)) {
		c.parse_whitespace(); 
		if (c.ptr != c.json.size() - 1) {
			this->type = lept_type::LEPT_NULL;
			ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
		}
	}
	assert(c.top == 0);
	c.stk.clear(); 
	return ret;
}

lept_value::lept_value() {
	this->type = lept_type::LEPT_NULL; 
}

lept_value::~lept_value() {
	this->free(); 
}

void lept_value::free() {
	this->type = lept_type::LEPT_NULL;
}

void lept_value::set_null() {
	this->free(); 
}

void lept_value::set_boolean(int b) {
	if (b != 0) type = lept_type::LEPT_TRUE;
	else type = lept_type::LEPT_FALSE;
}
bool lept_value::get_boolean() {
	assert(type == lept_type::LEPT_FALSE || type == lept_type::LEPT_TRUE); 
	if (type == lept_type::LEPT_TRUE) return true;
	else return false;
}

void lept_value::set_number(double num) {
	this->free();
	this->type = lept_type::LEPT_NUMBER;
	this->n = num; 
}

double lept_value::get_number() {
	assert(this->type == lept_type::LEPT_NUMBER); 
	return n;
}