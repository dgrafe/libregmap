# libregmap

This library, written in C++11, implements a generic interface for accessing Memory- or IO-Mapped registers from the linux userspace.

The structure of the register map (for instance offsets, sizes, bitmasks) is defined in an external json file. So your code will be kept clean of such low level details.

The definition file is needed when instantiating the register map. From this map object you can access all the defined register objects which represent the mapped register including the bitmasks and which feature some operators, helper functions, etc.

The intention of this library is to
* provide an OOP interface to the register maps (Abstraction)
* make writing clean code possible instead of coding the register structure
* remove boilerplate code such as mapping a PCI BAR (Reusability)

## Real world examples

### reading rtl8168's PHY link status
* The rtl8168 is identified by the PCI ID 10ec:8168
* The register map is memory mapped at BAR2
* The PHY's link status can be read from he Clause 22 Stat1 register via MDIO
* These registers can be accessed via the PHYAR register from the register map at offset 0x60
* The lower 16 bit of PHYAR represet the data to be read/written via MDIO
* The 5 bits [20-16] specify the Clause 22 register to be read/written
* Bit 31 has to be set to 0 for reads and will be set to 1 as soon as the Clause 22 register has been read and the data is available.

The resulting register definition file:
``` json
{
  "registers": {
                "PHYAR": {
                        "size": "4",
                        "offset": "0x60",
                        "bitmasks": {
                                "PMAPMD_STAT1_ADDRESS": "0x10000",
                                "PMAPMD_STAT1_RECEIVE_LINK": "0x4",
                                "PMAPMD_STAT1_FAULT_COND": "0x80"
                        },
                        "ready_mask": "0x80000000"
                }
        }
}
```
The code for reading the link status then boils down to:
``` c++
#include "regmap.hpp"
#include <iostream>

int main(int argc, char** argv) {

  // Map BAR2 of the RTL8168 and get the register object for the PHYAR register
  regmap::pci::MemMapped memmap(regmap::pci::PCI_ID(0x10ec, 0x8168), "rtl8168.json", regmap::pci::BAR2);
  auto phyar = memmap.get<regmap::Register32_t>("PHYAR");

  // Initiate a read of the PHY's clause 22 STAT1 register via MDIO
  // and wait until the data is available in PHYAR
  phyar = phyar["PMAPMD_STAT1_ADDRESS"];
  phyar.wait(std::chrono::milliseconds(100));

  std::cout << "PHYAR content..: " << std::hex << phyar << std::endl;
  std::cout << "Link status....: " << (phyar.is_set("PMAPMD_STAT1_RECEIVE_LINK") ? "UP" : "DOWN") << std::endl;
  std::cout << "Fault condition: " << std::boolalpha << phyar.is_set("PMAPMD_STAT1_FAULT_COND")  << std::endl;

  return 0;
}
```

### Reading the time from a PCF8523 RTC
* The PCF8523 is connected via i2c, slave address 0xD0, Bus 0 on my device
* BCD coded date values
* The date values are present in registers 0x03 to 0x09
* Not all bits in the 8Bit registers are defined, so we apply masks on accesses

Given the following definition file:
``` json
{
        "registers": {
                "Seconds": {
                        "size": "1",
                        "offset": "0x03",
                        "access_mask": "0x7F"
                },
                "Minutes": {
                        "size": "1",
                        "offset": "0x04",
                        "access_mask": "0x7F"
                },
                "Hours": {
                        "size": "1",
                        "offset": "0x05",
                        "access_mask": "0x3F"
                },
                "Days": {
                        "size": "1",
                        "offset": "0x06",
                        "access_mask": "0x3F"
                },
                "Weekdays": {
                        "size": "1",
                        "offset": "0x07",
                        "access_mask": "0x03"
                },
                "Months": {
                        "size": "1",
                        "offset": "0x08",
                        "access_mask": "0x1F"
                },
                "Years": {
                        "size": "1",
                        "offset": "0x09",
                        "access_mask": "0xFF"
                }
        }
}
```

The following code then reads and prints the RTC time and the local system time:
``` c++
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
```
```
../build/pcf8523_demo
PCF8523 Time: 2017-01-21 16:50:13
System  Time: 2017-01-21 16:50:13
```

