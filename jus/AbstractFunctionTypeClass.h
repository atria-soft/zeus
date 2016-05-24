/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <jus/TcpString.h>
#include <eproperty/Value.h>
#include <ejson/ejson.h>
#include <jus/debug.h>
#include <jus/AbstractFunction.h>
namespace jus {
	template <class JUS_CLASS_TYPE, class JUS_RETURN, class... JUS_TYPES>
	ejson::Value executeClassCallJson(JUS_CLASS_TYPE* _pointer, JUS_RETURN (JUS_CLASS_TYPE::*_func)(JUS_TYPES...), const ejson::Array& _params) {
		#if defined(__clang__)
			// clang generate a basic warning:
			//      warning: multiple unsequenced modifications to 'idParam' [-Wunsequenced]
			int32_t idParam = 0;
			return convertToJson((*_pointer.*_func)((convertJsonTo<JUS_TYPES>(_params[idParam++]))...));
		#elif defined(__GNUC__) || defined(__GNUG__) || defined(_MSC_VER)
			int32_t idParam = int32_t(sizeof...(JUS_TYPES))-1;
			return convertToJson((*_pointer.*_func)(convertJsonTo<JUS_TYPES>(_params[idParam--])...));
		#else
			#error Must be implemented ...
		#endif
		return ejson::Null();
	}
	
	template <class JUS_CLASS_TYPE, class... JUS_TYPES>
	ejson::Value executeClassCallJson(JUS_CLASS_TYPE* _pointer, void (JUS_CLASS_TYPE::*_func)(JUS_TYPES...), const ejson::Array& _params) {
		ejson::Object out;
		#if defined(__clang__)
			// clang generate a basic warning:
			//      warning: multiple unsequenced modifications to 'idParam' [-Wunsequenced]
			int32_t idParam = 0;
			(*_pointer.*_func)((convertJsonTo<JUS_TYPES>(_params[idParam++]))...);
		#elif defined(__GNUC__) || defined(__GNUG__) || defined(_MSC_VER)
			int32_t idParam = int32_t(sizeof...(JUS_TYPES))-1;
			(*_pointer.*_func)(convertJsonTo<JUS_TYPES>(_params[idParam--])...);
		#else
			#error Must be implemented ...
		#endif
		return ejson::Null();
	}
	
	template <class JUS_CLASS_TYPE, class JUS_RETURN, class... JUS_TYPES>
	std::string executeClassCallString(JUS_CLASS_TYPE* _pointer, JUS_RETURN (JUS_CLASS_TYPE::*_func)(JUS_TYPES...), const std::vector<std::string>& _params) {
		#if defined(__clang__)
			// clang generate a basic warning:
			//      warning: multiple unsequenced modifications to 'idParam' [-Wunsequenced]
			int32_t idParam = 0;
			return etk::to_string((*_pointer.*_func)((convertStringTo<JUS_TYPES>(_params[idParam++]))...));
		#elif defined(__GNUC__) || defined(__GNUG__) || defined(_MSC_VER)
			int32_t idParam = int32_t(sizeof...(JUS_TYPES))-1;
			return etk::to_string((*_pointer.*_func)(convertStringTo<JUS_TYPES>(_params[idParam--])...));
		#else
			#error Must be implemented ...
		#endif
		return "";
	}
	template <class JUS_CLASS_TYPE, class... JUS_TYPES>
	std::string executeClassCallString(JUS_CLASS_TYPE* _pointer, void (JUS_CLASS_TYPE::*_func)(JUS_TYPES...), const std::vector<std::string>& _params) {
		ejson::Object out;
		#if defined(__clang__)
			// clang generate a basic warning:
			//      warning: multiple unsequenced modifications to 'idParam' [-Wunsequenced]
			int32_t idParam = 0;
			(*_pointer.*_func)((convertStringTo<JUS_TYPES>(_params[idParam++]))...);
		#elif defined(__GNUC__) || defined(__GNUG__) || defined(_MSC_VER)
			int32_t idParam = int32_t(sizeof...(JUS_TYPES))-1;
			(*_pointer.*_func)(convertStringTo<JUS_TYPES>(_params[idParam--])...);
		#else
			#error Must be implemented ...
		#endif
		return "";
	}
	
	template <class JUS_RETURN, class JUS_CLASS_TYPE, class... JUS_TYPES>
	class AbstractFunctionTypeClass: public jus::AbstractFunction {
		protected:
			static const ParamType m_returnType;
			static const ParamType m_paramType[sizeof...(JUS_TYPES)];
		public:
			using functionType = JUS_RETURN (JUS_CLASS_TYPE::*)(JUS_TYPES...);
			functionType m_function;
			AbstractFunctionTypeClass(const std::string& _name, const std::string& _desc, functionType _fptr):
			  AbstractFunction(_name, _desc),
			  m_function(_fptr) {
			}
			std::string getPrototype() const override {
				std::string ret;
				ret += m_returnType.getName();
				ret += " ";
				ret += m_name;
				ret += "(";
				for (size_t iii=0; iii<sizeof...(JUS_TYPES); ++iii) {
					if (iii != 0) {
						ret += ", ";
					}
					ret += m_paramType[iii].getName();
				}
				ret += ");";
				return ret;
			}
			ejson::Value executeJson(const ejson::Array& _params, void* _class) override {
				JUS_CLASS_TYPE* tmpClass = (JUS_CLASS_TYPE*)_class;
				ejson::Object out;
				// check parameter number
				if (_params.size() != sizeof...(JUS_TYPES)) {
					JUS_ERROR("Wrong number of Parameters ...");
					out.add("error", ejson::String("WRONG-PARAMETER-NUMBER"));
					std::string help = "request ";
					help += etk::to_string(_params.size());
					help += " parameters and need ";
					help += etk::to_string(sizeof...(JUS_TYPES));
					help += " parameters. prototype function:";
					help += getPrototype();
					out.add("error-help", ejson::String(help));
					return out;
				}
				// check parameter compatibility
				for (size_t iii=0; iii<sizeof...(JUS_TYPES); ++iii) {
					if (checkCompatibility(m_paramType[iii], _params[iii]) == false) {
						out.add("error", ejson::String("WRONG-PARAMETER-TYPE"));
						out.add("error-help", ejson::String("Parameter id " + etk::to_string(iii) + " not compatible with type: '" + m_paramType[iii].getName() + "'"));
						return out;
					}
				}
				// execute cmd:
				ejson::Value retVal = jus::executeClassCallJson(tmpClass, m_function, _params);
				out.add("return", retVal);
				return out;
			}
			std::string executeString(const std::vector<std::string>& _params, void* _class) override {
				JUS_CLASS_TYPE* tmpClass = (JUS_CLASS_TYPE*)_class;
				std::string out;
				// check parameter number
				if (_params.size() != sizeof...(JUS_TYPES)) {
					JUS_ERROR("Wrong number of Parameters ...");
					out += "error:WRONG-PARAMETER-NUMBER;";
					out += "error-help:request ";
					out += etk::to_string(_params.size());
					out += " parameters and need ";
					out += etk::to_string(sizeof...(JUS_TYPES));
					out += " parameters. prototype function:";
					out += getPrototype();
					return out;
				}
				// check parameter compatibility
				for (size_t iii=0; iii<sizeof...(JUS_TYPES); ++iii) {
					if (checkCompatibility(m_paramType[iii], _params[iii]) == false) {
						out += "error:WRONG-PARAMETER-TYPE;";
						out += "error-help:Parameter id " + etk::to_string(iii) + " not compatible with type: '" + m_paramType[iii].getName() + "'";
						return out;
					}
				}
				// execute cmd:
				out = jus::executeClassCallString(tmpClass, m_function, _params);
				return out;
			}
	};
	
	template <class JUS_RETURN, class JUS_CLASS_TYPE, class... JUS_TYPES>
	const ParamType AbstractFunctionTypeClass<JUS_RETURN, JUS_CLASS_TYPE, JUS_TYPES...>::m_returnType = createType<JUS_RETURN>();
	
	template <class JUS_RETURN, class JUS_CLASS_TYPE, class... JUS_TYPES>
	const ParamType AbstractFunctionTypeClass<JUS_RETURN, JUS_CLASS_TYPE, JUS_TYPES...>::m_paramType[sizeof...(JUS_TYPES)] = {createType<JUS_TYPES>()...};
	
	
	template <typename JUS_RETURN, class JUS_CLASS_TYPE, typename... JUS_TYPES>
	AbstractFunction* createAbstractFunctionClass(const std::string& _name, const std::string& _desc, JUS_RETURN (JUS_CLASS_TYPE::*_fffp)(JUS_TYPES...)) {
		return new AbstractFunctionTypeClass<JUS_RETURN, JUS_CLASS_TYPE, JUS_TYPES...>(_name, _desc, _fffp);
	}
}

