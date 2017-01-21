#include <boost/test/unit_test.hpp>

#include "RegMapMock.hpp"

BOOST_AUTO_TEST_SUITE(operator_tests)


BOOST_AUTO_TEST_CASE(assignment_compound_assignment){

	auto test = regmap::RegMapMock("simple.json", 100);
	auto testreg = test.get<regmap::Register32_t>("test3");

	// direct assignment
	testreg = 0xAFFE;
	BOOST_CHECK_EQUAL(testreg, 0xAFFE);

	// compound assignments
	testreg ^= 0xBFF0;
	BOOST_CHECK_EQUAL(testreg, 0x100E);
		
	testreg = 0xAFFE;
	testreg |= 0x100F;
	BOOST_CHECK_EQUAL(testreg, 0xBFFF);
		
	testreg = 0xAFFE;
	testreg &= 0xDEAD;
	BOOST_CHECK_EQUAL(testreg, 0x8EAC);
		
	testreg = 0xAFFE;
	testreg <<= 4;
	BOOST_CHECK_EQUAL(testreg, 0xAFFE0);
		
	testreg = 0xAFFE;
	testreg >>= 4;
	BOOST_CHECK_EQUAL(testreg, 0xAFF);
}

BOOST_AUTO_TEST_CASE(comparison_test){

	auto test = regmap::RegMapMock("simple.json", 100);
	auto testreg = test.get<regmap::Register32_t>("test3");
	auto testreg2 = test.get<regmap::Register32_t>("bitmask_test");
	
	testreg = 0xAFFEu;
	BOOST_CHECK(testreg == 0xAFFEu);
	BOOST_CHECK(0xAFFEu == testreg);

	testreg2 = 0xAFFEu;
	BOOST_CHECK(testreg == testreg2);
	BOOST_CHECK(testreg2 == testreg);

	testreg2 = 0xDEADu;
	BOOST_CHECK(testreg != 0xDEADu);
	BOOST_CHECK(0xDEADu != testreg);

	BOOST_CHECK(testreg != testreg2);
	BOOST_CHECK(testreg2 != testreg);

}

BOOST_AUTO_TEST_CASE(logical_operator_test){

	auto test = regmap::RegMapMock("simple.json", 100);
	auto testreg = test.get<regmap::Register32_t>("test3");

	testreg = 0xAFFE;

	BOOST_CHECK_EQUAL(testreg ^ 0xBFF0, 0x100E);	
	BOOST_CHECK_EQUAL(testreg, 0xAFFE);
		
	BOOST_CHECK_EQUAL(testreg | 0x100F, 0xBFFF);
	BOOST_CHECK_EQUAL(testreg, 0xAFFE);
		
	BOOST_CHECK_EQUAL(testreg & 0xDEAD, 0x8EAC);
	BOOST_CHECK_EQUAL(testreg, 0xAFFE);
		
	BOOST_CHECK_EQUAL(testreg << 4, 0xAFFE0);
	BOOST_CHECK_EQUAL(testreg, 0xAFFE);
		
	BOOST_CHECK_EQUAL(testreg >> 4, 0xAFF);
	BOOST_CHECK_EQUAL(testreg, 0xAFFE);
	
	BOOST_CHECK_EQUAL(~testreg, 0xFFFF5001);
	BOOST_CHECK_EQUAL(testreg, 0xAFFE);
}


BOOST_AUTO_TEST_SUITE_END()
