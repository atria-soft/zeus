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
namespace zeus {
	template<>
	bool Buffer::internalGetParameter<bool>(int32_t _id) const {
		zeus::ParamType type = internalGetParameterType(_id);
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
	std::string Buffer::internalGetParameter<std::string>(int32_t _id) const {
		std::string out;
		zeus::ParamType type = internalGetParameterType(_id);
		const uint8_t* pointer = internalGetParameterPointer(_id);
		uint32_t dataSize = internalGetParameterSize(_id);
		out.resize(dataSize, 0);
		memcpy(&out[0], pointer, out.size());
		return out;
	}
	
	
	template<>
	uint8_t Buffer::internalGetParameter<uint8_t>(int32_t _id) const {
		zeus::ParamType type = internalGetParameterType(_id);
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
	uint16_t Buffer::internalGetParameter<uint16_t>(int32_t _id) const {
		zeus::ParamType type = internalGetParameterType(_id);
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
	uint32_t Buffer::internalGetParameter<uint32_t>(int32_t _id) const {
		zeus::ParamType type = internalGetParameterType(_id);
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
	uint64_t Buffer::internalGetParameter<uint64_t>(int32_t _id) const {
		zeus::ParamType type = internalGetParameterType(_id);
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
	int8_t Buffer::internalGetParameter<int8_t>(int32_t _id) const {
		zeus::ParamType type = internalGetParameterType(_id);
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
	int16_t Buffer::internalGetParameter<int16_t>(int32_t _id) const {
		zeus::ParamType type = internalGetParameterType(_id);
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
	int32_t Buffer::internalGetParameter<int32_t>(int32_t _id) const {
		zeus::ParamType type = internalGetParameterType(_id);
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
	int64_t Buffer::internalGetParameter<int64_t>(int32_t _id) const {
		zeus::ParamType type = internalGetParameterType(_id);
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
	float Buffer::internalGetParameter<float>(int32_t _id) const {
		zeus::ParamType type = internalGetParameterType(_id);
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
	double Buffer::internalGetParameter<double>(int32_t _id) const {
		zeus::ParamType type = internalGetParameterType(_id);
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
	std::vector<uint8_t> Buffer::internalGetParameter<std::vector<uint8_t>>(int32_t _id) const {
		std::vector<uint8_t> out;
		zeus::ParamType type = internalGetParameterType(_id);
		const uint8_t* pointer = internalGetParameterPointer(_id);
		uint32_t dataSize = internalGetParameterSize(_id);
		// TODO : Check size ...
		if (type == createType<std::vector<void>>()) {
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
	std::vector<uint16_t> Buffer::internalGetParameter<std::vector<uint16_t>>(int32_t _id) const {
		std::vector<uint16_t> out;
		zeus::ParamType type = internalGetParameterType(_id);
		const uint8_t* pointer = internalGetParameterPointer(_id);
		uint32_t dataSize = internalGetParameterSize(_id);
		// TODO : Check size ...
		if (type == createType<std::vector<void>>()) {
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
	std::vector<uint32_t> Buffer::internalGetParameter<std::vector<uint32_t>>(int32_t _id) const {
		std::vector<uint32_t> out;
		zeus::ParamType type = internalGetParameterType(_id);
		const uint8_t* pointer = internalGetParameterPointer(_id);
		uint32_t dataSize = internalGetParameterSize(_id);
		// TODO : Check size ...
		if (type == createType<std::vector<void>>()) {
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
	std::vector<uint64_t> Buffer::internalGetParameter<std::vector<uint64_t>>(int32_t _id) const {
		std::vector<uint64_t> out;
		zeus::ParamType type = internalGetParameterType(_id);
		const uint8_t* pointer = internalGetParameterPointer(_id);
		uint32_t dataSize = internalGetParameterSize(_id);
		// TODO : Check size ...
		if (type == createType<std::vector<void>>()) {
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
	std::vector<int8_t> Buffer::internalGetParameter<std::vector<int8_t>>(int32_t _id) const {
		std::vector<int8_t> out;
		zeus::ParamType type = internalGetParameterType(_id);
		const uint8_t* pointer = internalGetParameterPointer(_id);
		uint32_t dataSize = internalGetParameterSize(_id);
		// TODO : Check size ...
		if (type == createType<std::vector<void>>()) {
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
	std::vector<int16_t> Buffer::internalGetParameter<std::vector<int16_t>>(int32_t _id) const {
		std::vector<int16_t> out;
		zeus::ParamType type = internalGetParameterType(_id);
		const uint8_t* pointer = internalGetParameterPointer(_id);
		uint32_t dataSize = internalGetParameterSize(_id);
		// TODO : Check size ...
		if (type == createType<std::vector<void>>()) {
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
	std::vector<int32_t> Buffer::internalGetParameter<std::vector<int32_t>>(int32_t _id) const {
		std::vector<int32_t> out;
		zeus::ParamType type = internalGetParameterType(_id);
		const uint8_t* pointer = internalGetParameterPointer(_id);
		uint32_t dataSize = internalGetParameterSize(_id);
		// TODO : Check size ...
		if (type == createType<std::vector<void>>()) {
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
	std::vector<int64_t> Buffer::internalGetParameter<std::vector<int64_t>>(int32_t _id) const {
		std::vector<int64_t> out;
		zeus::ParamType type = internalGetParameterType(_id);
		const uint8_t* pointer = internalGetParameterPointer(_id);
		uint32_t dataSize = internalGetParameterSize(_id);
		// TODO : Check size ...
		if (type == createType<std::vector<void>>()) {
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
	std::vector<float> Buffer::internalGetParameter<std::vector<float>>(int32_t _id) const {
		std::vector<float> out;
		zeus::ParamType type = internalGetParameterType(_id);
		const uint8_t* pointer = internalGetParameterPointer(_id);
		uint32_t dataSize = internalGetParameterSize(_id);
		// TODO : Check size ...
		if (type == createType<std::vector<void>>()) {
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
	std::vector<double> Buffer::internalGetParameter<std::vector<double>>(int32_t _id) const {
		std::vector<double> out;
		zeus::ParamType type = internalGetParameterType(_id);
		const uint8_t* pointer = internalGetParameterPointer(_id);
		uint32_t dataSize = internalGetParameterSize(_id);
		// TODO : Check size ...
		if (type == createType<std::vector<void>>()) {
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
	std::vector<bool> Buffer::internalGetParameter<std::vector<bool>>(int32_t _id) const {
		std::vector<bool> out;
		zeus::ParamType type = internalGetParameterType(_id);
		const uint8_t* pointer = internalGetParameterPointer(_id);
		uint32_t dataSize = internalGetParameterSize(_id);
		// TODO : Check size ...
		if (type == createType<std::vector<void>>()) {
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
	std::vector<std::string> Buffer::internalGetParameter<std::vector<std::string>>(int32_t _id) const {
		std::vector<std::string> out;
		zeus::ParamType type = internalGetParameterType(_id);
		const uint8_t* pointer = internalGetParameterPointer(_id);
		uint32_t dataSize = internalGetParameterSize(_id);
		if (type == createType<std::vector<void>>()) {
			return out;
		} else if (createType<std::vector<std::string>>() == type) {
			// first element is the number of elements:
			const uint16_t* tmp = reinterpret_cast<const uint16_t*>(pointer);
			out.resize(*tmp);
			pointer += sizeof(uint16_t);
			//each string is separated with a \0:
			for (int32_t iii=0; iii<out.size(); ++iii) {
				const char* tmp2 = reinterpret_cast<const char*>(pointer);
				out[iii] = tmp2;
				pointer += out[iii].size() + 1;
			}
			return out;
		}
		ZEUS_ERROR("Can not get type from '" << type << "'");
		return out;
	}
	
	template<>
	zeus::File Buffer::internalGetParameter<zeus::File>(int32_t _id) const {
		zeus::File out;
		zeus::ParamType type = internalGetParameterType(_id);
		const uint8_t* pointer = internalGetParameterPointer(_id);
		uint32_t dataSize = internalGetParameterSize(_id);
		// TODO : Check size ...
		if (createType<zeus::File>() == type) {
			// get mine type in string:
			std::string mineType;
			int32_t currentOffset = 0;
			while (    char(pointer[currentOffset]) != '\0'
			        && currentOffset<dataSize) {
				mineType += char(pointer[currentOffset]);
				currentOffset++;
			}
			out.setMineType(mineType);
			// finish with '\0'
			currentOffset ++;
			if (currentOffset >= dataSize) {
				ZEUS_ERROR("Parameter size is wrong ...");
				return out;
			}
			// get size if the file in int32_t
			int32_t size = 0;
			memcpy(&size, &pointer[currentOffset], sizeof(int32_t));
			currentOffset += sizeof(int32_t);
			out.preSetDataSize(dataSize);
			// Get the datas:
			out.setData(0, &pointer[currentOffset], dataSize);
			return out;
		}
		ZEUS_ERROR("Can not get type from '" << type << "'");
		return out;
	}
}
