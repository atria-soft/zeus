/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#include <etk/types.h>
#include <zeus/Buffer.h>
#include <zeus/debug.h>
#include <zeus/ParamType.h>
#include <etk/stdTools.h>
#include <zeus/AbstractFunction.h>
#include <climits>

namespace etk {
	template<> std::string to_string<enum zeus::Buffer::typeMessage>(const enum zeus::Buffer::typeMessage& _value) {
		switch (_value) {
			case zeus::Buffer::typeMessage::call:
				return "call";
			case zeus::Buffer::typeMessage::answer:
				return "answer";
			case zeus::Buffer::typeMessage::event:
				return "event";
			case zeus::Buffer::typeMessage::data:
				return "event";
		}
		return "???";
	}
}
std::ostream& zeus::operator <<(std::ostream& _os, enum zeus::Buffer::typeMessage _value) {
	_os << etk::to_string(_value);
	return _os;
}

static enum zeus::Buffer::typeMessage getTypeType(uint16_t _value) {
	switch (_value) {
		case 1:
			return zeus::Buffer::typeMessage::call;
		case 2:
			return zeus::Buffer::typeMessage::answer;
		case 4:
			return zeus::Buffer::typeMessage::event;
		case 8:
			return zeus::Buffer::typeMessage::data;
	}
	return zeus::Buffer::typeMessage::call;
}

zeus::Buffer::Buffer() {
	clear();
}

void zeus::Buffer::internalComposeWith(const uint8_t* _buffer, uint32_t _lenght) {
	clear();
	uint32_t offset = 0;
	memcpy(reinterpret_cast<char*>(&m_header), &_buffer[offset], sizeof(headerBin));
	offset += sizeof(headerBin);
	if (m_header.numberOfParameter != 0) {
		m_paramOffset.resize(m_header.numberOfParameter);
		memcpy(&m_paramOffset[0], &_buffer[offset], m_header.numberOfParameter * sizeof(uint16_t));
		offset += m_header.numberOfParameter * sizeof(uint16_t);
		m_data.resize(_lenght - offset);
		memcpy(&m_data[0], &_buffer[offset], m_data.size());
	} else {
		// TODO : check size ...
	}
	ZEUS_DEBUG("Get binary messages " << generateHumanString());
}

void zeus::Buffer::composeWith(const std::vector<uint8_t>& _buffer) {
	internalComposeWith(&_buffer[0], _buffer.size());
}

void zeus::Buffer::clear() {
	ZEUS_VERBOSE("clear buffer");
	m_data.clear();
	m_paramOffset.clear();
	m_header.versionProtocol = 1;
	m_header.transactionID = 1;
	m_header.clientID = 0;
	m_header.partID = 0x8000;
	m_header.typeMessage = 1;
	m_header.numberOfParameter = 1;
}
std::string zeus::Buffer::generateHumanString() {
	std::string out = "zeus::Buffer Lenght=: ";
	out += " v=" + etk::to_string(m_header.versionProtocol);
	out += " id=" + etk::to_string(m_header.transactionID);
	out += " cId=" + etk::to_string(m_header.clientID);
	out += " pId=" + etk::to_string(getPartId());
	out += " finish=" + etk::to_string(getPartFinish());
	enum zeus::Buffer::typeMessage type = getTypeType(m_header.typeMessage);
	out += " type=" + etk::to_string(type);
	switch (type) {
		case zeus::Buffer::typeMessage::call:
			out += " nbParam=" + etk::to_string(getNumberParameter());
			out += " call='" + getCall() + "'";
			break;
		case zeus::Buffer::typeMessage::answer:
			if (m_paramOffset.size() == 1) {
				out += " mode=Value";
			} else if (m_paramOffset.size() == 2) {
				out += " mode=Error";
			} else if (m_paramOffset.size() == 3) {
				out += " mode=Value+Error";
			} else {
				out += " mode=???";
			}
			break;
		case zeus::Buffer::typeMessage::event:
			
			break;
		case zeus::Buffer::typeMessage::data:
			
			break;
	}
	if (getNumberParameter() != 0) {
		out += " paramType(";
		for (int32_t iii=0; iii< getNumberParameter(); ++iii) {
			if (iii != 0) {
				out += ",";
			}
			out += internalGetParameterType(iii);
		}
		out += ")";
	}
	return out;
}

uint16_t zeus::Buffer::getProtocalVersion() const {
	return m_header.versionProtocol;
}

void zeus::Buffer::setProtocolVersion(uint16_t _value) {
	ZEUS_VERBOSE("setProtocolVersion :" << _value);
	m_header.versionProtocol = _value;
}

uint32_t zeus::Buffer::getTransactionId() const {
	return m_header.transactionID;
}

void zeus::Buffer::setTransactionId(uint32_t _value) {
	ZEUS_VERBOSE("setTransactionId :" << _value);
	m_header.transactionID = _value;
}

uint32_t zeus::Buffer::getClientId() const {
	return m_header.clientID;
}

void zeus::Buffer::setClientId(uint32_t _value) {
	ZEUS_VERBOSE("setClientId :" << _value);
	m_header.clientID = _value;
}

// note limited 15 bits
uint16_t zeus::Buffer::getPartId() const {
	return uint16_t(m_header.partID & 0x7FFF);
}

void zeus::Buffer::setPartId(uint16_t _value) {
	ZEUS_VERBOSE("setPartId :" << _value);
	m_header.partID = (m_header.partID&0x8000) | (_value & 0x7FFF);
}

bool zeus::Buffer::getPartFinish() const {
	return m_header.partID<0;
}

void zeus::Buffer::setPartFinish(bool _value) {
	ZEUS_VERBOSE("setPartFinish :" << _value);
	if (_value == true) {
		m_header.partID = (m_header.partID & 0x7FFF) | 0x8000;
	} else {
		m_header.partID = m_header.partID & 0x7FFF;
	}
}

enum zeus::Buffer::typeMessage zeus::Buffer::getType() const {
	return (enum zeus::Buffer::typeMessage)m_header.typeMessage;
}

void zeus::Buffer::setType(enum typeMessage _value) {
	ZEUS_VERBOSE("setType :" << _value);
	m_header.typeMessage = uint16_t(_value);
}

uint16_t zeus::Buffer::getNumberParameter() const {
	return m_paramOffset.size()-1;
}
std::string zeus::Buffer::internalGetParameterType(int32_t _id) const {
	std::string out;
	if (m_paramOffset.size() <= _id) {
		ZEUS_ERROR("out of range Id for parameter ... " << _id << " have " << m_paramOffset.size());
		return out;
	}
	out = reinterpret_cast<const char*>(&m_data[m_paramOffset[_id]]);
	return out;
}
std::string zeus::Buffer::getParameterType(int32_t _id) const {
	return internalGetParameterType(_id + 1);
}

const uint8_t* zeus::Buffer::internalGetParameterPointer(int32_t _id) const {
	const uint8_t* out = nullptr;
	if (m_paramOffset.size() <= _id) {
		ZEUS_ERROR("out of range Id for parameter ... " << _id << " have " << m_paramOffset.size());
		return out;
	}
	out = reinterpret_cast<const uint8_t*>(&m_data[m_paramOffset[_id]]);
	if (out == nullptr) {
		return out;
	}
	// TODO : unlock if > 1024
	while (*out != 0) {
		out++;
	}
	out++;
	return out;
}

const uint8_t* zeus::Buffer::getParameterPointer(int32_t _id) const {
	return internalGetParameterPointer(_id + 1);
}

uint32_t zeus::Buffer::internalGetParameterSize(int32_t _id) const {
	int32_t out = 0;
	if (m_paramOffset.size() <= _id) {
		ZEUS_ERROR("out of range Id for parameter ... " << _id << " have " << m_paramOffset.size());
		return out;
	}
	int32_t startPos = m_paramOffset[_id];
	int32_t endPos = m_data.size();
	if (m_paramOffset.size() > _id+1) {
		endPos = m_paramOffset[_id+1];
	}
	// First get type:
	const char* type = reinterpret_cast<const char*>(&m_data[startPos]); // Will be stop with \0 ...
	// move in the buffer pos
	startPos += strlen(type) + 1;
	// get real data size
	out = endPos - startPos;
	out --;
	if (out < 0) {
		ZEUS_ERROR("Get size < 0 : " << out);
		out = 0;
	}
	return out;
}



void zeus::Buffer::addData(void* _data, uint32_t _size) {
	m_paramOffset.clear();
	setType(zeus::Buffer::typeMessage::data);
	m_data.resize(_size);
	memcpy(&m_data[0], _data, _size);
}


uint32_t zeus::Buffer::getParameterSize(int32_t _id) const {
	return internalGetParameterSize(_id + 1);
}

void zeus::Buffer::addParameter() {
	int32_t currentOffset = m_data.size();
	m_paramOffset.push_back(currentOffset);
	m_data.push_back('v');
	m_data.push_back('o');
	m_data.push_back('i');
	m_data.push_back('d');
	m_data.push_back('\0');
}
void zeus::Buffer::addParameterEmptyVector() {
	int32_t currentOffset = m_data.size();
	m_paramOffset.push_back(currentOffset);
	m_data.push_back('v');
	m_data.push_back('e');
	m_data.push_back('c');
	m_data.push_back('t');
	m_data.push_back('o');
	m_data.push_back('r');
	m_data.push_back(':');
	m_data.push_back('e');
	m_data.push_back('m');
	m_data.push_back('p');
	m_data.push_back('t');
	m_data.push_back('y');
	m_data.push_back('\0');
}
template<>
void zeus::Buffer::addParameter<std::string>(const std::string& _value) {
	int32_t currentOffset = m_data.size();
	m_paramOffset.push_back(currentOffset);
	m_data.push_back('s');
	m_data.push_back('t');
	m_data.push_back('r');
	m_data.push_back('i');
	m_data.push_back('n');
	m_data.push_back('g');
	m_data.push_back('\0');
	currentOffset = m_data.size();
	m_data.resize(m_data.size()+_value.size()+1);
	memcpy(&m_data[currentOffset], &_value[0], _value.size());
}
template<>
void zeus::Buffer::addParameter<std::vector<std::string>>(const std::vector<std::string>& _value) {
	int32_t currentOffset = m_data.size();
	m_paramOffset.push_back(currentOffset);
	m_data.push_back('v');
	m_data.push_back('e');
	m_data.push_back('c');
	m_data.push_back('t');
	m_data.push_back('o');
	m_data.push_back('r');
	m_data.push_back(':');
	m_data.push_back('s');
	m_data.push_back('t');
	m_data.push_back('r');
	m_data.push_back('i');
	m_data.push_back('n');
	m_data.push_back('g');
	m_data.push_back('\0');
	// count all datas:
	uint32_t size = 0;
	for (auto &it : _value) {
		size+=it.size()+1;
	}
	uint16_t nb = _value.size();
	currentOffset = m_data.size();
	m_data.resize(m_data.size()+size+2);
	memcpy(&m_data[currentOffset], &nb, sizeof(uint16_t));
	currentOffset += sizeof(uint16_t);
	for (auto &it : _value) {
		memcpy(&m_data[currentOffset], &it[0], it.size());
		currentOffset += it.size();
		m_data[currentOffset] = '\0';
		currentOffset++;
	}
}

template<>
void zeus::Buffer::addParameter<std::vector<bool>>(const std::vector<bool>& _value) {
	int32_t currentOffset = m_data.size();
	m_paramOffset.push_back(currentOffset);
	m_data.push_back('v');
	m_data.push_back('e');
	m_data.push_back('c');
	m_data.push_back('t');
	m_data.push_back('o');
	m_data.push_back('r');
	m_data.push_back(':');
	m_data.push_back('b');
	m_data.push_back('o');
	m_data.push_back('o');
	m_data.push_back('l');
	m_data.push_back('\0');
	// add size:
	uint16_t nb = _value.size();
	currentOffset = m_data.size();
	m_data.resize(m_data.size()+_value.size());
	for (const auto &it : _value) {
		if (it == true) {
			m_data[currentOffset] = 'T';
		} else {
			m_data[currentOffset] = 'F';
		}
		currentOffset++;
	}
}

template<>
void zeus::Buffer::addParameter<std::vector<int8_t>>(const std::vector<int8_t>& _value) {
	int32_t currentOffset = m_data.size();
	m_paramOffset.push_back(currentOffset);
	m_data.push_back('v');
	m_data.push_back('e');
	m_data.push_back('c');
	m_data.push_back('t');
	m_data.push_back('o');
	m_data.push_back('r');
	m_data.push_back(':');
	m_data.push_back('i');
	m_data.push_back('n');
	m_data.push_back('t');
	m_data.push_back('8');
	m_data.push_back('\0');
	// add size:
	currentOffset = m_data.size();
	m_data.resize(m_data.size()+_value.size());
	memcpy(&m_data[currentOffset], &_value[0], sizeof(int8_t)*_value.size());
}
template<>
void zeus::Buffer::addParameter<std::vector<int16_t>>(const std::vector<int16_t>& _value) {
	int32_t currentOffset = m_data.size();
	m_paramOffset.push_back(currentOffset);
	m_data.push_back('v');
	m_data.push_back('e');
	m_data.push_back('c');
	m_data.push_back('t');
	m_data.push_back('o');
	m_data.push_back('r');
	m_data.push_back(':');
	m_data.push_back('i');
	m_data.push_back('n');
	m_data.push_back('t');
	m_data.push_back('1');
	m_data.push_back('6');
	m_data.push_back('\0');
	// add size:
	currentOffset = m_data.size();
	m_data.resize(m_data.size()+_value.size());
	memcpy(&m_data[currentOffset], &_value[0], sizeof(int16_t)*_value.size());
}
template<>
void zeus::Buffer::addParameter<std::vector<int32_t>>(const std::vector<int32_t>& _value) {
	int32_t currentOffset = m_data.size();
	m_paramOffset.push_back(currentOffset);
	m_data.push_back('v');
	m_data.push_back('e');
	m_data.push_back('c');
	m_data.push_back('t');
	m_data.push_back('o');
	m_data.push_back('r');
	m_data.push_back(':');
	m_data.push_back('i');
	m_data.push_back('n');
	m_data.push_back('t');
	m_data.push_back('3');
	m_data.push_back('2');
	m_data.push_back('\0');
	// add size:
	currentOffset = m_data.size();
	m_data.resize(m_data.size()+_value.size());
	memcpy(&m_data[currentOffset], &_value[0], sizeof(int32_t)*_value.size());
}
template<>
void zeus::Buffer::addParameter<std::vector<int64_t>>(const std::vector<int64_t>& _value) {
	int32_t currentOffset = m_data.size();
	m_paramOffset.push_back(currentOffset);
	m_data.push_back('v');
	m_data.push_back('e');
	m_data.push_back('c');
	m_data.push_back('t');
	m_data.push_back('o');
	m_data.push_back('r');
	m_data.push_back(':');
	m_data.push_back('i');
	m_data.push_back('n');
	m_data.push_back('t');
	m_data.push_back('6');
	m_data.push_back('4');
	m_data.push_back('\0');
	// add size:
	currentOffset = m_data.size();
	m_data.resize(m_data.size()+_value.size());
	memcpy(&m_data[currentOffset], &_value[0], sizeof(int64_t)*_value.size());
}

template<>
void zeus::Buffer::addParameter<std::vector<uint8_t>>(const std::vector<uint8_t>& _value) {
	int32_t currentOffset = m_data.size();
	m_paramOffset.push_back(currentOffset);
	m_data.push_back('v');
	m_data.push_back('e');
	m_data.push_back('c');
	m_data.push_back('t');
	m_data.push_back('o');
	m_data.push_back('r');
	m_data.push_back(':');
	m_data.push_back('u');
	m_data.push_back('i');
	m_data.push_back('n');
	m_data.push_back('t');
	m_data.push_back('8');
	m_data.push_back('\0');
	// add size:
	currentOffset = m_data.size();
	m_data.resize(m_data.size()+_value.size());
	memcpy(&m_data[currentOffset], &_value[0], sizeof(uint8_t)*_value.size());
}
template<>
void zeus::Buffer::addParameter<std::vector<uint16_t>>(const std::vector<uint16_t>& _value) {
	int32_t currentOffset = m_data.size();
	m_paramOffset.push_back(currentOffset);
	m_data.push_back('v');
	m_data.push_back('e');
	m_data.push_back('c');
	m_data.push_back('t');
	m_data.push_back('o');
	m_data.push_back('r');
	m_data.push_back(':');
	m_data.push_back('u');
	m_data.push_back('i');
	m_data.push_back('n');
	m_data.push_back('t');
	m_data.push_back('1');
	m_data.push_back('6');
	m_data.push_back('\0');
	// add size:
	currentOffset = m_data.size();
	m_data.resize(m_data.size()+_value.size());
	memcpy(&m_data[currentOffset], &_value[0], sizeof(uint16_t)*_value.size());
}
template<>
void zeus::Buffer::addParameter<std::vector<uint32_t>>(const std::vector<uint32_t>& _value) {
	int32_t currentOffset = m_data.size();
	m_paramOffset.push_back(currentOffset);
	m_data.push_back('v');
	m_data.push_back('e');
	m_data.push_back('c');
	m_data.push_back('t');
	m_data.push_back('o');
	m_data.push_back('r');
	m_data.push_back(':');
	m_data.push_back('u');
	m_data.push_back('i');
	m_data.push_back('n');
	m_data.push_back('t');
	m_data.push_back('3');
	m_data.push_back('2');
	m_data.push_back('\0');
	// add size:
	currentOffset = m_data.size();
	m_data.resize(m_data.size()+_value.size());
	memcpy(&m_data[currentOffset], &_value[0], sizeof(uint32_t)*_value.size());
}
template<>
void zeus::Buffer::addParameter<std::vector<uint64_t>>(const std::vector<uint64_t>& _value) {
	int32_t currentOffset = m_data.size();
	m_paramOffset.push_back(currentOffset);
	m_data.push_back('v');
	m_data.push_back('e');
	m_data.push_back('c');
	m_data.push_back('t');
	m_data.push_back('o');
	m_data.push_back('r');
	m_data.push_back(':');
	m_data.push_back('u');
	m_data.push_back('i');
	m_data.push_back('n');
	m_data.push_back('t');
	m_data.push_back('6');
	m_data.push_back('4');
	m_data.push_back('\0');
	// add size:
	currentOffset = m_data.size();
	m_data.resize(m_data.size()+_value.size());
	memcpy(&m_data[currentOffset], &_value[0], sizeof(uint64_t)*_value.size());
}

template<>
void zeus::Buffer::addParameter<std::vector<float>>(const std::vector<float>& _value) {
	int32_t currentOffset = m_data.size();
	m_paramOffset.push_back(currentOffset);
	m_data.push_back('v');
	m_data.push_back('e');
	m_data.push_back('c');
	m_data.push_back('t');
	m_data.push_back('o');
	m_data.push_back('r');
	m_data.push_back(':');
	m_data.push_back('f');
	m_data.push_back('l');
	m_data.push_back('o');
	m_data.push_back('a');
	m_data.push_back('t');
	m_data.push_back('\0');
	// add size:
	currentOffset = m_data.size();
	m_data.resize(m_data.size()+_value.size());
	memcpy(&m_data[currentOffset], &_value[0], sizeof(float)*_value.size());
}

template<>
void zeus::Buffer::addParameter<std::vector<double>>(const std::vector<double>& _value) {
	int32_t currentOffset = m_data.size();
	m_paramOffset.push_back(currentOffset);
	m_data.push_back('v');
	m_data.push_back('e');
	m_data.push_back('c');
	m_data.push_back('t');
	m_data.push_back('o');
	m_data.push_back('r');
	m_data.push_back(':');
	m_data.push_back('d');
	m_data.push_back('o');
	m_data.push_back('u');
	m_data.push_back('b');
	m_data.push_back('l');
	m_data.push_back('e');
	m_data.push_back('\0');
	// add size:
	currentOffset = m_data.size();
	m_data.resize(m_data.size()+_value.size());
	memcpy(&m_data[currentOffset], &_value[0], sizeof(double)*_value.size());
}

template<>
void zeus::Buffer::addParameter<int8_t>(const int8_t& _value) {
	int32_t currentOffset = m_data.size();
	m_paramOffset.push_back(currentOffset);
	m_data.push_back('i');
	m_data.push_back('n');
	m_data.push_back('t');
	m_data.push_back('8');
	m_data.push_back('\0');
	m_data.push_back(uint8_t(_value));
}
template<>
void zeus::Buffer::addParameter<uint8_t>(const uint8_t& _value) {
	int32_t currentOffset = m_data.size();
	m_paramOffset.push_back(currentOffset);
	m_data.push_back('u');
	m_data.push_back('i');
	m_data.push_back('n');
	m_data.push_back('t');
	m_data.push_back('8');
	m_data.push_back('\0');
	m_data.push_back(_value);
}
template<>
void zeus::Buffer::addParameter<int16_t>(const int16_t& _value) {
	int32_t currentOffset = m_data.size();
	m_paramOffset.push_back(currentOffset);
	m_data.push_back('i');
	m_data.push_back('n');
	m_data.push_back('t');
	m_data.push_back('1');
	m_data.push_back('6');
	m_data.push_back('\0');
	currentOffset = m_data.size();
	m_data.resize(m_data.size()+2);
	memcpy(&m_data[currentOffset], &_value, 2);
}
template<>
void zeus::Buffer::addParameter<uint16_t>(const uint16_t& _value) {
	int32_t currentOffset = m_data.size();
	m_paramOffset.push_back(currentOffset);
	m_data.push_back('u');
	m_data.push_back('i');
	m_data.push_back('n');
	m_data.push_back('t');
	m_data.push_back('1');
	m_data.push_back('6');
	m_data.push_back('\0');
	currentOffset = m_data.size();
	m_data.resize(m_data.size()+2);
	memcpy(&m_data[currentOffset], &_value, 2);
}
template<>
void zeus::Buffer::addParameter<int32_t>(const int32_t& _value) {
	int32_t currentOffset = m_data.size();
	m_paramOffset.push_back(currentOffset);
	m_data.push_back('i');
	m_data.push_back('n');
	m_data.push_back('t');
	m_data.push_back('3');
	m_data.push_back('2');
	m_data.push_back('\0');
	currentOffset = m_data.size();
	m_data.resize(m_data.size()+4);
	memcpy(&m_data[currentOffset], &_value, 4);
}
template<>
void zeus::Buffer::addParameter<uint32_t>(const uint32_t& _value) {
	int32_t currentOffset = m_data.size();
	m_paramOffset.push_back(currentOffset);
	m_data.push_back('u');
	m_data.push_back('i');
	m_data.push_back('n');
	m_data.push_back('t');
	m_data.push_back('3');
	m_data.push_back('2');
	m_data.push_back('\0');
	currentOffset = m_data.size();
	m_data.resize(m_data.size()+4);
	memcpy(&m_data[currentOffset], &_value, 4);
}
template<>
void zeus::Buffer::addParameter<int64_t>(const int64_t& _value) {
	int32_t currentOffset = m_data.size();
	m_paramOffset.push_back(currentOffset);
	m_data.push_back('i');
	m_data.push_back('n');
	m_data.push_back('t');
	m_data.push_back('3');
	m_data.push_back('2');
	m_data.push_back('\0');
	currentOffset = m_data.size();
	m_data.resize(m_data.size()+8);
	memcpy(&m_data[currentOffset], &_value, 8);
}
template<>
void zeus::Buffer::addParameter<uint64_t>(const uint64_t& _value) {
	int32_t currentOffset = m_data.size();
	m_paramOffset.push_back(currentOffset);
	m_data.push_back('u');
	m_data.push_back('i');
	m_data.push_back('n');
	m_data.push_back('t');
	m_data.push_back('6');
	m_data.push_back('4');
	m_data.push_back('\0');
	currentOffset = m_data.size();
	m_data.resize(m_data.size()+8);
	memcpy(&m_data[currentOffset], &_value, 8);
}
template<>
void zeus::Buffer::addParameter<float>(const float& _value) {
	int32_t currentOffset = m_data.size();
	m_paramOffset.push_back(currentOffset);
	m_data.push_back('f');
	m_data.push_back('l');
	m_data.push_back('o');
	m_data.push_back('a');
	m_data.push_back('t');
	m_data.push_back('\0');
	currentOffset = m_data.size();
	m_data.resize(m_data.size()+4);
	memcpy(&m_data[currentOffset], &_value, 4);
}
template<>
void zeus::Buffer::addParameter<double>(const double& _value) {
	int32_t currentOffset = m_data.size();
	m_paramOffset.push_back(currentOffset);
	m_data.push_back('d');
	m_data.push_back('o');
	m_data.push_back('u');
	m_data.push_back('b');
	m_data.push_back('l');
	m_data.push_back('e');
	m_data.push_back('\0');
	currentOffset = m_data.size();
	m_data.resize(m_data.size()+8);
	memcpy(&m_data[currentOffset], &_value, 8);
}
template<>
void zeus::Buffer::addParameter<bool>(const bool& _value) {
	int32_t currentOffset = m_data.size();
	m_paramOffset.push_back(currentOffset);
	m_data.push_back('b');
	m_data.push_back('o');
	m_data.push_back('o');
	m_data.push_back('l');
	m_data.push_back('\0');
	if (_value == true) {
		m_data.push_back('T');
	} else {
		m_data.push_back('F');
	}
}

template<>
void zeus::Buffer::addParameter<zeus::File>(const zeus::File& _value) {
	int32_t currentOffset = m_data.size();
	m_paramOffset.push_back(currentOffset);
	m_data.push_back('f');
	m_data.push_back('i');
	m_data.push_back('l');
	m_data.push_back('e');
	m_data.push_back('\0');
	ZEUS_TODO("Send file in output ...");
}

template<>
bool zeus::Buffer::internalGetParameter<bool>(int32_t _id) const {
	std::string type = internalGetParameterType(_id);
	const uint8_t* pointer = internalGetParameterPointer(_id);
	uint32_t dataSize = internalGetParameterSize(_id);
	if (createType<bool>() != type) {
		return 0;
	}
	const char* pointer2 = reinterpret_cast<const char*>(pointer);
	if (    *pointer2 == 'T'
	     || *pointer2 == '1'
	     || *pointer2 == 1) {
		return true;
	}
	return false;
}

template<>
std::string zeus::Buffer::internalGetParameter<std::string>(int32_t _id) const {
	std::string out;
	std::string type = internalGetParameterType(_id);
	const uint8_t* pointer = internalGetParameterPointer(_id);
	uint32_t dataSize = internalGetParameterSize(_id);
	out.resize(dataSize, 0);
	memcpy(&out[0], pointer, out.size());
	return out;
}


template<>
uint8_t zeus::Buffer::internalGetParameter<uint8_t>(int32_t _id) const {
	std::string type = internalGetParameterType(_id);
	const uint8_t* pointer = internalGetParameterPointer(_id);
	uint32_t dataSize = internalGetParameterSize(_id);
	// TODO : Check size ...
	if (createType<uint8_t>() == type) {
		const uint8_t* tmp = reinterpret_cast<const uint8_t*>(pointer);
		return *tmp;
	} else if (createType<uint16_t>() == type) {
		const uint16_t* tmp = reinterpret_cast<const uint16_t*>(pointer);
		return std::min(*tmp, uint16_t(UCHAR_MAX));
	} else if (createType<uint32_t>() == type) {
		const uint32_t* tmp = reinterpret_cast<const uint32_t*>(pointer);
		return std::min(*tmp, uint32_t(UCHAR_MAX));
	} else if (createType<uint64_t>() == type) {
		const uint64_t* tmp = reinterpret_cast<const uint64_t*>(pointer);
		return std::min(*tmp, uint64_t(UCHAR_MAX));
	} else if (createType<int8_t>() == type) {
		const int8_t* tmp = reinterpret_cast<const int8_t*>(pointer);
		return std::max(int8_t(0), *tmp);
	} else if (createType<int16_t>() == type) {
		const int16_t* tmp = reinterpret_cast<const int16_t*>(pointer);
		return etk::avg(int16_t(0), *tmp, int16_t(UCHAR_MAX));
	} else if (createType<int32_t>() == type) {
		const int32_t* tmp = reinterpret_cast<const int32_t*>(pointer);
		return etk::avg(int32_t(0), *tmp, int32_t(UCHAR_MAX));
	} else if (createType<int64_t>() == type) {
		const int64_t* tmp = reinterpret_cast<const int64_t*>(pointer);
		return etk::avg(int64_t(0), *tmp, int64_t(UCHAR_MAX));
	} else if (createType<float>() == type) {
		const float* tmp = reinterpret_cast<const float*>(pointer);
		return uint8_t(etk::avg(float(0), *tmp, float(UCHAR_MAX)));
	} else if (createType<double>() == type) {
		const double* tmp = reinterpret_cast<const double*>(pointer);
		return uint8_t(etk::avg(double(0), *tmp, double(UCHAR_MAX)));
	}
	ZEUS_ERROR("Can not get type from '" << type << "'");
	return 0;
}
template<>
uint16_t zeus::Buffer::internalGetParameter<uint16_t>(int32_t _id) const {
	std::string type = internalGetParameterType(_id);
	const uint8_t* pointer = internalGetParameterPointer(_id);
	uint32_t dataSize = internalGetParameterSize(_id);
	// TODO : Check size ...
	if (createType<uint8_t>() == type) {
		const uint8_t* tmp = reinterpret_cast<const uint8_t*>(pointer);
		return *tmp;
	} else if (createType<uint16_t>() == type) {
		const uint16_t* tmp = reinterpret_cast<const uint16_t*>(pointer);
		return *tmp;
	} else if (createType<uint32_t>() == type) {
		const uint32_t* tmp = reinterpret_cast<const uint32_t*>(pointer);
		return std::min(*tmp, uint32_t(USHRT_MAX));
	} else if (createType<uint64_t>() == type) {
		const uint64_t* tmp = reinterpret_cast<const uint64_t*>(pointer);
		return std::min(*tmp, uint64_t(USHRT_MAX));
	} else if (createType<int8_t>() == type) {
		const int8_t* tmp = reinterpret_cast<const int8_t*>(pointer);
		return std::max(int8_t(0), *tmp);
	} else if (createType<int16_t>() == type) {
		const int16_t* tmp = reinterpret_cast<const int16_t*>(pointer);
		return std::max(int16_t(0), *tmp);
	} else if (createType<int32_t>() == type) {
		const int32_t* tmp = reinterpret_cast<const int32_t*>(pointer);
		return etk::avg(int32_t(0), *tmp, int32_t(USHRT_MAX));
	} else if (createType<int64_t>() == type) {
		const int64_t* tmp = reinterpret_cast<const int64_t*>(pointer);
		return etk::avg(int64_t(0), *tmp, int64_t(USHRT_MAX));
	} else if (createType<float>() == type) {
		const float* tmp = reinterpret_cast<const float*>(pointer);
		return uint16_t(etk::avg(float(0), *tmp, float(USHRT_MAX)));
	} else if (createType<double>() == type) {
		const double* tmp = reinterpret_cast<const double*>(pointer);
		return uint16_t(etk::avg(double(0), *tmp, double(USHRT_MAX)));
	}
	ZEUS_ERROR("Can not get type from '" << type << "'");
	return 0;
}

template<>
uint32_t zeus::Buffer::internalGetParameter<uint32_t>(int32_t _id) const {
	std::string type = internalGetParameterType(_id);
	const uint8_t* pointer = internalGetParameterPointer(_id);
	uint32_t dataSize = internalGetParameterSize(_id);
	// TODO : Check size ...
	if (createType<uint8_t>() == type) {
		const uint8_t* tmp = reinterpret_cast<const uint8_t*>(pointer);
		return *tmp;
	} else if (createType<uint16_t>() == type) {
		const uint16_t* tmp = reinterpret_cast<const uint16_t*>(pointer);
		return *tmp;
	} else if (createType<uint32_t>() == type) {
		const uint32_t* tmp = reinterpret_cast<const uint32_t*>(pointer);
		return *tmp;
	} else if (createType<uint64_t>() == type) {
		const uint64_t* tmp = reinterpret_cast<const uint64_t*>(pointer);
		return std::min(*tmp, uint64_t(ULONG_MAX));
	} else if (createType<int8_t>() == type) {
		const int8_t* tmp = reinterpret_cast<const int8_t*>(pointer);
		return std::max(int8_t(0), *tmp);
	} else if (createType<int16_t>() == type) {
		const int16_t* tmp = reinterpret_cast<const int16_t*>(pointer);
		return std::max(int16_t(0), *tmp);
	} else if (createType<int32_t>() == type) {
		const int32_t* tmp = reinterpret_cast<const int32_t*>(pointer);
		return std::max(int32_t(0), *tmp);
	} else if (createType<int64_t>() == type) {
		const int64_t* tmp = reinterpret_cast<const int64_t*>(pointer);
		return etk::avg(int64_t(0), *tmp, int64_t(ULONG_MAX));
	} else if (createType<float>() == type) {
		const float* tmp = reinterpret_cast<const float*>(pointer);
		return uint32_t(etk::avg(float(0), *tmp, float(ULONG_MAX)));
	} else if (createType<double>() == type) {
		const double* tmp = reinterpret_cast<const double*>(pointer);
		return uint32_t(etk::avg(double(0), *tmp, double(ULONG_MAX)));
	}
	ZEUS_ERROR("Can not get type from '" << type << "'");
	return 0;
}

template<>
uint64_t zeus::Buffer::internalGetParameter<uint64_t>(int32_t _id) const {
	std::string type = internalGetParameterType(_id);
	const uint8_t* pointer = internalGetParameterPointer(_id);
	uint32_t dataSize = internalGetParameterSize(_id);
	// TODO : Check size ...
	if (createType<uint8_t>() == type) {
		const uint8_t* tmp = reinterpret_cast<const uint8_t*>(pointer);
		return *tmp;
	} else if (createType<uint16_t>() == type) {
		const uint16_t* tmp = reinterpret_cast<const uint16_t*>(pointer);
		return *tmp;
	} else if (createType<uint32_t>() == type) {
		const uint32_t* tmp = reinterpret_cast<const uint32_t*>(pointer);
		return *tmp;
	} else if (createType<uint64_t>() == type) {
		const uint64_t* tmp = reinterpret_cast<const uint64_t*>(pointer);
		return *tmp;
	} else if (createType<int8_t>() == type) {
		const int8_t* tmp = reinterpret_cast<const int8_t*>(pointer);
		return std::max(int8_t(0), *tmp);
	} else if (createType<int16_t>() == type) {
		const int16_t* tmp = reinterpret_cast<const int16_t*>(pointer);
		return std::max(int16_t(0), *tmp);
	} else if (createType<int32_t>() == type) {
		const int32_t* tmp = reinterpret_cast<const int32_t*>(pointer);
		return std::max(int32_t(0), *tmp);
	} else if (createType<int64_t>() == type) {
		const int64_t* tmp = reinterpret_cast<const int64_t*>(pointer);
		return std::max(int64_t(0), *tmp);
	} else if (createType<float>() == type) {
		const float* tmp = reinterpret_cast<const float*>(pointer);
		return uint64_t(etk::avg(float(0), *tmp, float(ULONG_MAX)));
	} else if (createType<double>() == type) {
		const double* tmp = reinterpret_cast<const double*>(pointer);
		return uint64_t(etk::avg(double(0), *tmp, double(ULONG_MAX)));
	}
	ZEUS_ERROR("Can not get type from '" << type << "'");
	return 0;
}

template<>
int8_t zeus::Buffer::internalGetParameter<int8_t>(int32_t _id) const {
	std::string type = internalGetParameterType(_id);
	const uint8_t* pointer = internalGetParameterPointer(_id);
	uint32_t dataSize = internalGetParameterSize(_id);
	// TODO : Check size ...
	if (createType<uint8_t>() == type) {
		const uint8_t* tmp = reinterpret_cast<const uint8_t*>(pointer);
		return std::min(*tmp, uint8_t(SCHAR_MAX));
	} else if (createType<uint16_t>() == type) {
		const uint16_t* tmp = reinterpret_cast<const uint16_t*>(pointer);
		return std::min(*tmp, uint16_t(SCHAR_MAX));
	} else if (createType<uint32_t>() == type) {
		const uint32_t* tmp = reinterpret_cast<const uint32_t*>(pointer);
		return std::min(*tmp, uint32_t(SCHAR_MAX));
	} else if (createType<uint64_t>() == type) {
		const uint64_t* tmp = reinterpret_cast<const uint64_t*>(pointer);
		return std::min(*tmp, uint64_t(SCHAR_MAX));
	} else if (createType<int8_t>() == type) {
		const int8_t* tmp = reinterpret_cast<const int8_t*>(pointer);
		return *tmp;
	} else if (createType<int16_t>() == type) {
		const int16_t* tmp = reinterpret_cast<const int16_t*>(pointer);
		return etk::avg(int16_t(SCHAR_MIN), *tmp, int16_t(SCHAR_MAX));
	} else if (createType<int32_t>() == type) {
		const int32_t* tmp = reinterpret_cast<const int32_t*>(pointer);
		return etk::avg(int32_t(SCHAR_MIN), *tmp, int32_t(SCHAR_MAX));
	} else if (createType<int64_t>() == type) {
		const int64_t* tmp = reinterpret_cast<const int64_t*>(pointer);
		return etk::avg(int64_t(SCHAR_MIN), *tmp, int64_t(SCHAR_MAX));
	} else if (createType<float>() == type) {
		const float* tmp = reinterpret_cast<const float*>(pointer);
		return int8_t(etk::avg(float(SCHAR_MIN), *tmp, float(SCHAR_MAX)));
	} else if (createType<double>() == type) {
		const double* tmp = reinterpret_cast<const double*>(pointer);
		return int8_t(etk::avg(double(SCHAR_MIN), *tmp, double(SCHAR_MAX)));
	}
	ZEUS_ERROR("Can not get type from '" << type << "'");
	return 0;
}

template<>
int16_t zeus::Buffer::internalGetParameter<int16_t>(int32_t _id) const {
	std::string type = internalGetParameterType(_id);
	const uint8_t* pointer = internalGetParameterPointer(_id);
	uint32_t dataSize = internalGetParameterSize(_id);
	// TODO : Check size ...
	if (createType<uint8_t>() == type) {
		const uint8_t* tmp = reinterpret_cast<const uint8_t*>(pointer);
		return *tmp;
	} else if (createType<uint16_t>() == type) {
		const uint16_t* tmp = reinterpret_cast<const uint16_t*>(pointer);
		return std::min(*tmp, uint16_t(SHRT_MAX));
	} else if (createType<uint32_t>() == type) {
		const uint32_t* tmp = reinterpret_cast<const uint32_t*>(pointer);
		return std::min(*tmp, uint32_t(SHRT_MAX));
	} else if (createType<uint64_t>() == type) {
		const uint64_t* tmp = reinterpret_cast<const uint64_t*>(pointer);
		return std::min(*tmp, uint64_t(SHRT_MAX));
	} else if (createType<int8_t>() == type) {
		const int8_t* tmp = reinterpret_cast<const int8_t*>(pointer);
		return *tmp;
	} else if (createType<int16_t>() == type) {
		const int16_t* tmp = reinterpret_cast<const int16_t*>(pointer);
		return *tmp;
	} else if (createType<int32_t>() == type) {
		const int32_t* tmp = reinterpret_cast<const int32_t*>(pointer);
		return etk::avg(int32_t(SHRT_MIN), *tmp, int32_t(SHRT_MAX));
	} else if (createType<int64_t>() == type) {
		const int64_t* tmp = reinterpret_cast<const int64_t*>(pointer);
		return etk::avg(int64_t(SHRT_MIN), *tmp, int64_t(SHRT_MAX));
	} else if (createType<float>() == type) {
		const float* tmp = reinterpret_cast<const float*>(pointer);
		return int16_t(etk::avg(float(SHRT_MIN), *tmp, float(SHRT_MAX)));
	} else if (createType<double>() == type) {
		const double* tmp = reinterpret_cast<const double*>(pointer);
		return int16_t(etk::avg(double(SHRT_MIN), *tmp, double(SHRT_MAX)));
	}
	ZEUS_ERROR("Can not get type from '" << type << "'");
	return 0;
}

template<>
int32_t zeus::Buffer::internalGetParameter<int32_t>(int32_t _id) const {
	std::string type = internalGetParameterType(_id);
	const uint8_t* pointer = internalGetParameterPointer(_id);
	uint32_t dataSize = internalGetParameterSize(_id);
	// TODO : Check size ...
	if (createType<uint8_t>() == type) {
		const uint8_t* tmp = reinterpret_cast<const uint8_t*>(pointer);
		return *tmp;
	} else if (createType<uint16_t>() == type) {
		const uint16_t* tmp = reinterpret_cast<const uint16_t*>(pointer);
		return *tmp;
	} else if (createType<uint32_t>() == type) {
		const uint32_t* tmp = reinterpret_cast<const uint32_t*>(pointer);
		return std::min(*tmp, uint32_t(LONG_MAX));
	} else if (createType<uint64_t>() == type) {
		const uint64_t* tmp = reinterpret_cast<const uint64_t*>(pointer);
		return std::min(*tmp, uint64_t(LONG_MAX));
	} else if (createType<int8_t>() == type) {
		const int8_t* tmp = reinterpret_cast<const int8_t*>(pointer);
		return *tmp;
	} else if (createType<int16_t>() == type) {
		const int16_t* tmp = reinterpret_cast<const int16_t*>(pointer);
		return *tmp;
	} else if (createType<int32_t>() == type) {
		const int32_t* tmp = reinterpret_cast<const int32_t*>(pointer);
		return *tmp;
	} else if (createType<int64_t>() == type) {
		const int64_t* tmp = reinterpret_cast<const int64_t*>(pointer);
		return etk::avg(int64_t(LONG_MIN), *tmp, int64_t(LONG_MAX));
	} else if (createType<float>() == type) {
		const float* tmp = reinterpret_cast<const float*>(pointer);
		return int32_t(etk::avg(float(LONG_MIN), *tmp, float(LONG_MAX)));
	} else if (createType<double>() == type) {
		const double* tmp = reinterpret_cast<const double*>(pointer);
		return int32_t(etk::avg(double(LONG_MIN), *tmp, double(LONG_MAX)));
	}
	ZEUS_ERROR("Can not get type from '" << type << "'");
	return 0;
}

template<>
int64_t zeus::Buffer::internalGetParameter<int64_t>(int32_t _id) const {
	std::string type = internalGetParameterType(_id);
	const uint8_t* pointer = internalGetParameterPointer(_id);
	uint32_t dataSize = internalGetParameterSize(_id);
	// TODO : Check size ...
	if (createType<uint8_t>() == type) {
		const uint8_t* tmp = reinterpret_cast<const uint8_t*>(pointer);
		return *tmp;
	} else if (createType<uint16_t>() == type) {
		const uint16_t* tmp = reinterpret_cast<const uint16_t*>(pointer);
		return *tmp;
	} else if (createType<uint32_t>() == type) {
		const uint32_t* tmp = reinterpret_cast<const uint32_t*>(pointer);
		return *tmp;
	} else if (createType<uint64_t>() == type) {
		const uint64_t* tmp = reinterpret_cast<const uint64_t*>(pointer);
		return std::min(*tmp, uint64_t(LLONG_MAX));
	} else if (createType<int8_t>() == type) {
		const int8_t* tmp = reinterpret_cast<const int8_t*>(pointer);
		return *tmp;
	} else if (createType<int16_t>() == type) {
		const int16_t* tmp = reinterpret_cast<const int16_t*>(pointer);
		return *tmp;
	} else if (createType<int32_t>() == type) {
		const int32_t* tmp = reinterpret_cast<const int32_t*>(pointer);
		return *tmp;
	} else if (createType<int64_t>() == type) {
		const int64_t* tmp = reinterpret_cast<const int64_t*>(pointer);
		return *tmp;
	} else if (createType<float>() == type) {
		const float* tmp = reinterpret_cast<const float*>(pointer);
		return int64_t(etk::avg(float(LLONG_MIN), *tmp, float(LLONG_MAX)));
	} else if (createType<double>() == type) {
		const double* tmp = reinterpret_cast<const double*>(pointer);
		return int64_t(etk::avg(double(LLONG_MIN), *tmp, double(LLONG_MAX)));
	}
	ZEUS_ERROR("Can not get type from '" << type << "'");
	return 0;
}

template<>
float zeus::Buffer::internalGetParameter<float>(int32_t _id) const {
	std::string type = internalGetParameterType(_id);
	const uint8_t* pointer = internalGetParameterPointer(_id);
	uint32_t dataSize = internalGetParameterSize(_id);
	// TODO : Check size ...
	if (createType<uint8_t>() == type) {
		const uint8_t* tmp = reinterpret_cast<const uint8_t*>(pointer);
		return *tmp;
	} else if (createType<uint16_t>() == type) {
		const uint16_t* tmp = reinterpret_cast<const uint16_t*>(pointer);
		return *tmp;
	} else if (createType<uint32_t>() == type) {
		const uint32_t* tmp = reinterpret_cast<const uint32_t*>(pointer);
		return *tmp;
	} else if (createType<uint64_t>() == type) {
		const uint64_t* tmp = reinterpret_cast<const uint64_t*>(pointer);
		return *tmp;
	} else if (createType<int8_t>() == type) {
		const int8_t* tmp = reinterpret_cast<const int8_t*>(pointer);
		return *tmp;
	} else if (createType<int16_t>() == type) {
		const int16_t* tmp = reinterpret_cast<const int16_t*>(pointer);
		return *tmp;
	} else if (createType<int32_t>() == type) {
		const int32_t* tmp = reinterpret_cast<const int32_t*>(pointer);
		return *tmp;
	} else if (createType<int64_t>() == type) {
		const int64_t* tmp = reinterpret_cast<const int64_t*>(pointer);
		return *tmp;
	} else if (createType<float>() == type) {
		const float* tmp = reinterpret_cast<const float*>(pointer);
		return *tmp;
	} else if (createType<double>() == type) {
		const double* tmp = reinterpret_cast<const double*>(pointer);
		return *tmp;
	}
	ZEUS_ERROR("Can not get type from '" << type << "'");
	return 0.0f;
}
template<>
double zeus::Buffer::internalGetParameter<double>(int32_t _id) const {
	std::string type = internalGetParameterType(_id);
	const uint8_t* pointer = internalGetParameterPointer(_id);
	uint32_t dataSize = internalGetParameterSize(_id);
	// TODO : Check size ...
	if (createType<uint8_t>() == type) {
		const uint8_t* tmp = reinterpret_cast<const uint8_t*>(pointer);
		return *tmp;
	} else if (createType<uint16_t>() == type) {
		const uint16_t* tmp = reinterpret_cast<const uint16_t*>(pointer);
		return *tmp;
	} else if (createType<uint32_t>() == type) {
		const uint32_t* tmp = reinterpret_cast<const uint32_t*>(pointer);
		return *tmp;
	} else if (createType<uint64_t>() == type) {
		const uint64_t* tmp = reinterpret_cast<const uint64_t*>(pointer);
		return *tmp;
	} else if (createType<int8_t>() == type) {
		const int8_t* tmp = reinterpret_cast<const int8_t*>(pointer);
		return *tmp;
	} else if (createType<int16_t>() == type) {
		const int16_t* tmp = reinterpret_cast<const int16_t*>(pointer);
		return *tmp;
	} else if (createType<int32_t>() == type) {
		const int32_t* tmp = reinterpret_cast<const int32_t*>(pointer);
		return *tmp;
	} else if (createType<int64_t>() == type) {
		const int64_t* tmp = reinterpret_cast<const int64_t*>(pointer);
		return *tmp;
	} else if (createType<float>() == type) {
		const float* tmp = reinterpret_cast<const float*>(pointer);
		return *tmp;
	} else if (createType<double>() == type) {
		const double* tmp = reinterpret_cast<const double*>(pointer);
		return *tmp;
	}
	ZEUS_ERROR("Can not get type from '" << type << "'");
	return 0.0;
}
























template<>
std::vector<uint8_t> zeus::Buffer::internalGetParameter<std::vector<uint8_t>>(int32_t _id) const {
	std::vector<uint8_t> out;
	std::string type = internalGetParameterType(_id);
	const uint8_t* pointer = internalGetParameterPointer(_id);
	uint32_t dataSize = internalGetParameterSize(_id);
	// TODO : Check size ...
	if (type == "vector:empty") {
		return out;
	} else if (createType<std::vector<uint8_t>>() == type) {
		int32_t nbElement = dataSize / sizeof(uint8_t);
		out.resize(nbElement);
		memcpy(&out, pointer, nbElement * sizeof(uint8_t));
		return out;
	} else if (createType<std::vector<uint16_t>>() == type) {
		const uint16_t* tmp = reinterpret_cast<const uint16_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint16_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = std::min(tmp[iii], uint16_t(UCHAR_MAX));
		}
		return out;
	} else if (createType<std::vector<uint32_t>>() == type) {
		const uint32_t* tmp = reinterpret_cast<const uint32_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint32_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = std::min(tmp[iii], uint32_t(UCHAR_MAX));
		}
		return out;
	} else if (createType<std::vector<uint64_t>>() == type) {
		const uint64_t* tmp = reinterpret_cast<const uint64_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint64_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = std::min(tmp[iii], uint64_t(UCHAR_MAX));
		}
		return out;
	} else if (createType<std::vector<int8_t>>() == type) {
		const int8_t* tmp = reinterpret_cast<const int8_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int8_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = std::max(int8_t(0), tmp[iii]);
		}
		return out;
	} else if (createType<std::vector<int16_t>>() == type) {
		const int16_t* tmp = reinterpret_cast<const int16_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int16_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = etk::avg(int16_t(0), tmp[iii], int16_t(UCHAR_MAX));
		}
		return out;
	} else if (createType<std::vector<int32_t>>() == type) {
		const int32_t* tmp = reinterpret_cast<const int32_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int32_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = etk::avg(int32_t(0), tmp[iii], int32_t(UCHAR_MAX));
		}
		return out;
	} else if (createType<std::vector<int64_t>>() == type) {
		const int64_t* tmp = reinterpret_cast<const int64_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int64_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = etk::avg(int64_t(0), tmp[iii], int64_t(UCHAR_MAX));
		}
		return out;
	} else if (createType<std::vector<float>>() == type) {
		const float* tmp = reinterpret_cast<const float*>(pointer);
		int32_t nbElement = dataSize / sizeof(float);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = uint8_t(etk::avg(float(0), tmp[iii], float(UCHAR_MAX)));
		}
		return out;
	} else if (createType<std::vector<double>>() == type) {
		const double* tmp = reinterpret_cast<const double*>(pointer);
		int32_t nbElement = dataSize / sizeof(double);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = uint8_t(etk::avg(double(0), tmp[iii], double(UCHAR_MAX)));
		}
		return out;
	}
	ZEUS_ERROR("Can not get type from '" << type << "'");
	return out;
}
template<>
std::vector<uint16_t> zeus::Buffer::internalGetParameter<std::vector<uint16_t>>(int32_t _id) const {
	std::vector<uint16_t> out;
	std::string type = internalGetParameterType(_id);
	const uint8_t* pointer = internalGetParameterPointer(_id);
	uint32_t dataSize = internalGetParameterSize(_id);
	// TODO : Check size ...
	if (type == "vector:empty") {
		return out;
	} else if (createType<std::vector<uint8_t>>() == type) {
		const uint8_t* tmp = reinterpret_cast<const uint8_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint8_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<uint16_t>>() == type) {
		int32_t nbElement = dataSize / sizeof(uint16_t);
		out.resize(nbElement);
		memcpy(&out, pointer, nbElement * sizeof(uint16_t));
		return out;
	} else if (createType<std::vector<uint32_t>>() == type) {
		const uint32_t* tmp = reinterpret_cast<const uint32_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint32_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = std::min(tmp[iii], uint32_t(USHRT_MAX));
		}
		return out;
	} else if (createType<std::vector<uint64_t>>() == type) {
		const uint64_t* tmp = reinterpret_cast<const uint64_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint64_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = std::min(tmp[iii], uint64_t(USHRT_MAX));
		}
		return out;
	} else if (createType<std::vector<int8_t>>() == type) {
		const int8_t* tmp = reinterpret_cast<const int8_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int8_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = std::max(int8_t(0), tmp[iii]);
		}
		return out;
	} else if (createType<std::vector<int16_t>>() == type) {
		const int16_t* tmp = reinterpret_cast<const int16_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int16_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = std::max(int16_t(0), tmp[iii]);
		}
		return out;
	} else if (createType<std::vector<int32_t>>() == type) {
		const int32_t* tmp = reinterpret_cast<const int32_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int32_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = etk::avg(int32_t(0), tmp[iii], int32_t(USHRT_MAX));
		}
		return out;
	} else if (createType<std::vector<int64_t>>() == type) {
		const int64_t* tmp = reinterpret_cast<const int64_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int64_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = etk::avg(int64_t(0), tmp[iii], int64_t(USHRT_MAX));
		}
		return out;
	} else if (createType<std::vector<float>>() == type) {
		const float* tmp = reinterpret_cast<const float*>(pointer);
		int32_t nbElement = dataSize / sizeof(float);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = uint16_t(etk::avg(float(0), tmp[iii], float(USHRT_MAX)));
		}
		return out;
	} else if (createType<std::vector<double>>() == type) {
		const double* tmp = reinterpret_cast<const double*>(pointer);
		int32_t nbElement = dataSize / sizeof(double);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = uint16_t(etk::avg(double(0), tmp[iii], double(USHRT_MAX)));
		}
		return out;
	}
	ZEUS_ERROR("Can not get type from '" << type << "'");
	return out;
}

template<>
std::vector<uint32_t> zeus::Buffer::internalGetParameter<std::vector<uint32_t>>(int32_t _id) const {
	std::vector<uint32_t> out;
	std::string type = internalGetParameterType(_id);
	const uint8_t* pointer = internalGetParameterPointer(_id);
	uint32_t dataSize = internalGetParameterSize(_id);
	// TODO : Check size ...
	if (type == "vector:empty") {
		return out;
	} else if (createType<std::vector<uint8_t>>() == type) {
		const uint8_t* tmp = reinterpret_cast<const uint8_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint8_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<uint16_t>>() == type) {
		const uint16_t* tmp = reinterpret_cast<const uint16_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint16_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<uint32_t>>() == type) {
		int32_t nbElement = dataSize / sizeof(uint32_t);
		out.resize(nbElement);
		memcpy(&out, pointer, nbElement * sizeof(uint32_t));
		return out;
	} else if (createType<std::vector<uint64_t>>() == type) {
		const uint64_t* tmp = reinterpret_cast<const uint64_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint64_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = std::min(tmp[iii], uint64_t(ULONG_MAX));
		}
		return out;
	} else if (createType<std::vector<int8_t>>() == type) {
		const int8_t* tmp = reinterpret_cast<const int8_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int8_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = std::max(int8_t(0), tmp[iii]);
		}
		return out;
	} else if (createType<std::vector<int16_t>>() == type) {
		const int16_t* tmp = reinterpret_cast<const int16_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int16_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = std::max(int16_t(0), tmp[iii]);
		}
		return out;
	} else if (createType<std::vector<int32_t>>() == type) {
		const int32_t* tmp = reinterpret_cast<const int32_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int32_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = std::max(int32_t(0), tmp[iii]);
		}
		return out;
	} else if (createType<std::vector<int64_t>>() == type) {
		const int64_t* tmp = reinterpret_cast<const int64_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int64_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = etk::avg(int64_t(0), tmp[iii], int64_t(ULONG_MAX));
		}
		return out;
	} else if (createType<std::vector<float>>() == type) {
		const float* tmp = reinterpret_cast<const float*>(pointer);
		int32_t nbElement = dataSize / sizeof(float);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = uint32_t(etk::avg(float(0), tmp[iii], float(ULONG_MAX)));
		}
		return out;
	} else if (createType<std::vector<double>>() == type) {
		const double* tmp = reinterpret_cast<const double*>(pointer);
		int32_t nbElement = dataSize / sizeof(double);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = uint32_t(etk::avg(double(0), tmp[iii], double(ULONG_MAX)));
		}
		return out;
	}
	ZEUS_ERROR("Can not get type from '" << type << "'");
	return out;
}

template<>
std::vector<uint64_t> zeus::Buffer::internalGetParameter<std::vector<uint64_t>>(int32_t _id) const {
	std::vector<uint64_t> out;
	std::string type = internalGetParameterType(_id);
	const uint8_t* pointer = internalGetParameterPointer(_id);
	uint32_t dataSize = internalGetParameterSize(_id);
	// TODO : Check size ...
	if (type == "vector:empty") {
		return out;
	} else if (createType<std::vector<uint8_t>>() == type) {
		const uint8_t* tmp = reinterpret_cast<const uint8_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint8_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<uint16_t>>() == type) {
		const uint16_t* tmp = reinterpret_cast<const uint16_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint16_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<uint32_t>>() == type) {
		const uint32_t* tmp = reinterpret_cast<const uint32_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint32_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<uint64_t>>() == type) {
		int32_t nbElement = dataSize / sizeof(uint64_t);
		out.resize(nbElement);
		memcpy(&out, pointer, nbElement * sizeof(uint64_t));
		return out;
	} else if (createType<std::vector<int8_t>>() == type) {
		const int8_t* tmp = reinterpret_cast<const int8_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int8_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = std::max(int8_t(0), tmp[iii]);
		}
		return out;
	} else if (createType<std::vector<int16_t>>() == type) {
		const int16_t* tmp = reinterpret_cast<const int16_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int16_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = std::max(int16_t(0), tmp[iii]);
		}
		return out;
	} else if (createType<std::vector<int32_t>>() == type) {
		const int32_t* tmp = reinterpret_cast<const int32_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int32_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = std::max(int32_t(0), tmp[iii]);
		}
		return out;
	} else if (createType<std::vector<int64_t>>() == type) {
		const int64_t* tmp = reinterpret_cast<const int64_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int64_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = std::max(int64_t(0), tmp[iii]);
		}
		return out;
	} else if (createType<std::vector<float>>() == type) {
		const float* tmp = reinterpret_cast<const float*>(pointer);
		int32_t nbElement = dataSize / sizeof(float);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = uint64_t(etk::avg(float(0), tmp[iii], float(ULONG_MAX)));
		}
		return out;
	} else if (createType<std::vector<double>>() == type) {
		const double* tmp = reinterpret_cast<const double*>(pointer);
		int32_t nbElement = dataSize / sizeof(double);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = uint64_t(etk::avg(double(0), tmp[iii], double(ULONG_MAX)));
		}
		return out;
	}
	ZEUS_ERROR("Can not get type from '" << type << "'");
	return out;
}

template<>
std::vector<int8_t> zeus::Buffer::internalGetParameter<std::vector<int8_t>>(int32_t _id) const {
	std::vector<int8_t> out;
	std::string type = internalGetParameterType(_id);
	const uint8_t* pointer = internalGetParameterPointer(_id);
	uint32_t dataSize = internalGetParameterSize(_id);
	// TODO : Check size ...
	if (type == "vector:empty") {
		return out;
	} else if (createType<std::vector<uint8_t>>() == type) {
		const uint8_t* tmp = reinterpret_cast<const uint8_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint8_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = std::min(tmp[iii], uint8_t(SCHAR_MAX));
		}
		return out;
	} else if (createType<std::vector<uint16_t>>() == type) {
		const uint16_t* tmp = reinterpret_cast<const uint16_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint16_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = std::min(tmp[iii], uint16_t(SCHAR_MAX));
		}
		return out;
	} else if (createType<std::vector<uint32_t>>() == type) {
		const uint32_t* tmp = reinterpret_cast<const uint32_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint32_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = std::min(tmp[iii], uint32_t(SCHAR_MAX));
		}
		return out;
	} else if (createType<std::vector<uint64_t>>() == type) {
		const uint64_t* tmp = reinterpret_cast<const uint64_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint64_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = std::min(tmp[iii], uint64_t(SCHAR_MAX));
		}
		return out;
	} else if (createType<std::vector<int8_t>>() == type) {
		int32_t nbElement = dataSize / sizeof(int8_t);
		out.resize(nbElement);
		memcpy(&out, pointer, nbElement * sizeof(int8_t));
		return out;
	} else if (createType<std::vector<int16_t>>() == type) {
		const int16_t* tmp = reinterpret_cast<const int16_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int16_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = etk::avg(int16_t(SCHAR_MIN), tmp[iii], int16_t(SCHAR_MAX));
		}
		return out;
	} else if (createType<std::vector<int32_t>>() == type) {
		const int32_t* tmp = reinterpret_cast<const int32_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int32_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = etk::avg(int32_t(SCHAR_MIN), tmp[iii], int32_t(SCHAR_MAX));
		}
		return out;
	} else if (createType<std::vector<int64_t>>() == type) {
		const int64_t* tmp = reinterpret_cast<const int64_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int64_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = etk::avg(int64_t(SCHAR_MIN), tmp[iii], int64_t(SCHAR_MAX));
		}
		return out;
	} else if (createType<std::vector<float>>() == type) {
		const float* tmp = reinterpret_cast<const float*>(pointer);
		int32_t nbElement = dataSize / sizeof(float);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = int8_t(etk::avg(float(SCHAR_MIN), tmp[iii], float(SCHAR_MAX)));
		}
		return out;
	} else if (createType<std::vector<double>>() == type) {
		const double* tmp = reinterpret_cast<const double*>(pointer);
		int32_t nbElement = dataSize / sizeof(double);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = int8_t(etk::avg(double(SCHAR_MIN), tmp[iii], double(SCHAR_MAX)));
		}
		return out;
	}
	ZEUS_ERROR("Can not get type from '" << type << "'");
	return out;
}

template<>
std::vector<int16_t> zeus::Buffer::internalGetParameter<std::vector<int16_t>>(int32_t _id) const {
	std::vector<int16_t> out;
	std::string type = internalGetParameterType(_id);
	const uint8_t* pointer = internalGetParameterPointer(_id);
	uint32_t dataSize = internalGetParameterSize(_id);
	// TODO : Check size ...
	if (type == "vector:empty") {
		return out;
	} else if (createType<std::vector<uint8_t>>() == type) {
		const uint8_t* tmp = reinterpret_cast<const uint8_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint8_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<uint16_t>>() == type) {
		const uint16_t* tmp = reinterpret_cast<const uint16_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint16_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = std::min(tmp[iii], uint16_t(SHRT_MAX));
		}
		return out;
	} else if (createType<std::vector<uint32_t>>() == type) {
		const uint32_t* tmp = reinterpret_cast<const uint32_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint32_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = std::min(tmp[iii], uint32_t(SHRT_MAX));
		}
		return out;
	} else if (createType<std::vector<uint64_t>>() == type) {
		const uint64_t* tmp = reinterpret_cast<const uint64_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint64_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = std::min(tmp[iii], uint64_t(SHRT_MAX));
		}
		return out;
	} else if (createType<std::vector<int8_t>>() == type) {
		const int8_t* tmp = reinterpret_cast<const int8_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int8_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<int16_t>>() == type) {
		int32_t nbElement = dataSize / sizeof(int16_t);
		out.resize(nbElement);
		memcpy(&out, pointer, nbElement * sizeof(int16_t));
		return out;
	} else if (createType<std::vector<int32_t>>() == type) {
		const int32_t* tmp = reinterpret_cast<const int32_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int32_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = etk::avg(int32_t(SHRT_MIN), tmp[iii], int32_t(SHRT_MAX));
		}
		return out;
	} else if (createType<std::vector<int64_t>>() == type) {
		const int64_t* tmp = reinterpret_cast<const int64_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int64_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = etk::avg(int64_t(SHRT_MIN), tmp[iii], int64_t(SHRT_MAX));
		}
		return out;
	} else if (createType<std::vector<float>>() == type) {
		const float* tmp = reinterpret_cast<const float*>(pointer);
		int32_t nbElement = dataSize / sizeof(float);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = int16_t(etk::avg(float(SHRT_MIN), tmp[iii], float(SHRT_MAX)));
		}
		return out;
	} else if (createType<std::vector<double>>() == type) {
		const double* tmp = reinterpret_cast<const double*>(pointer);
		int32_t nbElement = dataSize / sizeof(double);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = int16_t(etk::avg(double(SHRT_MIN), tmp[iii], double(SHRT_MAX)));
		}
		return out;
	}
	ZEUS_ERROR("Can not get type from '" << type << "'");
	return out;
}

template<>
std::vector<int32_t> zeus::Buffer::internalGetParameter<std::vector<int32_t>>(int32_t _id) const {
	std::vector<int32_t> out;
	std::string type = internalGetParameterType(_id);
	const uint8_t* pointer = internalGetParameterPointer(_id);
	uint32_t dataSize = internalGetParameterSize(_id);
	// TODO : Check size ...
	if (type == "vector:empty") {
		return out;
	} else if (createType<std::vector<uint8_t>>() == type) {
		const uint8_t* tmp = reinterpret_cast<const uint8_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint8_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<uint16_t>>() == type) {
		const uint16_t* tmp = reinterpret_cast<const uint16_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint16_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<uint32_t>>() == type) {
		const uint32_t* tmp = reinterpret_cast<const uint32_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint32_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = std::min(tmp[iii], uint32_t(LONG_MAX));
		}
		return out;
	} else if (createType<std::vector<uint64_t>>() == type) {
		const uint64_t* tmp = reinterpret_cast<const uint64_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint64_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = std::min(tmp[iii], uint64_t(LONG_MAX));
		}
		return out;
	} else if (createType<std::vector<int8_t>>() == type) {
		const int8_t* tmp = reinterpret_cast<const int8_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int8_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<int16_t>>() == type) {
		const int16_t* tmp = reinterpret_cast<const int16_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int16_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<int32_t>>() == type) {
		int32_t nbElement = dataSize / sizeof(int32_t);
		out.resize(nbElement);
		memcpy(&out, pointer, nbElement * sizeof(int32_t));
		return out;
	} else if (createType<std::vector<int64_t>>() == type) {
		const int64_t* tmp = reinterpret_cast<const int64_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int64_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = etk::avg(int64_t(LONG_MIN), tmp[iii], int64_t(LONG_MAX));
		}
		return out;
	} else if (createType<std::vector<float>>() == type) {
		const float* tmp = reinterpret_cast<const float*>(pointer);
		int32_t nbElement = dataSize / sizeof(float);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = int32_t(etk::avg(float(LONG_MIN), tmp[iii], float(LONG_MAX)));
		}
		return out;
	} else if (createType<std::vector<double>>() == type) {
		const double* tmp = reinterpret_cast<const double*>(pointer);
		int32_t nbElement = dataSize / sizeof(double);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = int32_t(etk::avg(double(LONG_MIN), tmp[iii], double(LONG_MAX)));
		}
		return out;
	}
	ZEUS_ERROR("Can not get type from '" << type << "'");
	return out;
}

template<>
std::vector<int64_t> zeus::Buffer::internalGetParameter<std::vector<int64_t>>(int32_t _id) const {
	std::vector<int64_t> out;
	std::string type = internalGetParameterType(_id);
	const uint8_t* pointer = internalGetParameterPointer(_id);
	uint32_t dataSize = internalGetParameterSize(_id);
	// TODO : Check size ...
	if (type == "vector:empty") {
		return out;
	} else if (createType<std::vector<uint8_t>>() == type) {
		const uint8_t* tmp = reinterpret_cast<const uint8_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint8_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<uint16_t>>() == type) {
		const uint16_t* tmp = reinterpret_cast<const uint16_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint16_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<uint32_t>>() == type) {
		const uint32_t* tmp = reinterpret_cast<const uint32_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint32_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<uint64_t>>() == type) {
		const uint64_t* tmp = reinterpret_cast<const uint64_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint64_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = std::min(tmp[iii], uint64_t(LLONG_MAX));
		}
		return out;
	} else if (createType<std::vector<int8_t>>() == type) {
		const int8_t* tmp = reinterpret_cast<const int8_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int8_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<int16_t>>() == type) {
		const int16_t* tmp = reinterpret_cast<const int16_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int16_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<int32_t>>() == type) {
		const int32_t* tmp = reinterpret_cast<const int32_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int32_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<int64_t>>() == type) {
		int32_t nbElement = dataSize / sizeof(int64_t);
		out.resize(nbElement);
		memcpy(&out, pointer, nbElement * sizeof(int64_t));
		return out;
	} else if (createType<std::vector<float>>() == type) {
		const float* tmp = reinterpret_cast<const float*>(pointer);
		int32_t nbElement = dataSize / sizeof(float);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = int64_t(etk::avg(float(LLONG_MIN), tmp[iii], float(LLONG_MAX)));
		}
		return out;
	} else if (createType<std::vector<double>>() == type) {
		const double* tmp = reinterpret_cast<const double*>(pointer);
		int32_t nbElement = dataSize / sizeof(double);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = int64_t(etk::avg(double(LLONG_MIN), tmp[iii], double(LLONG_MAX)));
		}
		return out;
	}
	ZEUS_ERROR("Can not get type from '" << type << "'");
	return out;
}

template<>
std::vector<float> zeus::Buffer::internalGetParameter<std::vector<float>>(int32_t _id) const {
	std::vector<float> out;
	std::string type = internalGetParameterType(_id);
	const uint8_t* pointer = internalGetParameterPointer(_id);
	uint32_t dataSize = internalGetParameterSize(_id);
	// TODO : Check size ...
	if (type == "vector:empty") {
		return out;
	} else if (createType<std::vector<uint8_t>>() == type) {
		const uint8_t* tmp = reinterpret_cast<const uint8_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint8_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<uint16_t>>() == type) {
		const uint16_t* tmp = reinterpret_cast<const uint16_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint16_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<uint32_t>>() == type) {
		const uint32_t* tmp = reinterpret_cast<const uint32_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint32_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<uint64_t>>() == type) {
		const uint64_t* tmp = reinterpret_cast<const uint64_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint64_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<int8_t>>() == type) {
		const int8_t* tmp = reinterpret_cast<const int8_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int8_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<int16_t>>() == type) {
		const int16_t* tmp = reinterpret_cast<const int16_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int16_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<int32_t>>() == type) {
		const int32_t* tmp = reinterpret_cast<const int32_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int32_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<int64_t>>() == type) {
		const int64_t* tmp = reinterpret_cast<const int64_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int64_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<float>>() == type) {
		int32_t nbElement = dataSize / sizeof(float);
		out.resize(nbElement);
		memcpy(&out, pointer, nbElement * sizeof(float));
		return out;
	} else if (createType<std::vector<double>>() == type) {
		const double* tmp = reinterpret_cast<const double*>(pointer);
		int32_t nbElement = dataSize / sizeof(double);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	}
	ZEUS_ERROR("Can not get type from '" << type << "'");
	return out;
}

template<>
std::vector<double> zeus::Buffer::internalGetParameter<std::vector<double>>(int32_t _id) const {
	std::vector<double> out;
	std::string type = internalGetParameterType(_id);
	const uint8_t* pointer = internalGetParameterPointer(_id);
	uint32_t dataSize = internalGetParameterSize(_id);
	// TODO : Check size ...
	if (type == "vector:empty") {
		return out;
	} else if (createType<std::vector<uint8_t>>() == type) {
		const uint8_t* tmp = reinterpret_cast<const uint8_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint8_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<uint16_t>>() == type) {
		const uint16_t* tmp = reinterpret_cast<const uint16_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint16_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<uint32_t>>() == type) {
		const uint32_t* tmp = reinterpret_cast<const uint32_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint32_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<uint64_t>>() == type) {
		const uint64_t* tmp = reinterpret_cast<const uint64_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint64_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<int8_t>>() == type) {
		const int8_t* tmp = reinterpret_cast<const int8_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int8_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<int16_t>>() == type) {
		const int16_t* tmp = reinterpret_cast<const int16_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int16_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<int32_t>>() == type) {
		const int32_t* tmp = reinterpret_cast<const int32_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int32_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<int64_t>>() == type) {
		const int64_t* tmp = reinterpret_cast<const int64_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(int64_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<float>>() == type) {
		const float* tmp = reinterpret_cast<const float*>(pointer);
		int32_t nbElement = dataSize / sizeof(float);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii];
		}
		return out;
	} else if (createType<std::vector<double>>() == type) {
		int32_t nbElement = dataSize / sizeof(double);
		out.resize(nbElement);
		memcpy(&out, pointer, nbElement * sizeof(double));
		return out;
	}
	ZEUS_ERROR("Can not get type from '" << type << "'");
	return out;
}

template<>
std::vector<bool> zeus::Buffer::internalGetParameter<std::vector<bool>>(int32_t _id) const {
	std::vector<bool> out;
	std::string type = internalGetParameterType(_id);
	const uint8_t* pointer = internalGetParameterPointer(_id);
	uint32_t dataSize = internalGetParameterSize(_id);
	// TODO : Check size ...
	if (type == "vector:empty") {
		return out;
	} else if (createType<std::vector<bool>>() == type) {
		const uint8_t* tmp = reinterpret_cast<const uint8_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint8_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii] == 'T';
		}
		return out;
	}
	ZEUS_ERROR("Can not get type from '" << type << "'");
	return out;
}

template<>
std::vector<std::string> zeus::Buffer::internalGetParameter<std::vector<std::string>>(int32_t _id) const {
	std::vector<std::string> out;
	std::string type = internalGetParameterType(_id);
	const uint8_t* pointer = internalGetParameterPointer(_id);
	uint32_t dataSize = internalGetParameterSize(_id);
	if (type == "vector:empty") {
		return out;
	} else if (createType<std::vector<std::string>>() == type) {
		// first element is the number of elements:
		const uint16_t* tmp = reinterpret_cast<const uint16_t*>(pointer);
		out.resize(*tmp);
		pointer += sizeof(uint16_t);
		ZEUS_DEBUG("Parse list of string: Find " << out.size() << " elements");
		//each string is separated with a \0:
		for (int32_t iii=0; iii<out.size(); ++iii) {
			const char* tmp2 = reinterpret_cast<const char*>(pointer);
			out[iii] = tmp2;
			pointer += out[iii].size() + 1;
			ZEUS_DEBUG("    value: '" << out[iii] << "'");
		}
		return out;
	}
	ZEUS_ERROR("Can not get type from '" << type << "'");
	return out;
}

template<>
zeus::File zeus::Buffer::internalGetParameter<zeus::File>(int32_t _id) const {
	zeus::File out;
	std::string type = internalGetParameterType(_id);
	const uint8_t* pointer = internalGetParameterPointer(_id);
	uint32_t dataSize = internalGetParameterSize(_id);
	// TODO : Check size ...
	if (createType<zeus::File>() == type) {
		/*
		const uint8_t* tmp = reinterpret_cast<const uint8_t*>(pointer);
		int32_t nbElement = dataSize / sizeof(uint8_t);
		out.resize(nbElement);
		for (size_t iii=0; iii<nbElement; ++iii) {
			out[iii] = tmp[iii] == 'T';
		}
		*/
		ZEUS_TODO("Generate retrun of file...");
		return out;
	}
	ZEUS_ERROR("Can not get type from '" << type << "'");
	return out;
}


void zeus::Buffer::addError(const std::string& _value, const std::string& _comment) {
	addParameter(_value);
	addParameter(_comment);
}

std::string zeus::Buffer::getCall() const {
	std::string out;
	switch(getType()) {
		case zeus::Buffer::typeMessage::call:
			return internalGetParameter<std::string>(0);
			break;
		case zeus::Buffer::typeMessage::answer:
			ZEUS_WARNING("get 'call' with an input type: 'answer'");
			break;
		case zeus::Buffer::typeMessage::event:
			ZEUS_WARNING("get 'call' with an input type: 'event'");
			break;
		default:
			ZEUS_ERROR("unknow type: " << uint16_t(getType()));
			break;
	}
	return "";
}

void zeus::Buffer::setCall(std::string _value) {
	if (m_paramOffset.size() != 0) {
		ZEUS_ERROR("Clear Buffer of parameter ==> set the call type in first ...");
		m_paramOffset.clear();
		m_data.clear();
	}
	addParameter(_value);
}

uint64_t zeus::Buffer::prepare() {
	m_header.numberOfParameter = m_paramOffset.size();
	uint64_t size = sizeof(headerBin);
	size += m_paramOffset.size() * sizeof(uint16_t); // param list
	size += m_data.size();
	return size;
}

bool zeus::Buffer::hasError() {
	if (getType() != zeus::Buffer::typeMessage::answer) {
		return false;
	}
	if (m_paramOffset.size() == 2) {
		return true;
	} else if (m_paramOffset.size() == 3) {
		return true;
	}
	return false;
}

std::string zeus::Buffer::getError() {
	if (getType() != zeus::Buffer::typeMessage::answer) {
		return "";
	}
	if (m_paramOffset.size() == 2) {
		return getParameter<std::string>(0);
	} else if (m_paramOffset.size() == 3) {
		return getParameter<std::string>(1);
	}
	return "";
}

std::string zeus::Buffer::getErrorHelp() {
	if (getType() != zeus::Buffer::typeMessage::answer) {
		return "";
	}
	if (m_paramOffset.size() == 2) {
		return getParameter<std::string>(1);
	} else if (m_paramOffset.size() == 3) {
		return getParameter<std::string>(2);
	}
	return "";
}
