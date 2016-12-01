/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <zeus/WebServer.hpp>
#include <zeus/debug.hpp>
#include <zeus/AbstractFunction.hpp>
#include <zeus/ServiceRemote.hpp>
#include <zeus/Future.hpp>
#include <zeus/WebServer.hpp>
#include <zeus/WebObj.hpp>

namespace zeus {
	class Client;
	class ServiceRemote;
	/**
	 * @brief 
	 * @param[in] 
	 * @return 
	 */
	class ServiceRemoteBase : public zeus::WebObj {
		friend class ServiceRemote;
		private:
			std::string m_name;
			uint32_t m_serviceId;
			bool m_isLinked;
		public:
			/**
			 * @brief 
			 * @param[in] 
			 * @return 
			 */
			ServiceRemoteBase():
			  zeus::WebObj(nullptr, 0, 0) {
				
			}
			/**
			 * @brief 
			 * @param[in] 
			 * @return 
			 */
			ServiceRemoteBase(ememory::SharedPtr<zeus::WebServer> _clientLink, const std::string& _name, uint16_t _localId, uint16_t _localObjectId);
			/**
			 * @brief 
			 * @param[in] 
			 * @return 
			 */
			~ServiceRemoteBase();
			/**
			 * @brief 
			 * @param[in] 
			 * @return 
			 */
			bool exist() const;
			/**
			 * @brief 
			 * @param[in] 
			 * @return 
			 */
			const std::string& getName() const;
	};
	/**
	 * @brief 
	 * @param[in] 
	 * @return 
	 */
	class ServiceRemote {
		private:
			ememory::SharedPtr<zeus::ServiceRemoteBase> m_interface;
		public:
			/**
			 * @brief 
			 * @param[in] 
			 * @return 
			 */
			ServiceRemote(ememory::SharedPtr<zeus::ServiceRemoteBase> _interface = nullptr);
			/**
			 * @brief 
			 * @param[in] 
			 * @return 
			 */
			~ServiceRemote();
			/**
			 * @brief 
			 * @param[in] 
			 * @return 
			 */
			bool exist() const;
		public:
			/**
			 * @brief 
			 * @param[in] 
			 * @return 
			 */
			template<class... _ARGS>
			zeus::FutureBase call(const std::string& _functionName, _ARGS&&... _args) {
				if (    m_interface == nullptr
				     || m_interface->m_interfaceWeb == nullptr) {
					ememory::SharedPtr<zeus::BufferAnswer> ret = zeus::BufferAnswer::create();
					if (ret != nullptr) {
						ret->addError("NULLPTR", "call " + _functionName + " with no interface open");
					}
					return zeus::FutureBase(0, ret);
				}
				return m_interface->m_interfaceWeb->call(m_interface->getFullId(),
				                                         m_interface->m_serviceId,
				                                         _functionName,
				                                         _args...);
			}
	};
	
	
	
}

