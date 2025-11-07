#include <gtest/gtest.h>
#include "leptjson.h" 

// ¶¨Òå¸¨ÖúºêÀ´²âÊÔÊý×Ö½âÎö
#define TEST_NUMBER_GTEST(expect, json) \
    do { \
        lept_value v; \
        EXPECT_EQ(LEPT_PARSE_OK, v.parse(json)); \
        EXPECT_EQ(lept_type::number, v.get_type()); \
        EXPECT_DOUBLE_EQ(expect, v.get_number()); \
    } while (0)

// ¶¨Òå¸¨ÖúºêÀ´²âÊÔ×Ö·û´®½âÎö
#define TEST_STRING_GTEST(expect, json) \
    do { \
        lept_value v; \
        EXPECT_EQ(LEPT_PARSE_OK, v.parse(json)); \
        EXPECT_EQ(lept_type::string, v.get_type()); \
        EXPECT_EQ(sizeof(expect) - 1, v.get_string().size()); \
        EXPECT_STREQ(expect, v.get_string().c_str()); \
    } while (0)

TEST(LeptJsonTest, ParseNull) {
    lept_value v;
    v.set_boolean(0);
    EXPECT_EQ(LEPT_PARSE_OK, v.parse("null"));
    EXPECT_EQ(lept_type::null, v.get_type());
}

TEST(LeptJsonTest, ParseTrue) {
    lept_value v;
    v.set_boolean(0);
    EXPECT_EQ(LEPT_PARSE_OK, v.parse("true"));
    EXPECT_EQ(lept_type::ltrue, v.get_type());
    EXPECT_TRUE(v.get_boolean());
}

TEST(LeptJsonTest, ParseFalse) {
    lept_value v;
    v.set_boolean(1);
    EXPECT_EQ(LEPT_PARSE_OK, v.parse("false"));
    EXPECT_EQ(lept_type::lfalse, v.get_type());
    EXPECT_FALSE(v.get_boolean());
}

TEST(LeptJsonTest, ParseNumber) {
    TEST_NUMBER_GTEST(0.0, "-0");
#if 1
    TEST_NUMBER_GTEST(0.0, "-0.0");
    TEST_NUMBER_GTEST(1.0, "1");
    TEST_NUMBER_GTEST(-1.0, "-1");
    TEST_NUMBER_GTEST(1.5, "1.5");
    TEST_NUMBER_GTEST(-1.5, "-1.5");
    TEST_NUMBER_GTEST(3.1416, "3.1416");
    TEST_NUMBER_GTEST(1E10, "1E10");
    TEST_NUMBER_GTEST(1e10, "1e10");
    TEST_NUMBER_GTEST(1E+10, "1E+10");
    TEST_NUMBER_GTEST(1E-10, "1E-10");
    TEST_NUMBER_GTEST(-1E10, "-1E10");
    TEST_NUMBER_GTEST(-1e10, "-1e10");
    TEST_NUMBER_GTEST(-1E+10, "-1E+10");
    TEST_NUMBER_GTEST(-1E-10, "-1E-10");
    TEST_NUMBER_GTEST(1.234E+10, "1.234E+10");
    TEST_NUMBER_GTEST(1.234E-10, "1.234E-10");
#if 0
    TEST_NUMBER_GTEST(0.0, "1e-10000"); /* must underflow */
#endif
    TEST_NUMBER_GTEST(1.0000000000000002, "1.0000000000000002"); /* the smallest number > 1 */
    TEST_NUMBER_GTEST(4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
    TEST_NUMBER_GTEST(-4.9406564584124654e-324, "-4.9406564584124654e-324");
    TEST_NUMBER_GTEST(2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
    TEST_NUMBER_GTEST(-2.2250738585072009e-308, "-2.2250738585072009e-308");
    TEST_NUMBER_GTEST(2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
    TEST_NUMBER_GTEST(-2.2250738585072014e-308, "-2.2250738585072014e-308");
    TEST_NUMBER_GTEST(1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
    TEST_NUMBER_GTEST(-1.7976931348623157e+308, "-1.7976931348623157e+308");
#endif
}

TEST(LeptJsonTest, ParseString) {
    TEST_STRING_GTEST("1", "\"1\"");
#if 1
    TEST_STRING_GTEST("Hello", "\"Hello\"");
    TEST_STRING_GTEST("Hello\nWorld", "\"Hello\\nWorld\"");
    TEST_STRING_GTEST("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
#endif
#if 1
    TEST_STRING_GTEST("Hello\0World", "\"Hello\\u0000World\"");
    TEST_STRING_GTEST("\x24", "\"\\u0024\"");         /* Dollar sign U+0024 */
    TEST_STRING_GTEST("\xC2\xA2", "\"\\u00A2\"");     /* Cents sign U+00A2 */
    TEST_STRING_GTEST("\xE2\x82\xAC", "\"\\u20AC\""); /* Euro sign U+20AC */
    TEST_STRING_GTEST("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\"");  /* G clef sign U+1D11E */
    TEST_STRING_GTEST("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\"");  /* G clef sign U+1D11E */
#endif
}

TEST(LeptJsonTest, ParseArray) {
    lept_value v;
#if 1
    EXPECT_EQ(LEPT_PARSE_OK, v.parse("[ ]"));
    EXPECT_EQ(lept_type::array, v.get_type());
    EXPECT_EQ(size_t(0), v.get_array_size());

    EXPECT_EQ(LEPT_PARSE_OK, v.parse("[ null , false , true , 123 , \"abc\" ]"));
    EXPECT_EQ(lept_type::array, v.get_type());
    EXPECT_EQ(size_t(5), v.get_array_size());
    std::vector<lept_type> a = { lept_type::null, lept_type::lfalse, lept_type::ltrue, lept_type::number, lept_type::string };
    for (size_t i = 0; i < 5; ++i) {
        EXPECT_EQ(a[i], v.get_array_element(i).get_type());
    }
    EXPECT_DOUBLE_EQ(123.0, v.get_array_element(3).get_number());
    EXPECT_STREQ("abc", v.get_array_element(4).get_string().c_str());
#endif
    EXPECT_EQ(LEPT_PARSE_OK, v.parse("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]"));
    EXPECT_EQ(lept_type::array, v.get_type());
    EXPECT_EQ(size_t(4), v.get_array_size());
    for (size_t i = 0; i < 4; ++i) {
        lept_value a = v.get_array_element(i);
        EXPECT_EQ(lept_type::array, a.get_type());
        EXPECT_EQ(i, a.get_array_size());
        for (size_t j = 0; j < i; ++j) {
            lept_value e = a.get_array_element(j);
            EXPECT_EQ(lept_type::number, e.get_type());
            EXPECT_DOUBLE_EQ((double)j, e.get_number());
        }
    }
}

TEST(LeptJsonTest, ParseObject) {
    lept_value v;
#if 1
    EXPECT_EQ(LEPT_PARSE_OK, v.parse(" { } "));
    EXPECT_EQ(lept_type::object, v.get_type());
    EXPECT_EQ(size_t(0), v.get_object_size());
#endif
#if 1
    EXPECT_EQ(LEPT_PARSE_OK, v.parse(
        " { "
        "\"n\" : null , "
        "\"f\" : false , "
        "\"t\" : true , "
        "\"i\" : 123 , "
        "\"s\" : \"abc\", "
        "\"a\" : [ 1, 2, 3 ],"
        "\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
        " } "
    ));
#endif 
#if 1
    EXPECT_EQ(lept_type::object, v.get_type());
    EXPECT_EQ(size_t(7), v.get_object_size());
    EXPECT_TRUE(v.contains_key("n"));
    EXPECT_EQ(lept_type::null, v.get_object_value("n").get_type());
    EXPECT_TRUE(v.contains_key("f"));
    EXPECT_EQ(lept_type::lfalse, v.get_object_value("f").get_type());
    EXPECT_TRUE(v.contains_key("t"));
    EXPECT_EQ(lept_type::ltrue, v.get_object_value("t").get_type());
    EXPECT_TRUE(v.contains_key("i"));
    EXPECT_DOUBLE_EQ(123.0, v.get_object_value("i").get_number());
    EXPECT_TRUE(v.contains_key("s"));
    EXPECT_STREQ("abc", v.get_object_value("s").get_string().c_str());
    EXPECT_TRUE(v.contains_key("a"));
    lept_value a = v.get_object_value("a");
    EXPECT_EQ(lept_type::array, a.get_type());
    EXPECT_EQ(size_t(3), a.get_array_size());
    for (size_t i = 0; i < 3; ++i) {
        lept_value e = a.get_array_element(i);
        EXPECT_EQ(lept_type::number, e.get_type());
        EXPECT_DOUBLE_EQ((double)(i + 1), e.get_number());
    }
    EXPECT_TRUE(v.contains_key("o"));
    lept_value o = v.get_object_value("o");
    EXPECT_EQ(size_t(3), o.get_object_size());
    for (int i = 1; i <= 3; ++i) {
        std::string s = std::to_string(i);
        lept_value e = o.get_object_value(s);
        EXPECT_EQ(lept_type::number, e.get_type());
        EXPECT_DOUBLE_EQ((double)i, e.get_number());
    }
#endif 
}

// ¶¨Òå¸¨ÖúºêÀ´²âÊÔ×Ö·û´®»¯
#define TEST_STRINGIFY_GTEST(json) \
    do { \
        lept_value v; \
        v.parse(json); \
        std::string str = v.stringify(); \
        EXPECT_STREQ(json, str.c_str()); \
    } while (0)

TEST(LeptJsonTest, Stringify) {
    TEST_STRINGIFY_GTEST("null");
    TEST_STRINGIFY_GTEST("false");
    TEST_STRINGIFY_GTEST("true");
    TEST_STRINGIFY_GTEST("123.3231");
    TEST_STRINGIFY_GTEST("\"ABFDSF\"");
    TEST_STRINGIFY_GTEST("\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
    TEST_STRINGIFY_GTEST("[null,false,true,123,\"abc\"]");
    TEST_STRINGIFY_GTEST("[[],[0],[0,1],[0,1,2]]");
#if 1
    TEST_STRINGIFY_GTEST(
        "{"
        "\"a\":[1,2,3],"
        "\"f\":false,"
        "\"i\":123,"
        "\"n\":null,"
        "\"o\":{\"1\":1,\"2\":2,\"3\":3},"
        "\"s\":\"abc\","
        "\"t\":true"
        "}"
    );
#endif 
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}