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
#include <sys/mman.h>
#include "devmem.hpp"

namespace regmap { namespace devmem {

DevMem::DevMem(std::uint32_t physStart, std::uint32_t physEnd, const std::string &defFile)
: RegMapBase(defFile) {

	if (physEnd - physStart <= 0)
		std::runtime_error("DevMem: Illegal physical adresses given");

	std::uint32_t regionSize = physEnd - physStart;

	int fd = open("/dev/mem", O_RDWR);
	if (-1 == fd)
		throw std::runtime_error("Unable to open /dev/mem");

	void *ptr = mmap(NULL, regionSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, physStart);
	close(fd);
	if (MAP_FAILED == ptr)
		throw std::runtime_error("Could not memmap given devmem region " + std::to_string(physStart) + "-" + std::to_string(physEnd));

	m_pMemory = std::shared_ptr<void>(ptr, std::bind(&DevMem::munmapDeleter, std::placeholders::_1, regionSize));
	m_oRegBackendMemory = RegBackendMemory(m_pMemory, regionSize);
	m_oRegBackend = m_oRegBackendMemory;
}

void DevMem::munmapDeleter(void* addr, std::size_t length) {

	munmap(addr, length);
}

}};

