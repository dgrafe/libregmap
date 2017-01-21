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
#include "i2c.hpp"

namespace regmap { namespace i2c {

I2C::I2C(unsigned char bus, unsigned char slave_addr, const std::string &defFile)
: RegMapBase(defFile) {

	BackendFile_t file(new int(), &I2C::closeDeleter);
	*file = open(std::string("/dev/i2c-" + std::to_string(bus)).c_str(), O_RDWR);
	if (0 > *file)
		throw std::runtime_error("Unable to open i2c bus " + std::to_string(bus));

	if (0 > ioctl(*file, I2C_SLAVE, slave_addr))
		throw std::runtime_error("Could not access slave with address" + std::to_string(slave_addr));

	m_oRegBackendI2CDev = RegBackendI2CDev(file, slave_addr);
	m_oRegBackend = m_oRegBackendI2CDev;
}

void I2C::closeDeleter(int* fd) {

	close(*fd);
	delete fd;
}

}};
