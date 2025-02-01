#include "leptjson.h"
#include <assert.h> 
#include <math.h> 
#include <string> 
#include <vector> 
#include <errno.h> 
#include <iostream> 

#define ISDIGIT(ch) ((ch) >= '0' && (ch) <= '9') 
#define ISDIGIT1TO9(ch) ((ch) >= '1' && (ch) <= '9') 

class lept_context{
public : 
	std::string json; 
	size_t ptr;

	size_t  str_top;
	std::string stk_str;  
	
	void parse_whitespace();
	int parse_value(lept_value* v); 
	int parse_literal(lept_value* v, std::string literal , lept_type type); 
	int parse_number(lept_value* v); 
	int parse_string(lept_value* v); 
	size_t parse_hex4(size_t p , int* u); 
	int parse_array(lept_value* v); 
	void encode_utf8(int u); 
	void push_char(char c);
	std::string pop_string(size_t size); 
	lept_context() { ptr = str_top = 0; stk_str = ""; };
};


void lept_context::push_char(char c) {
	str_top++; 
	stk_str.push_back(c); 
}

std::string lept_context::pop_string(size_t size) {
	assert(str_top >= size); 
	return stk_str; 
}

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
	switch (type) {
		case lept_type::null: v->set_null(); break; 
		case lept_type::lfalse: v->set_boolean(0); break; 
		case lept_type::ltrue: v->set_boolean(1);  break; 
	}
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
	ptr = tmp;
	return LEPT_PARSE_OK;
}

size_t lept_context::parse_hex4(size_t p, int* u) {
	*u = 0; 
	for (int i = 0; i < 4; i++) {
		char ch = json[++ p];
		*u <<= 4; 
		if (ch >= '0' && ch <= '9') *u |= ch - '0';
		else if (ch >= 'A' && ch <= 'Z') *u |= ch - ('A' - 10);
		else if (ch >= 'a' && ch <= 'z') *u |= ch - ('a' - 10);
		else return 0;
	}
	return p; 
}

void lept_context::encode_utf8(int u) {
	if (u <= 0x7F)
		push_char((char)u);
	else if (u <= 0x7FF) {
		push_char((char)(0xC0 | ((u >> 6) & 0xFF)));
		push_char((char)(0x80 | (u) & 0x3F)); 
	}
	else if (u <= 0xFFFF) {
		push_char((char)(0xE0 | (u >> 12) & 0xFF)); 
		push_char((char)(0x80 | (u >> 6) & 0x3F));
		push_char((char)(0x80 | u & 0x3F));
	}
	else {
		assert(u <= 0x10FFFF);
		push_char((char)(0xF0 | (u >> 18) & 0xFF));
		push_char((char)(0x80 | (u >> 12) & 0x3F));
		push_char((char)(0x80 | (u >> 6) & 0x3F));
		push_char((char)(0x80 | u & 0x3F));
	}
}

int lept_context::parse_string(lept_value* v) {
	assert(ptr < json.size() && json[ptr] == '\"');
	size_t tmp = ptr, len = 0, head = str_top;
	std::string str; 
	int u, u2;
	for (;;) {
		char ch = json[++tmp];
		switch (ch) {
			case '\"':
				len = str_top - head;
				v->set_string(pop_string(len), len);
				ptr = ++tmp;
				str_top = head; 
				return LEPT_PARSE_OK;
			case '\\':
				switch (json[++tmp]) {
					case '\"': push_char('\"');  break;
					case '\\': push_char('\\'); break;
					case '/': push_char('/');  break;
					case 'b': push_char('\b'); break;
					case 't': push_char('\t'); break;
					case 'n': push_char('\n'); break;
					case 'r':push_char('\r'); break;
					case 'f': push_char('\f'); break;
					case 'u':
						if (!(tmp = parse_hex4(tmp, &u)))
							return LEPT_PARSE_INVALID_UNICODE_HEX;
						if (u >= 0xD800 && u <= 0xDBFF) {
							if (json[++tmp] != '\\')
								return LEPT_PARSE_INVALID_UNICODE_SURROGATE;
							if (json[++tmp] != 'u')
								return LEPT_PARSE_INVALID_UNICODE_SURROGATE;
							if (!(tmp = parse_hex4(tmp, &u2)))
								return LEPT_PARSE_INVALID_UNICODE_HEX;
							if (u2 < 0xDC00 || u2 > 0xDFFF)
								return LEPT_PARSE_INVALID_UNICODE_SURROGATE;
							u = (((u - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000; 
						}
						encode_utf8(u); 
						break; 
					default: 
						return LEPT_PARSE_INVALID_STRING_CHAR; 
				}
				break; 
			case '\0': 
				str_top = 0; 
				return LEPT_PARSE_MISS_QUOTATION_MARK;
			default:
				if ((unsigned char)ch < 0x20) {
					str_top = 0; 
					return LEPT_PARSE_INVALID_STRING_CHAR;
				}
				push_char(ch);
				break; 
		}
	}
}

int lept_context::parse_array(lept_value* v) {
	assert(json[ptr ++] == '['); 
	std::vector<lept_value> arr; 
	parse_whitespace(); 
	if (json[ptr] == ']') {
		ptr++; 
		v->set_array(arr); 
		return LEPT_PARSE_OK; 
	}
	int ret;
	size_t len = 0; 
	for (;;) {
		lept_value e; 
		if ((ret = parse_value(&e)) != LEPT_PARSE_OK) 
			break; 
		arr.push_back(e); 
		len++; 
		parse_whitespace(); 
		if (json[ptr] == ',') {
			ptr++; 
			parse_whitespace(); 
		}
		else if (json[ptr] == ']') {
			ptr++; 
			v->set_array(arr); 
			return LEPT_PARSE_OK; 
		}
		else {
			ret = LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET; 
			break;
		}
	}
	return ret; 
}

int lept_context::parse_value(lept_value* v) {
	switch (this->json[ptr]) {
	case 't': return this->parse_literal(v, "true", lept_type::ltrue);
	case 'f': return this->parse_literal(v, "false", lept_type::lfalse);
	case 'n': return this->parse_literal(v, "null", lept_type::null);
	case '\0': return LEPT_PARSE_EXPECT_VALUE;
	case '\"': return this->parse_string(v); 
	case '[': return this->parse_array(v);
	default: return parse_number(v); 
	}
}

/**********************************  lept_value  **************************************/

int lept_value::parse(std::string json) {
	lept_context c;
	int ret;
	c.json = json + '\0';
	c.str_top = 0;
	this->free();
	c.parse_whitespace(); 
	if (ret = c.parse_value(this)) {
		c.parse_whitespace(); 
		if (c.ptr != c.json.size() - 1) {
			this->type = lept_type::null;
			ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
		}
	}
	assert(c.str_top == 0);
	c.stk_str.clear(); 
	return ret;
}

lept_value::lept_value() {
	this->type = lept_type::null; 
}

lept_value::lept_value(const lept_value& val) {
	this->free(); 
	switch (val.type) {
		case lept_type::number: n = val.n; break;
		case lept_type::string: new(&s) std::string(val.s); break;
		case lept_type::array: new(&arr) std::vector<lept_value>(val.arr); break;
		case lept_type::object: new(&obj) std::map<std::string, lept_value>(val.obj); break;
		default: break;
	}
	type = val.type; 
}

lept_value::~lept_value() {
	this->free(); 
}

void lept_value::free() {
	switch (this->type) {
		case lept_type::string: 
			s.~basic_string(); break;
		case lept_type::array: 
			arr.~vector(); break; 
		case lept_type::object : 
			obj.~map(); 
		default: 
			break; 
	}
	type = lept_type::null; 
}

void lept_value::set_null() {
	this->free(); 
}

void lept_value::set_boolean(int b) {
	this->free(); 
	if (b != 0) type = lept_type::ltrue;
	else type = lept_type::lfalse;
}

bool lept_value::get_boolean() {
	assert(type == lept_type::lfalse || type == lept_type::ltrue);
	if (type == lept_type::ltrue) return true;
	else return false;
}

void lept_value::set_number(double num) {
	this->free();
	this->type = lept_type::number;
	this->n = num; 
}

double lept_value::get_number() {
	assert(this->type == lept_type::number); 
	return n;
}

const std::string lept_value::get_string() {
	assert(type == lept_type::string);
	return s; 
}

void lept_value::set_string(std::string str, size_t len) {
	assert(str.size() == len); 
	this->free();
	new(&s) std::string; 
	type = lept_type::string; 
	this->s; 
	this->s = str; 
}

size_t lept_value::get_string_length() {
	assert(type == lept_type::string); 
	return s.size(); 
}

void lept_value::set_array(const std::vector<lept_value>& val) {
	this->free(); 
	type = lept_type::array; 
	new(&arr) std::vector<lept_value>(val); 
}

size_t lept_value::get_array_size() {
	assert(type == lept_type::array); 
	return arr.size(); 
}

lept_value lept_value::get_array_element(size_t index) {
	assert(type == lept_type::array && arr.size() > index); 
	return arr[index]; 
}