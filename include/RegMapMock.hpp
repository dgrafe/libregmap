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

#ifndef __REG_MAP_MOCK__
#define __REG_MAP_MOCK__

#include <memory>
#include "RegMapBase.hpp"

namespace regmap {

class RegMapMock : public RegMapBase<RegBackendMemory> {

public:
	RegMapMock(std::string defFile, unsigned int size)
	: RegMapBase(defFile), m_pMemory(malloc(size), free) {
		m_oRegBackendMemory = RegBackendMemory(m_pMemory, size);
		m_oRegBackend = m_oRegBackendMemory;
	}

private:
	BackendMemory_t  m_pMemory;
	RegBackendMemory m_oRegBackendMemory;
};

};

#endif
