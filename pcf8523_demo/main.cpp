#include "regmap.hpp"

#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>

int main(int argc, char** argv) {

	// i2c-dev expects the 7 address bits in the seven lower bits
	regmap::i2c::I2C registers(0, 0xD0 >> 1, "pcf8523.json");

	std::tm tm;
	std::array<char, 30> date_string;
	tm.tm_sec  = regmap::bcd::to_dec(registers.get<regmap::Register8_t>("Seconds"));
	tm.tm_min  = regmap::bcd::to_dec(registers.get<regmap::Register8_t>("Minutes"));
	tm.tm_hour = regmap::bcd::to_dec(registers.get<regmap::Register8_t>("Hours"));
	tm.tm_mday = regmap::bcd::to_dec(registers.get<regmap::Register8_t>("Days"));
	tm.tm_mon  = regmap::bcd::to_dec(registers.get<regmap::Register8_t>("Months")) - 1;
	tm.tm_year = regmap::bcd::to_dec(registers.get<regmap::Register8_t>("Years")) + 100;
	tm.tm_wday = regmap::bcd::to_dec(registers.get<regmap::Register8_t>("Weekdays"));
	tm.tm_yday = regmap::bcd::to_dec(registers.get<regmap::Register8_t>("Seconds"));

	// Those with a C++11 complete compiler will prolly use std::put_time	
	std::strftime(date_string.data(), date_string.size(), "%Y-%m-%d %H:%M:%S", &tm);
	std::cout << "PCF8523 Time: " << date_string.data() << std::endl;

	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::strftime(date_string.data(), date_string.size(), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
	std::cout << "System  Time: " << date_string.data() << std::endl;

	return 0;
}

