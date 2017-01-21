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

#ifndef __REGMAP_I2C__
#define __REGMAP_I2C__

#include "IRegBackend.hpp"
#include "RegMapBase.hpp"

namespace regmap { namespace i2c {


class I2C : public RegMapBase<RegBackendI2CDev> {

public:
	I2C(unsigned char bus, unsigned char slave_id, const std::string &defFile);
	
private:
	static void closeDeleter(int* fd);

	BackendFile_t  m_pFile;
	RegBackendI2CDev m_oRegBackendI2CDev;
};

}};

#endif
