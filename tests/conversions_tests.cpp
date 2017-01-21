#include <boost/test/unit_test.hpp>

#include "regmap.hpp"

BOOST_AUTO_TEST_SUITE(conversion_tests)


BOOST_AUTO_TEST_CASE(bcd_converion_helper){

	std::uint32_t bcd_32 = 0x12345678;
	std::uint32_t bcd_16 = 0x1234;
	std::uint32_t bcd_8  = 0x12;

	BOOST_CHECK_EQUAL(regmap::bcd::to_dec(bcd_32), 12345678u);
	BOOST_CHECK_EQUAL(regmap::bcd::to_dec(bcd_16), 1234u);
	BOOST_CHECK_EQUAL(regmap::bcd::to_dec(bcd_8), 12u);

	std::uint32_t dec_32 = 12345678;
	std::uint32_t dec_16 = 1234;
	std::uint32_t dec_8  = 12;
	
	BOOST_CHECK_EQUAL(regmap::bcd::to_bcd(dec_32), 0x12345678);
	BOOST_CHECK_EQUAL(regmap::bcd::to_bcd(dec_16), 0x1234);
	BOOST_CHECK_EQUAL(regmap::bcd::to_bcd(dec_8), 0x12);
}

BOOST_AUTO_TEST_SUITE_END()
