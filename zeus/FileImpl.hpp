/** @file
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <zeus/File.hpp>
#include <etk/os/FSNode.hpp>

namespace zeus {
	class FileImpl : public zeus::File {
		private:
			std::string m_filename;
			etk::FSNode m_node;
			size_t m_size;
			std::string m_mineType;
		public:
			FileImpl(std::string _filename);
			~FileImpl();
			uint64_t getSize() override;
			std::string getName() override;
			std::string getMineType() override;
			zeus::Raw getPart(uint64_t _start, uint64_t _stop) override;
		
	};
}

