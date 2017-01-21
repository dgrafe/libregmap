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

#ifndef __REGMAP_COMVERSIONS__
#define __REGMAP_CONVERSIONS__

#include <cstdint>

namespace regmap { namespace bcd {

template <class T>
static inline T to_dec(const T& value) {
	return (value) ? ((to_dec(value >> 4) * 10) + (value % 16)) : 0;
}

template <class T>
static inline T to_bcd(const T& value) {
	return (value) ? ((to_bcd(value / 10) << 4) + (value % 10)) : 0;
}

template <class T>
static inline T to_dec(const regmap::RegisterBase<T>& obj) {
	return to_dec(static_cast<T>(obj));
}

template <class T>
static inline T to_bcd(const regmap::RegisterBase<T>& obj) {
	return to_bcd(static_cast<T>(obj));
}

}};

#endif
