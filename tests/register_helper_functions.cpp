#include <boost/test/unit_test.hpp>

#include "RegMapMock.hpp"

BOOST_AUTO_TEST_SUITE(register_helper_functions)


BOOST_AUTO_TEST_CASE(set_unset_is_set){

	        auto test = regmap::RegMapMock("simple.json", 100);
		auto testreg = test.get<regmap::Register32_t>("test3");

		testreg = 0xAFFE;
		BOOST_CHECK_EQUAL(testreg, 0xAFFE);
		BOOST_CHECK_EQUAL(testreg.apply(0xFFFF0001), 0xFFFFAFFF);
		BOOST_CHECK(testreg.is_set(0xFFFF0001));
		BOOST_CHECK_EQUAL(testreg.clear(0xFFFF0001), 0xAFFE);
		BOOST_CHECK_EQUAL(testreg, 0xAFFE);
}

BOOST_AUTO_TEST_CASE(simple_and_test){

}

BOOST_AUTO_TEST_SUITE_END()
