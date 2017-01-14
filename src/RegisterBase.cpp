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

#include <thread>
#include <chrono>
#include "RegisterBase.hpp"

namespace regmap {

template <class T>
template <class U>
bool RegisterBase<T>::wait(const U &timeout) {
		
	if (m_uReadyMask == 0)
		throw std::runtime_error("No ready mask set for register " + m_sRegName);

	auto start = std::chrono::high_resolution_clock::now();
	while(!this->is_set(m_uReadyMask)) {
		auto end = std::chrono::high_resolution_clock::now();
		U elapsed = std::chrono::duration_cast<U>(end-start);
		if (timeout.count() && elapsed.count() > timeout.count())
			return false;

		std::this_thread::sleep_for(U(1));
	}

	return true;
}

template <class T>
template <class U>
bool RegisterBase<T>::work(const U &timeout) {

	if (m_uBusyMask == 0)
		throw std::runtime_error("No busy mask set for register " + m_sRegName);

	auto start = std::chrono::high_resolution_clock::now();
	while(this->is_set(m_uBusyMask)) {
		auto end = std::chrono::high_resolution_clock::now();
		U elapsed = std::chrono::duration_cast<U>(end-start);
		if (timeout.count() && elapsed.count() > timeout.count())
			return false;

		std::this_thread::sleep_for(U(1));
	}

	return true;
}

template <class T>
T RegisterBase<T>::operator[](const std::string &name) {

	if (m_oBitmasks.end() == m_oBitmasks.find(name))
		throw std::runtime_error("Bitmap not defined: " + name);

	return m_oBitmasks[name];
}

template <class T>
void RegisterBase<T>::addBitmask(const std::string &name, T mask) {
		
	if (m_oBitmasks.end() != m_oBitmasks.find(name))
		throw std::runtime_error("Bitmap already defined: " + name);

	m_oBitmasks[name] = mask;
}

template bool RegisterBase<std::uint8_t>::work(const std::chrono::nanoseconds&);
template bool RegisterBase<std::uint8_t>::work(const std::chrono::microseconds&);
template bool RegisterBase<std::uint8_t>::work(const std::chrono::milliseconds&);
template bool RegisterBase<std::uint8_t>::work(const std::chrono::seconds&);

template bool RegisterBase<std::uint8_t>::wait(const std::chrono::nanoseconds&);
template bool RegisterBase<std::uint8_t>::wait(const std::chrono::microseconds&);
template bool RegisterBase<std::uint8_t>::wait(const std::chrono::milliseconds&);
template bool RegisterBase<std::uint8_t>::wait(const std::chrono::seconds&);

template bool RegisterBase<std::uint16_t>::work(const std::chrono::nanoseconds&);
template bool RegisterBase<std::uint16_t>::work(const std::chrono::microseconds&);
template bool RegisterBase<std::uint16_t>::work(const std::chrono::milliseconds&);
template bool RegisterBase<std::uint16_t>::work(const std::chrono::seconds&);

template bool RegisterBase<std::uint16_t>::wait(const std::chrono::nanoseconds&);
template bool RegisterBase<std::uint16_t>::wait(const std::chrono::microseconds&);
template bool RegisterBase<std::uint16_t>::wait(const std::chrono::milliseconds&);
template bool RegisterBase<std::uint16_t>::wait(const std::chrono::seconds&);

template bool RegisterBase<std::uint32_t>::work(const std::chrono::nanoseconds&);
template bool RegisterBase<std::uint32_t>::work(const std::chrono::microseconds&);
template bool RegisterBase<std::uint32_t>::work(const std::chrono::milliseconds&);
template bool RegisterBase<std::uint32_t>::work(const std::chrono::seconds&);

template bool RegisterBase<std::uint32_t>::wait(const std::chrono::nanoseconds&);
template bool RegisterBase<std::uint32_t>::wait(const std::chrono::microseconds&);
template bool RegisterBase<std::uint32_t>::wait(const std::chrono::milliseconds& timeout);
template bool RegisterBase<std::uint32_t>::wait(const std::chrono::seconds&);

template class RegisterBase<std::uint8_t>;
template class RegisterBase<std::uint16_t>;
template class RegisterBase<std::uint32_t>;

};
