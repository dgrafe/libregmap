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

#ifndef __RegMapBase__
#define __RegMapBase__

#include <map>
#include <boost/format.hpp>
#include <boost/any.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "RegisterBase.hpp"

namespace regmap {
namespace pt = boost::property_tree;

typedef std::map<std::string, boost::any> RegisterMap_t;

template <class TBackend>
class RegMapBase {

public:
	RegMapBase() = delete;
	virtual ~RegMapBase() {}
	RegMapBase(std::string defFile) {
		this->createFromFile(defFile);
	}

	template <class T>
	T get(std::string key) {
		try {
			if (m_oRegisters.end() == m_oRegisters.find(key))
				throw std::runtime_error("No register found with name " + key);

			return boost::any_cast<T>(m_oRegisters[key]);
		} catch (const boost::bad_any_cast &ex) {
			throw std::runtime_error("Invalid register size for " + key);
		}
	}

private:
	void createFromFile(const std::string &filename) {

		pt::ptree pTree;
		try {
			pt::read_json(filename, pTree);
		} catch (...) {
			throw std::runtime_error("Definition file could not be parsed: " + filename);
		}

		// parse and create all registers
		for (auto &node : pTree.get_child("registers")) {

			std::string key = node.first;
			unsigned int offset = static_cast<unsigned int>(strtoul(node.second.get<std::string>("offset").c_str(), NULL, 0));
			unsigned int size = static_cast<unsigned int>(strtoul(node.second.get<std::string>("size").c_str(), NULL, 0));
			unsigned int busy_mask = static_cast<unsigned int>(strtoul(node.second.get<std::string>("busy_mask", "0").c_str(), NULL, 0));
			unsigned int ready_mask = static_cast<unsigned int>(strtoul(node.second.get<std::string>("ready_mask", "0").c_str(), NULL, 0));
			switch (size) {
				case 1:
				m_oRegisters[key] = Register8_t(key, m_oRegBackend, offset, static_cast<std::uint8_t>(busy_mask), static_cast<std::uint8_t>(ready_mask));
				break;

				case 2:
				m_oRegisters[key] = Register16_t(key, m_oRegBackend, offset, static_cast<std::uint16_t>(busy_mask), static_cast<std::uint16_t>(ready_mask));
				break;

				case 4:
				m_oRegisters[key] = Register32_t(key, m_oRegBackend, offset, static_cast<std::uint32_t>(busy_mask), static_cast<std::uint32_t>(ready_mask));
				break;

				default:
				throw std::runtime_error("Size out of range for register " + key);
				break;
			}

			// parse defined bitmasks
			try {
				for (auto &bitmask : node.second.get_child("bitmasks")) {

					std::string maskName = bitmask.first;
					long value = strtoul(bitmask.second.data().c_str(), NULL, 0);

					switch (size) {
						case 1:
						boost::any_cast<Register8_t&>(m_oRegisters[key]).addBitmask(maskName, (std::uint8_t)(value));
						break;

						case 2:
						boost::any_cast<Register16_t&>(m_oRegisters[key]).addBitmask(maskName, (std::uint16_t)(value));
						break;

						case 4:
						boost::any_cast<Register32_t&>(m_oRegisters[key]).addBitmask(maskName, (std::uint32_t)(value));
						break;
					}
				}
			}
			catch (const pt::ptree_bad_path &ex) {}
		}
	}

	RegisterMap_t	m_oRegisters;

protected:
	TBackend	m_oRegBackend;
};

} // endof namespace regmap

#endif
