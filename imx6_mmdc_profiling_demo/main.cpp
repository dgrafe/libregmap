#include <iostream>
#include <thread>
#include "regmap.hpp"

int main(int argc, char** argv) {

	// mapping the MMDC registers via devmem, phys. addresses as documented
	regmap::devmem::DevMem registers(0x21B0000, 0x21B3FFF, "mmdc.json");

	auto MMDC_CTRL0 = registers.get<regmap::Register32_t>("MMDC1_CR0");
	auto MMDC_CTRL1 = registers.get<regmap::Register32_t>("MMDC1_CR1");
	auto MMDC_STAT0 = registers.get<regmap::Register32_t>("MMDC1_SR0");
	auto MMDC_STAT1 = registers.get<regmap::Register32_t>("MMDC1_SR1");
	auto MMDC_STAT4 = registers.get<regmap::Register32_t>("MMDC1_SR4");
	auto MMDC_STAT5 = registers.get<regmap::Register32_t>("MMDC1_SR5");

	// apply a filter for profiling specific subsystems connected to the MMDC
	MMDC_CTRL1 = MMDC_CTRL1["FILTER_ALL"];
	//MMDC_CTRL1 = MMDC_CTRL1["FILTER_IPU1"];
	//MMDC_CTRL1 = MMDC_CTRL1["FILTER_PCIE"];

	// start profiling, count for one second and then freeze the counters
	MMDC_CTRL0.start();
	MMDC_CTRL0.clear_reset();
	MMDC_CTRL0.unfreeze();
	std::this_thread::sleep_for(std::chrono::seconds(1));
	MMDC_CTRL0.freeze();

	std::cout << "Utilization..: " << static_cast<float>(MMDC_STAT1) / static_cast<float>(MMDC_STAT0) << std::endl;
	std::cout << "Bytes Read...: " << MMDC_STAT4 << std::endl;
	std::cout << "Bytes Written: " << MMDC_STAT5 << std::endl;

	// reset all profiling counters and stop profiling
	MMDC_CTRL0.reset();
	MMDC_CTRL0.stop();

	return 0;
}

