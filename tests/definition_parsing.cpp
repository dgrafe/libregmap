#include <boost/test/unit_test.hpp>

#include "RegMapMock.hpp"

BOOST_AUTO_TEST_SUITE(definition_parsing)

BOOST_AUTO_TEST_CASE(parses_existing_file){

	regmap::RegMapMock("simple.json", 100);
}

BOOST_AUTO_TEST_CASE(throws_on_non_existing_file){

	BOOST_CHECK_THROW(regmap::RegMapMock("xXx.json", 100), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(it_creates_simple_registers){

	auto test = regmap::RegMapMock("simple.json", 100);

	BOOST_CHECK_EQUAL(test.get<regmap::Register8_t>("test1").getName(), "test1");
	BOOST_CHECK_EQUAL(test.get<regmap::Register8_t>("test1").getOffset(), 0);
		
	BOOST_CHECK_EQUAL(test.get<regmap::Register16_t>("test2").getName(), "test2");
	BOOST_CHECK_EQUAL(test.get<regmap::Register16_t>("test2").getOffset(), 1);
		
	BOOST_CHECK_EQUAL(test.get<regmap::Register32_t>("test3").getName(), "test3");
	BOOST_CHECK_EQUAL(test.get<regmap::Register32_t>("test3").getOffset(), 3);
}

BOOST_AUTO_TEST_CASE(it_throws_on_non_existing_registers){

	auto test = regmap::RegMapMock("simple.json", 100);
	BOOST_CHECK_THROW(test.get<regmap::Register8_t>("xXx").getName(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(it_throws_on_wrong_template_arguments){

        auto test = regmap::RegMapMock("simple.json", 100);
		
	BOOST_CHECK_THROW(test.get<regmap::Register32_t>("test1"), std::runtime_error);
	BOOST_CHECK_THROW(test.get<regmap::Register8_t>("test2"), std::runtime_error);
	BOOST_CHECK_THROW(test.get<regmap::Register16_t>("test3"), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(it_throws_on_wrong_register_size){

        BOOST_CHECK_THROW(regmap::RegMapMock("wrong_register_size.json", 100), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(it_parses_ready_and_busy_masgs){
	        
	auto test = regmap::RegMapMock("busy_ready_mask.json", 100);
	auto testreg = test.get<regmap::Register32_t>("busy_ready_mask");

	BOOST_CHECK_EQUAL(testreg.get_busy_mask(), 0x01);
	BOOST_CHECK_EQUAL(testreg.get_ready_mask(), 0x10);
}

BOOST_AUTO_TEST_SUITE_END()
