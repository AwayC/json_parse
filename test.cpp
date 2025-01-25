#ifdef WINDOWS
#define _CRTDBG_MAP_ALLOC
#include <ctrdbg.h>
#endif
#include <iostream>
#include <string> 
#include <vector> 
#include "leptjson.h" 
#include <stdio.h> 


static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect , actual, format) \
	do {\
		test_count ++; \
		if(equality) \
			test_pass ++ ; \
		else {\
			fprintf(stderr , "%s:%d: expect: " format" actual: " format "\n", __FILE__, __LINE__, expect, actual) ; \
			main_ret = 1;\
		}\
	} while(0) 

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual) , expect, actual , "%d") 
#define EXPECT_EQ_TYPE(expect, actual) EXPECT_EQ_BASE((expect) == (actual) , expect, actual, "%d" ) 
#define EXPECT_EQ_DOUBLE(expect, actual) EXPECT_EQ_BASE((expect) == (actual) , expect, actual, "%.17g") 
#define EXPECT_TRUE(actual) EXPECT_EQ_BASE((actual) != 0, "true" , "false" , "%s" ) 
#define EXPECT_FALSE(actual) EXPECT_EQ_BASE((actual) != 1, "false" , "true", "%s") 

static void test_parse_null() {
	lept_value v; 
	v.set_boolean(0);
	EXPECT_EQ_INT(LEPT_PARSE_OK, v.parse("null"));
	EXPECT_EQ_TYPE(lept_type::LEPT_NULL, v.get_type());
	v.free();
}

static void test_parse_true() {
	lept_value v;
	v.set_boolean(0);
	EXPECT_EQ_INT(LEPT_PARSE_OK, v.parse("true"));
	EXPECT_EQ_TYPE(lept_type::LEPT_TRUE, v.get_type()); 
	EXPECT_TRUE(v.get_boolean()); 
	v.free();
}

static void test_parse_false() {
	lept_value v; 
	v.set_boolean(1); 
	EXPECT_EQ_INT(LEPT_PARSE_OK, v.parse("false"));
	EXPECT_EQ_TYPE(lept_type::LEPT_FALSE, v.get_type());
	EXPECT_FALSE(v.get_boolean());
	v.free();
}

static void test_parse() {
	test_parse_null();
	test_parse_false();
	test_parse_true();
}

int main() {
	test_parse(); 
	printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count); 
	return main_ret;
}


