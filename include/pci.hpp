/* This file is part of libregmap.
 *
 * libregmap is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libregmap is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libregmap.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __REGMAP_PCI__
#define __REGMAP_PCI__

#include "IRegBackend.hpp"
#include "RegMapBase.hpp"

namespace regmap { namespace pci {

struct PCI_ID {

	PCI_ID(std::uint16_t vendor, std::uint16_t device)
	: m_uVendor(vendor), m_uDevice(device) {}

	std::uint16_t m_uVendor;
	std::uint16_t m_uDevice;
};

struct BDF {

	BDF(std::uint8_t bus, std::uint8_t device, std::uint8_t function)
	: m_uBus(bus), m_uDevice(device), m_uFunction(function){}

	std::uint8_t m_uBus;
	std::uint8_t m_uDevice;
	std::uint8_t m_uFunction;
};

enum eBARs {
	BAR0 = 0,
	BAR1 = 1,
	BAR2 = 2,
	BAR3 = 3,
	BAR4 = 4,
	BAR5 = 5,
	BAR6 = 6,
	BAR7 = 7,
	BAR8 = 8
};

class PCICommon {

public:
	PCICommon(const PCI_ID &pciID, unsigned char instance = 1);
	PCICommon(const BDF &bdf);
	bool isEnabled();
	void enableDevice();
	void removeDevice();
	void rescanDevice();

	std::size_t barSize(const eBARs &bar);

protected:
	BackendMemory_t memMapBar(const eBARs &bar);
	BackendFile_t ioMapBar(const eBARs &bar);

private:
	static void munmapDeleter(void* addr, std::size_t length);
	static void closeDeleter(int* fd);
	std::string m_sSysFSPath;
};

class MemMapped : public RegMapBase<RegBackendMemory>, public PCICommon {

public:
	MemMapped(const PCI_ID &pciID, const std::string &defFile, const eBARs &bar, unsigned char instance = 1);
	MemMapped(const BDF &bdf, const std::string &defFile, const eBARs &bar);
	
private:
	BackendMemory_t  m_pMemory;
	RegBackendMemory m_oRegBackendMemory;
};

class IOMapped : public RegMapBase<RegBackendFile>, public PCICommon {

public:
	IOMapped(const PCI_ID &pciID, const std::string &defFile, const eBARs &bar, unsigned char instance = 1);
	IOMapped(const BDF &bdf, const std::string &defFile, const eBARs &bar);
	
private:
	BackendFile_t	m_pFile;
	RegBackendFile	m_oRegBackendFile;
};


}};

#endif
