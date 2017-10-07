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

#ifndef __IRegBackend__
#define __IRegBackend__

#include <memory>
#include <cstring>
#include <stdexcept>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

namespace regmap {

class IRegBackend {

public:
	IRegBackend() : m_addrOffset(0), m_dataOffset(0) {}

	template <class T>
	void set(unsigned int offset, T value) {

		if (m_addrOffset == 0) {
			// direct access
			this->write(offset, (void*)(&value), sizeof(T));
		} else {
			this->write(m_addrOffset, (void*)(&m_dataOffset), sizeof(T));
			this->write(m_dataOffset, (void*)(&value), sizeof(T));
		}
	}

	template <class T>
	T get(unsigned int offset) {
		T buf;

		if (m_addrOffset == 0) {
			// direct access
			this->read(offset, (void*)(&buf), sizeof(T));
		} else {
			this->write(m_addrOffset, (void*)(&m_dataOffset), sizeof(T));
			this->read(m_dataOffset, (void*)(&buf), sizeof(T));
		}
		return buf;
	}

	void setIndirection(unsigned int addrReg, unsigned int dataReg) {

		m_addrOffset = addrReg;
		m_dataOffset = dataReg;
	}

protected:
	virtual void write(unsigned int offset, void* value, size_t size){}
	virtual void read(unsigned int offset, void* value, size_t size){}

private:
	unsigned int m_addrOffset;
	unsigned int m_dataOffset;
};

typedef std::shared_ptr<void> BackendMemory_t;
class RegBackendMemory : public IRegBackend {

public:
	RegBackendMemory() {}
	RegBackendMemory(BackendMemory_t mem, size_t size)
	: m_pMem(mem), m_uSize(size) {}

private:
	void write(unsigned int offset, void* value, size_t size) {
		if (offset + size > m_uSize)
			throw std::out_of_range("RegBackendMemory: Given offset is out of range");

		memcpy((unsigned char*)(m_pMem.get())+offset, value, size);
	}

	void read(unsigned int offset, void* value, size_t size) {
		if (offset + size > m_uSize)
			throw std::out_of_range("RegBackendMemory: Given offset is out of range");

		memcpy(value, (unsigned char*)(m_pMem.get())+offset, size);
	}

	BackendMemory_t m_pMem;
	size_t		m_uSize;
};

typedef std::shared_ptr<int> BackendFile_t;
class RegBackendFile : public IRegBackend {

public:
	RegBackendFile() {}
	RegBackendFile(BackendFile_t file, size_t size)
	: m_pFile(file), m_uSize(size) {}

private:
	void write(unsigned int offset, void* value, size_t size) {
		if (offset + size > m_uSize)
			throw std::out_of_range("RegBackendFile: Given offset is out of range");

		lseek(*m_pFile, offset, SEEK_SET);
		if (-1 == ::write(*m_pFile, value, size))
			throw std::runtime_error("Error wrting to io mapped register");
	}

	void read(unsigned int offset, void* value, size_t size) {
		if (offset + size > m_uSize)
			throw std::out_of_range("RegBackenFile: Given offset is out of range");

		lseek(*m_pFile, offset, SEEK_SET);
		if (-1 == ::read(*m_pFile, value, size))
			throw std::runtime_error("Error reading io mapped register");
	}

	BackendFile_t	m_pFile;
	size_t		m_uSize;
};

class RegBackendI2CDev : public IRegBackend {

public:
	RegBackendI2CDev() {}
	RegBackendI2CDev(BackendFile_t file, unsigned char slave_addr)
	: m_pFile(file), m_uSlaveAddr(slave_addr) {}

private:
	void write(unsigned int offset, void* value, size_t size) {

		unsigned char outbuf[size+1];
		struct i2c_rdwr_ioctl_data packets;
		struct i2c_msg messages[1];

		messages[0].addr  = m_uSlaveAddr;
		messages[0].flags = 0;
		messages[0].len   = sizeof(outbuf);
		messages[0].buf   = outbuf;

		outbuf[0] = offset;
		memcpy(&outbuf[1], value, size);

		packets.msgs  = messages;
		packets.nmsgs = 1;

		if (ioctl(*m_pFile, I2C_RDWR, &packets) < 0) 
			throw std::runtime_error("Write to the i2c dev unsuccessful");
	}

	void read(unsigned int offset, void* value, size_t size) {

		unsigned char outbuf;
		struct i2c_rdwr_ioctl_data packets;
		struct i2c_msg messages[2];

		outbuf=offset;
		messages[0].addr  = m_uSlaveAddr;
		messages[0].flags = 0;
		messages[0].len   = sizeof(outbuf);
		messages[0].buf   = &outbuf;

		messages[1].addr  = m_uSlaveAddr;
		messages[1].flags = I2C_M_RD/* | I2C_M_NOSTART*/;
		messages[1].len   = size;
		messages[1].buf   = static_cast<unsigned char*>(value);

		packets.msgs      = messages;
		packets.nmsgs     = 2;

		if (ioctl(*m_pFile, I2C_RDWR, &packets) < 0) 
			throw std::runtime_error("Read on the i2c dev unsuccessful");
	}

	BackendFile_t	m_pFile;
	unsigned char	m_uSlaveAddr;
};

};

#endif
