/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

#include <zeus/ObjectRemote.hpp>

namespace zeus {
	/**
	 * @brief This object permit to simplify the acces of the introspection propoerty of a remote service.
	 */
	class ObjectIntrospect {
		protected:
			zeus::ObjectRemote& m_obj; //!< Service instance handle
		public:
			/**
			 * @brief Contructor on the object introspection element.
			 * @param[in] _obj Object to introspect.
			 */
			ObjectIntrospect(zeus::ObjectRemote& _obj);
			/**
			 * @brief Get the service/ object description
			 * @return A future of the description
			 */
			zeus::Future<std::string> getDescription();
			/**
			 * @brief Get the version of the service/Object
			 * @return A future of the string version
			 */
			zeus::Future<std::string> getVersion();
			/**
			 * @brief Get the Type of the service/object. ex: VIDEO, PICTURE, FILE
			 * @return A future of the string type
			 */
			zeus::Future<std::string> getType();
			/**
			 * @brief Get the list of extention availlable of the object (more specific of the service)
			 * @return A future on all extention availlable. Format: "typeExtention:xx.yy.zz"
			 */
			zeus::Future<std::vector<std::string>> getExtention();
			/**
			 * @brief get the list of all authors of the objects
			 * @return A future on the list of authors. Format: "NAME surname <email@someware.xxx>"
			 */
			zeus::Future<std::vector<std::string>> getAuthors();
			/**
			 * @brief get all the function names
			 * @return Future on a list of function names
			 */
			zeus::Future<std::vector<std::string>> getFunctions();
			/**
			 * @brief Get the fonction prototype (same as IDL)
			 * @param[in] _functionName Name of the function
			 * @return a future on the function prototype
			 */
			zeus::Future<std::string> getFunctionPrototype(std::string _functionName);
			/**
			 * @brief Det a fonction specific description
			 * @param[in] _functionName Name of the function
			 * @return a future on the function description
			 */
			zeus::Future<std::string> getFunctionDescription(std::string _functionName);
	};
}
