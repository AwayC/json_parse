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
#define EXPECT_EQ_STRING(expect, actual, alength) \
	EXPECT_EQ_BASE(sizeof(expect) -1 == alength && memcmp(expect, actual, alength + 1) == 0 , expect, actual, "%s"); 
#if defined(_MSC_VER)
#define EXPECT_EQ_SIZE_T(expect, actual) EXPECT_EQ_BASE((expect) == (actual), (size_t)expect, (size_t)actual, "%Iu")
#else
#define EXPECT_EQ_SIZE_T(expect, actual) EXPECT_EQ_BASE((expect) == (actual), (size_t)expect, (size_t)actual, "%zu")
#endif

static void test_parse_null() {
	lept_value v; 
	v.set_boolean(0);
	EXPECT_EQ_INT(LEPT_PARSE_OK, v.parse("null"));
	EXPECT_EQ_TYPE(lept_type::null, v.get_type());
}

static void test_parse_true() {
	lept_value v;
	v.set_boolean(0);
	EXPECT_EQ_INT(LEPT_PARSE_OK, v.parse("true"));
	EXPECT_EQ_TYPE(lept_type::ltrue, v.get_type()); 
	EXPECT_TRUE(v.get_boolean()); 
}

static void test_parse_false() {
	lept_value v; 
	v.set_boolean(1); 
	EXPECT_EQ_INT(LEPT_PARSE_OK, v.parse("false"));
	EXPECT_EQ_TYPE(lept_type::lfalse, v.get_type());
	EXPECT_FALSE(v.get_boolean());
}

#define TEST_NUMBER(expect, json) do{\
	lept_value v; \
	EXPECT_EQ_INT(LEPT_PARSE_OK, v.parse(json)) ; \
	EXPECT_EQ_INT(lept_type::number, v.get_type()) ;\
	EXPECT_EQ_DOUBLE(expect, v.get_number()) ;\
} while(0) 

static void test_parse_number() {
	TEST_NUMBER(0.0, "-0"); 
#if 1
	TEST_NUMBER(0.0, "-0.0");
	TEST_NUMBER(1.0, "1");
	TEST_NUMBER(-1.0, "-1");
	TEST_NUMBER(1.5, "1.5");
	TEST_NUMBER(-1.5, "-1.5");
	TEST_NUMBER(3.1416, "3.1416");
	TEST_NUMBER(1E10, "1E10");
	TEST_NUMBER(1e10, "1e10");
	TEST_NUMBER(1E+10, "1E+10");
	TEST_NUMBER(1E-10, "1E-10");
	TEST_NUMBER(-1E10, "-1E10");
	TEST_NUMBER(-1e10, "-1e10");
	TEST_NUMBER(-1E+10, "-1E+10");
	TEST_NUMBER(-1E-10, "-1E-10");
	TEST_NUMBER(1.234E+10, "1.234E+10");
	TEST_NUMBER(1.234E-10, "1.234E-10");
#if 0
	TEST_NUMBER(0.0, "1e-10000"); /* must underflow */
#endif
	TEST_NUMBER(1.0000000000000002, "1.0000000000000002"); /* the smallest number > 1 */
	TEST_NUMBER(4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
	TEST_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
	TEST_NUMBER(2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
	TEST_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
	TEST_NUMBER(2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
	TEST_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
	TEST_NUMBER(1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
	TEST_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");
#endif
}

#define TEST_STRING(expect, json) do{ \
	lept_value v; \
	EXPECT_EQ_INT(LEPT_PARSE_OK, v.parse(json)); \
	EXPECT_EQ_INT(lept_type::string, v.get_type()); \
	EXPECT_EQ_SIZE_T(sizeof(expect), v.get_string_length() + 1) ;\
	EXPECT_EQ_STRING(expect, v.get_string().c_str(), v.get_string_length()) ; \
} while(0) 

static void test_parse_string() {
 	TEST_STRING("1", "\"1\"");
#if 1
	TEST_STRING("Hello", "\"Hello\"");
	TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
	TEST_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
#endif
#if 1
	TEST_STRING("Hello\0World", "\"Hello\\u0000World\"");
	TEST_STRING("\x24", "\"\\u0024\"");         /* Dollar sign U+0024 */
	TEST_STRING("\xC2\xA2", "\"\\u00A2\"");     /* Cents sign U+00A2 */
	TEST_STRING("\xE2\x82\xAC", "\"\\u20AC\""); /* Euro sign U+20AC */
	TEST_STRING("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\"");  /* G clef sign U+1D11E */
	TEST_STRING("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\"");  /* G clef sign U+1D11E */
#endif
}

void static test_parse_array() {
	lept_value v; 
#if 1
	EXPECT_EQ_INT(LEPT_PARSE_OK, v.parse("[ ]")); 
	EXPECT_EQ_INT(lept_type::array, v.get_type()); 
	EXPECT_EQ_SIZE_T(0, v.get_array_size()); 
	
	EXPECT_EQ_INT(LEPT_PARSE_OK, v.parse("[ null , false , true , 123 , \"abc\" ]")); 
	EXPECT_EQ_INT(lept_type::array, v.get_type()); 
	EXPECT_EQ_SIZE_T(5, v.get_array_size());
	std::vector<lept_type> a = { lept_type::null, lept_type::lfalse, lept_type::ltrue, lept_type::number, lept_type::string }; 
	for (size_t i = 0; i < 5;i ++) 
		EXPECT_EQ_INT(a[i], v.get_array_element(i).get_type());
	EXPECT_EQ_DOUBLE(123.0, v.get_array_element(3).get_number()); 
	EXPECT_EQ_STRING("abc", v.get_array_element(4).get_string().c_str(), v.get_array_element(4).get_string_length());
#endif
	EXPECT_EQ_INT(LEPT_PARSE_OK, v.parse("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]")); 
	EXPECT_EQ_INT(lept_type::array, v.get_type()); 
	EXPECT_EQ_SIZE_T(4, v.get_array_size()); 
	for (size_t i = 0; i < 4; i++) {
		lept_value a = v.get_array_element(i);
		EXPECT_EQ_INT(lept_type::array, a.get_type());
		EXPECT_EQ_SIZE_T(i, a.get_array_size());
		for (size_t j = 0; j < i; j++) {
			lept_value e = a.get_array_element(j); 
			EXPECT_EQ_INT(lept_type::number, e.get_type()); 
			EXPECT_EQ_DOUBLE((double)j, e.get_number()); 
		}
	}
}

static void test_parse() {
	test_parse_null();
	test_parse_false();
	test_parse_true();
	test_parse_number();
	test_parse_string(); 
	test_parse_array(); 
}

int main() {
	test_parse(); 
	printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count); 
	return main_ret;
}


