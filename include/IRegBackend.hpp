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
#include <fcntl.h>
#include <unistd.h>

namespace regmap {

class IRegBackend {

public:
	IRegBackend(){}

	template <class T>
	void set(unsigned int offset, T value) {
		this->write(offset, (void*)(&value), sizeof(T));
	}
	template <class T>
	T get(unsigned int offset) {
		T buf;
		this->read(offset, (void*)(&buf), sizeof(T));
		return buf;
	}

protected:
	virtual void write(unsigned int offset, void* value, size_t size){}
	virtual void read(unsigned int offset, void* value, size_t size){}
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
			throw std::runtime_error("Error reading io mapped register");
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

};

#endif
