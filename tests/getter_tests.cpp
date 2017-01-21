#include <boost/test/unit_test.hpp>

#include "RegMapMock.hpp"

BOOST_AUTO_TEST_SUITE(Bitmask_tests)


BOOST_AUTO_TEST_CASE(getter_tests){

	        auto test = regmap::RegMapMock("simple.json", 100);
		auto testreg = test.get<regmap::Register32_t>("bitmask_test");

		BOOST_CHECK_EQUAL(testreg["ALL_SET"], 0xffffffff);
		BOOST_CHECK_EQUAL(testreg["ALL_UNSET"], 0);
		BOOST_CHECK_EQUAL(testreg["124"], 0x7);
}

BOOST_AUTO_TEST_CASE(simple_and_test){

	        auto test = regmap::RegMapMock("simple.json", 100);
		auto testreg = test.get<regmap::Register32_t>("bitmask_test");

		testreg = 0xAAAAAAAA;
		BOOST_CHECK_EQUAL(testreg, 0xAAAAAAAA);

		BOOST_CHECK_EQUAL(testreg & testreg["ALL_SET"], 0xAAAAAAAA);
		BOOST_CHECK_EQUAL(testreg & testreg["ALL_UNSET"], 0);
		BOOST_CHECK_EQUAL(testreg & testreg["124"], 2);
}

BOOST_AUTO_TEST_CASE(access_mask_test){

	        auto test = regmap::RegMapMock("simple.json", 100);
		auto testreg = test.get<regmap::Register16_t>("access_mask_test");

		testreg = 0x1234;
		BOOST_CHECK_EQUAL(testreg, 0x0034);
}

BOOST_AUTO_TEST_SUITE_END()
