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

#ifndef __REGMAP_DEVMEM__
#define __REGMAP_DEVMEM__

#include "IRegBackend.hpp"
#include "RegMapBase.hpp"

namespace regmap { namespace devmem {

class DevMem : public RegMapBase<RegBackendMemory> {

public:
	DevMem(std::uint32_t physStart, std::uint32_t physEnd, const std::string &defFile);
	
private:
	static void munmapDeleter(void* addr, std::size_t length);
	BackendMemory_t  m_pMemory;
	RegBackendMemory m_oRegBackendMemory;
};

}};

#endif
