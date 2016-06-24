/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once
#include <etk/types.h>

namespace zeus {
	/**
	 * @brief get the mine type with the file extention
	 * @param[in] _extention file extention (without the '.')
	 * @return The generic mine tipe in http format
	 */
	std::string getMineType(const std::string& _extention);
	/**
	 * @brief Retrive the extention of a file with his mine type
	 * @param[in] _mineType Mine tipe in http format
	 * @return file extention (without the '.')
	 */
	std::string getExtention(const std::string& _mineType);
};

