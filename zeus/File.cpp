/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#include <zeus/File.h>
#include <zeus/debug.h>
#include <etk/types.h>
#include <etk/stdTools.h>
#include <zeus/mineType.h>
#include <etk/os/FSNode.h>



zeus::File::File() {
	
}

zeus::File::File(const std::string& _filename) {
	m_data = etk::FSNodeReadAllDataType<uint8_t>(_filename);
	std::string extention = std::string(_filename.begin()+_filename.size() -3, _filename.end());
	ZEUS_WARNING("send file: '" << _filename << "' with extention: '" << extention << "'");
	m_mineType = zeus::getMineType(extention);
}

void zeus::File::storeIn(const std::string& _filename) const {
	etk::FSNodeWriteAllDataType(_filename, m_data);
}

zeus::File::File(const std::string& _mineType, std::vector<uint8_t> _data, int32_t _fileSize):
  m_mineType(_mineType),
  m_data(_data) {
	if (_fileSize < 0){
		m_fileSize = m_data.size();
	} else {
		m_fileSize = _fileSize;
	}
}

void zeus::File::setData(uint64_t _offset, const std::vector<uint8_t>& _data) {
	// TODO : Check size/offset before set
	memcpy(&m_data[_offset], &_data[0], _data.size());
}


zeus::FileServer::FileServer() {
	
}
zeus::FileServer::FileServer(const std::string& _filename) :
  m_name(_filename) {
	
}


namespace etk {
	template<>
	std::string to_string<zeus::FileServer>(zeus::FileServer const& _obj) {
		return "";
	}
}

