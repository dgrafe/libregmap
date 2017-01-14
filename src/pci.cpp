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

#include <stdexcept>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <sys/mman.h>
#include "pci.hpp"

namespace regmap { namespace pci {

MemMapped::MemMapped(const PCI_ID &pciID, const std::string &defFile, const eBARs &bar, unsigned char instance)
: RegMapBase(defFile), PCICommon(pciID) {

	m_oRegBackendMemory = RegBackendMemory(PCICommon::memMapBar(bar), PCICommon::barSize(bar));
	m_oRegBackend = m_oRegBackendMemory;
}

MemMapped::MemMapped(const BDF &bdf, const std::string &defFile, const eBARs &bar)
: RegMapBase(defFile), PCICommon(bdf) {
	
	m_oRegBackendMemory = RegBackendMemory(PCICommon::memMapBar(bar), PCICommon::barSize(bar));
	m_oRegBackend = m_oRegBackendMemory;
}

IOMapped::IOMapped(const PCI_ID &pciID, const std::string &defFile, const eBARs &bar, unsigned char instance)
: RegMapBase(defFile), PCICommon(pciID) {

	m_oRegBackendFile = RegBackendFile(PCICommon::ioMapBar(bar), PCICommon::barSize(bar));
	m_oRegBackend = m_oRegBackendFile;
}

IOMapped::IOMapped(const BDF &bdf, const std::string &defFile, const eBARs &bar)
: RegMapBase(defFile), PCICommon(bdf) {
	
	m_oRegBackendFile = RegBackendFile(PCICommon::ioMapBar(bar), PCICommon::barSize(bar));
	m_oRegBackend = m_oRegBackendFile;
}


PCICommon::PCICommon(const PCI_ID &pciID, unsigned char instance) {

	boost::filesystem::path p("/sys/bus/pci/devices/");

	for (auto &entry : boost::make_iterator_range(boost::filesystem::directory_iterator(p), {})) {

		boost::filesystem::path vendor(entry.path().string() + "/vendor");	
		boost::filesystem::path device(entry.path().string() + "/device");	

		auto comp = [](boost::filesystem::path path, std::uint16_t value) -> bool {
			std::uint16_t file_value;
			std::ifstream f(path.string(), std::fstream::in);
			f >> std::hex >> file_value;

			return file_value == value;
		};

		if (!boost::filesystem::is_regular_file(vendor) || !comp(vendor, pciID.m_uVendor))
			continue;
		if (!boost::filesystem::is_regular_file(device) || !comp(device, pciID.m_uDevice))
			continue;	

		if (--instance > 0)
			continue;

		m_sSysFSPath = entry.path().string() + "/";
	}

	if(!m_sSysFSPath.length())
		throw std::runtime_error("Could not find PCI device with id " +
					std::to_string(pciID.m_uVendor) + ":"
					+ std::to_string(pciID.m_uDevice));
}

PCICommon::PCICommon(const BDF &bdf) {

	char sBDF[20];
	sprintf(sBDF, "0000:%0.2d:%0.2d.%d", bdf.m_uBus, bdf.m_uDevice, bdf.m_uFunction);
	boost::filesystem::path p("/sys/bus/pci/devices/" + std::string(sBDF));

	if (!boost::filesystem::is_directory(p))
		throw std::runtime_error("SysyFS path does not exist for bdf " + std::string(sBDF));

	m_sSysFSPath = p.string() + "/";
}

void PCICommon::munmapDeleter(void* addr, std::size_t length) {

	munmap(addr, length);
}

void PCICommon::closeDeleter(int* fd) {

	close(*fd);
	delete fd;
}

std::shared_ptr<std::fstream> PCICommon::sysfsEntry(const std::string &entry, std::ios_base::openmode mode) {

	std::shared_ptr<std::fstream> f = std::make_shared<std::fstream>(m_sSysFSPath + entry, mode);
	if (!f->is_open())
		throw std::runtime_error("sysfs is missing entry " + entry);

	return f;
}


bool PCICommon::isEnabled() {

	long enableCount = 0;
	*PCICommon::sysfsEntry("enable") >> enableCount;

	return enableCount > 0;
}

void PCICommon::enableDevice() {

	*PCICommon::sysfsEntry("enable", std::ios_base::out) << std::string("1");
}

void PCICommon::disableDevice() {

	auto file = PCICommon::sysfsEntry("enable", std::ios_base::out);
	while (PCICommon::isEnabled())
		*file << std::string("0");
}

void PCICommon::removeDevice() {

	*PCICommon::sysfsEntry("remove", std::ios_base::out) << std::string("1");
}

void PCICommon::rescanDevice() {

	*PCICommon::sysfsEntry("rescan", std::ios_base::out) << std::string("1");
}

void PCICommon::resetDevice() {

	*PCICommon::sysfsEntry("reset", std::ios_base::out) << std::string("1");
}

void PCICommon::rescanBus() {

	std::fstream f("/sys/bus/pci/rescan", std::ios_base::out);
	if (!f.is_open())
		throw std::runtime_error("Unable to rescan the pci bus");

	f << std::string("1");
}

std::size_t PCICommon::barSize(const eBARs &bar) {
	
	boost::filesystem::path p(m_sSysFSPath + std::string("resource" + std::to_string(bar)));
	if (!boost::filesystem::is_regular_file(p))
		throw std::runtime_error("BAR does not exist: " + bar);

	return boost::filesystem::file_size(p);

}

BackendMemory_t PCICommon::memMapBar(const eBARs &bar) {

	std::size_t uBarSize = PCICommon::barSize(bar);

	int fd = open((m_sSysFSPath + std::string("resource") + std::to_string(bar)).c_str(), O_RDWR);
	if (-1 == fd)
		throw std::runtime_error("Unable to open bar" + std::to_string(bar));

	void *ptr = mmap(NULL, uBarSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
	if (MAP_FAILED == ptr)
		throw std::runtime_error("Could not memmap bar" + std::to_string(bar));

	return std::shared_ptr<void>(ptr, std::bind(&PCICommon::munmapDeleter, std::placeholders::_1, uBarSize));
}

BackendFile_t PCICommon::ioMapBar(const eBARs &bar) {

	BackendFile_t file(new int(), &PCICommon::closeDeleter);
	*file = open((m_sSysFSPath + std::string("resource") + std::to_string(bar)).c_str(), O_RDWR);
	
	if (-1 == *file)
		throw std::runtime_error("Could not iomap bar" + std::to_string(bar));

	return file;
}

}};

