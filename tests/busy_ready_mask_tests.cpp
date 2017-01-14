#include <thread>
#include <chrono>
#include <boost/test/unit_test.hpp>
#include "RegMapMock.hpp"

BOOST_AUTO_TEST_SUITE(busy_ready_mask_tests)


void setFlag(regmap::Register32_t reg, std::uint32_t mask, unsigned int delay_ms) {

	std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
	reg = mask;
}

BOOST_AUTO_TEST_CASE(ready_mask_tests) {

	        auto test = regmap::RegMapMock("busy_ready_mask.json", 100);
		auto testreg = test.get<regmap::Register32_t>("busy_ready_mask");

		testreg = 0x10;
		// it returns true if mask is set
		BOOST_CHECK_EQUAL(testreg.wait(std::chrono::milliseconds(10)), true);

		testreg = 0x0;
		// it times out if mask is not set in time
		BOOST_CHECK_EQUAL(testreg.wait(std::chrono::milliseconds(10)), false);

		// a somewhat more realistic test with a thread setting the mask
		// while blocking on the wait
		testreg = 0x0;
		std::thread setter(setFlag, testreg, 0x10, 30);
		BOOST_CHECK_EQUAL(testreg.wait(std::chrono::milliseconds(100)), true);
		setter.join();
}

BOOST_AUTO_TEST_CASE(busy_mask_tests) {

	        auto test = regmap::RegMapMock("busy_ready_mask.json", 100);
		auto testreg = test.get<regmap::Register32_t>("busy_ready_mask");

		testreg = 0x00;
		// it returns true when mask is unset
		BOOST_CHECK_EQUAL(testreg.work(std::chrono::milliseconds(10)), true);

		testreg = 0x01;
		// it times out if mask is not unsset in time
		BOOST_CHECK_EQUAL(testreg.work(std::chrono::milliseconds(10)), false);

		// a somewhat more realistic test with a thread setting the mask
		// while blocking on the wait
		testreg = 0x01;
		std::thread setter(setFlag, testreg, 0x00, 30);
		BOOST_CHECK_EQUAL(testreg.work(std::chrono::milliseconds(100)), true);
		setter.join();
}

BOOST_AUTO_TEST_CASE(simple_and_test){

}

BOOST_AUTO_TEST_SUITE_END()
