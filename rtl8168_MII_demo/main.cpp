#include "regmap.hpp"

#include <iostream>
#include <string>

int main(int argc, char** argv) {

	// Mem Map BAR2 of the RTL8168 and get the register object for the PHYAR register
	regmap::pci::MemMapped memmap(regmap::pci::PCI_ID(0x10ec, 0x8168), "rtl8168.json", regmap::pci::BAR2);
	
	// Aternatively: IO Map BAR0 of the RTL8168 and get the register object for the PHYAR register
	//regmap::pci::IOMapped memmap(regmap::pci::PCI_ID(0x10ec, 0x8168), "rtl8168.json", regmap::pci::BAR0);
	
	auto phyar = memmap.get<regmap::Register32_t>("PHYAR");

	// Initiate a read of the PHY's clause 22 STAT1 register via MDIO
	// and wait until the data is available in PHYAR
	phyar = phyar["PMAPMD_STAT1_ADDRESS"];
	phyar.wait(100);

	std::cout << "PHYAR content..: " << std::hex << phyar << std::endl;
	std::cout << "Link status....: " << (phyar.is_set("PMAPMD_STAT1_RECEIVE_LINK") ? "UP" : "DOWN") << std::endl;
	std::cout << "Fault condition: " << std::boolalpha << phyar.is_set("PMAPMD_STAT1_FAULT_COND")  << std::endl;

	return 0;
}

