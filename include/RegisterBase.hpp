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

#ifndef __RegisterBase__
#define __RegisterBase__

#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <map>
#include "IRegBackend.hpp"

namespace regmap {

template <class T>
class RegisterBase {

public:
	RegisterBase() = delete;
	RegisterBase(const RegisterBase&) = default;
	RegisterBase(RegisterBase&&) = default;
	virtual ~RegisterBase() = default;

	RegisterBase(std::string regName,
		IRegBackend& regBackend,
		unsigned int offset,
		T busy_mask,
		T ready_mask)
	: m_sRegName(regName),
	  m_oRegBackend(regBackend),
	  m_uOffset(offset),
	  m_uBusyMask(busy_mask),
	  m_uReadyMask(ready_mask) {}

	const std::string& getName() {
		return m_sRegName;
	}

	unsigned int getOffset() {
		return m_uOffset;
	}

	// register access
	RegisterBase<T>& operator=(T value) {
		m_oRegBackend.set(m_uOffset, value);
		return *this;
	}

	explicit operator T() const {
		return m_oRegBackend.get<T>(m_uOffset);
	}

	// operator overloading
	RegisterBase<T> operator^=(const T &mask) {
		T tmp = m_oRegBackend.get<T>(m_uOffset);
		tmp ^= mask;
		m_oRegBackend.set(m_uOffset, tmp);
		return *this;
	}

	T operator^(const T &mask) const {
		T tmp = m_oRegBackend.get<T>(m_uOffset);
		return tmp ^ mask;
	}
	
	T operator^(const typename std::make_signed<T>::type &mask) const {
		T tmp = m_oRegBackend.get<T>(m_uOffset);
		return tmp ^ mask;
	}

	RegisterBase<T> operator|=(const T &mask) {
		T tmp = m_oRegBackend.get<T>(m_uOffset);
		tmp |= mask;
		m_oRegBackend.set(m_uOffset, tmp);
		return *this;
	}

	T operator|(const T &mask) const {
		T tmp = m_oRegBackend.get<T>(m_uOffset);
		return tmp | mask;
	}
	
	T operator|(const typename std::make_signed<T>::type &mask) const {
		T tmp = m_oRegBackend.get<T>(m_uOffset);
		return tmp | mask;
	}

	RegisterBase<T> operator&=(const T &mask) {
		T tmp = m_oRegBackend.get<T>(m_uOffset);
		tmp &= mask;
		m_oRegBackend.set(m_uOffset, tmp);
		return *this;
	}
	
	T operator&(const T &mask) const {
		T tmp = m_oRegBackend.get<T>(m_uOffset);
		return tmp & mask;
	}
	
	T operator&(const typename std::make_signed<T>::type &mask) const {
		T tmp = m_oRegBackend.get<T>(m_uOffset);
		return tmp & mask;
	}
	
	T operator~() const {
		return ~m_oRegBackend.get<T>(m_uOffset);
	}
	
	T operator<<(const T &steps) const {
		T tmp = m_oRegBackend.get<T>(m_uOffset);
		return tmp << steps;
	}

	T operator<<(const typename std::make_signed<T>::type &steps) const {
		T tmp = m_oRegBackend.get<T>(m_uOffset);
		return tmp << steps;
	}

	RegisterBase<T> operator<<=(const T &steps) {
		T tmp = m_oRegBackend.get<T>(m_uOffset);
		tmp <<= steps;
		m_oRegBackend.set(m_uOffset, tmp);
		return *this;
	}
	
	T operator>>(const T &steps) const {
		T tmp = m_oRegBackend.get<T>(m_uOffset);
		return tmp >> steps;
	}
	
	T operator>>(const typename std::make_signed<T>::type &steps) const {
		T tmp = m_oRegBackend.get<T>(m_uOffset);
		return tmp >> steps;
	}

	RegisterBase<T> operator>>=(const T &steps) {
		T tmp = m_oRegBackend.get<T>(m_uOffset);
		tmp >>= steps;
		m_oRegBackend.set(m_uOffset, tmp);
		return *this;
	}

	T operator!() const {
		return  !m_oRegBackend.get<T>(m_uOffset);
	}

	bool operator==(const T &right) const {
		return m_oRegBackend.get<T>(m_uOffset) == right;
	}

	bool operator==(const typename std::make_signed<T>::type &right) const {
		return m_oRegBackend.get<T>(m_uOffset) == right;
	}
	
	bool operator!=(const T &right) const {
		return m_oRegBackend.get<T>(m_uOffset) != right;
	}

	bool operator!=(const typename std::make_signed<T>::type &right) const {
		return m_oRegBackend.get<T>(m_uOffset) != right;
	}
	
	operator T() {
		return m_oRegBackend.get<T>(m_uOffset);
	}

	T operator[](const char *string) {
		return operator[](std::string(string));
	}

	// some helper functions
	T set(const T &mask) {
		return this->operator|=(mask);
	}
	T unset(const T &mask) {
		return this->operator&=(~mask);
	}
	bool is_set(const T &mask) const {
		return this->operator&(mask) == mask;
	}
	bool is_set(const std::string &mask) {
		T bitmask = this->operator[](mask);
		return this->operator&(bitmask) == bitmask;
	}

	// busy and ready mask related functions
	void set_ready_mask(const T &mask) {
		m_uReadyMask = mask;
	}

	T get_ready_mask() const {
		return m_uReadyMask;
	}
	
	void set_busy_mask(const T &mask) {
		m_uBusyMask = mask;
	}

	T get_busy_mask() const {
		return m_uBusyMask;
	}

	// block until the busy mask is cleared
	inline void work() { this->work(-1); }
	bool work(int timeout_ms);

	// block until the ready mask is set
	inline void wait() { this->wait(-1); }
	bool wait(int timeout_ms);

	// bitmask accessor
	T operator[](const std::string &name);

	void addBitmask(const std::string &name, T mask);
		

protected:
	std::string			m_sRegName;
	IRegBackend&			m_oRegBackend;
	unsigned int			m_uOffset;
	std::map<std::string, T>	m_oBitmasks;
	T				m_uBusyMask;
	T				m_uReadyMask;

	friend std::ostream& operator<<(std::ostream& os, const RegisterBase<T>& obj) {
		os << (T)obj;
		return os;
	}
};


typedef RegisterBase<std::uint32_t> Register32_t;
typedef RegisterBase<std::uint16_t> Register16_t;
typedef RegisterBase<std::uint8_t>  Register8_t;

};

#endif
