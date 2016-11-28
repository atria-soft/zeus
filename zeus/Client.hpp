/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <zeus/WebServer.hpp>
#include <eproperty/Value.hpp>
#include <zeus/debug.hpp>
#include <chrono>

#include <zeus/Future.hpp>
#include <zeus/ServiceRemote.hpp>

namespace zeus {
	/**
	 * @brief Client interface to acces on the remote service and gateway
	 */
	class Client : public eproperty::Interface {
		friend class ServiceRemote;
		public:
			eproperty::Value<std::string> propertyIp; //!< Ip of WebSocket TCP connection
			eproperty::Value<uint16_t> propertyPort; //!< Port of the WebSocket connection
		private:
			std::string m_clientName; //!< Local client name to generate the local serrvice name if needed (if direct connection ==> no name)
			ememory::SharedPtr<zeus::WebServer> m_interfaceClient; //!< Interface on the Websocket interface
			std::vector<ememory::WeakPtr<zeus::ServiceRemoteBase>> m_listConnectedService; //!< Connect only one time on each service, not needed more.
			std::vector<ememory::SharedPtr<zeus::Service>> m_listProvicedService; //!< Connect only one time on each service, not needed more.
		public:
			/**
			 * @brief 
			 * @param[in] 
			 * @return 
			 */
			Client();
			/**
			 * @brief 
			 * @param[in] 
			 * @return 
			 */
			virtual ~Client();
		protected:
			/**
			 * @brief Connetc to a remote extern server
			 * @param[in] _address Address of the user: "ABCD.efgh~atria-soft.com:1993"
			 */
			bool connectTo(const std::string& _address);
		public:
			/**
			 * @brief Direct connection on a GateWay No Identification needed, the Port on the Gateway garenty the connection)
			 * @note This is exclusively reserve for server service provicers
			 */
			bool connect();
			/**
			 * @brief Create a client on a specific user in an ANONIMOUS way
			 * @param[in] _address Address of the user: "ABCD.efgh~atria-soft.com:1993"
			 */
			bool connect(const std::string& _address);
			/**
			 * @brief Create a client on a specific user in a user mode (connect to your personnal account)
			 * @param[in] _address Address of the user: "ABCD.efgh~atria-soft.com:1993"
			 * @param[in] _userPassword Password of the user
			 */
			bool connect(const std::string& _address, const std::string& _userPassword);
			/**
			 * @brief Create a client on a specific user in a client mode with the tocken associated
			 * @param[in] _address Address of the user: "ABCD.efgh~atria-soft.com:1993"
			 * @param[in] 
			 * @param[in] 
			 */
			bool connect(const std::string& _address, const std::string& _clientName, const std::string& _clientTocken);
			/**
			 * @brief Disconnect of the current interface
			 */
			void disconnect();
		public:
			/**
			 * @brief Get a remote service
			 * @param[in] _serviceName Name of the service
			 * @return Pointer on an interface of remote service
			 */
			zeus::ServiceRemote getService(const std::string& _serviceName);
			/**
			 * @brief Provide a service with a specific name
			 * @param[in] _serviceName Name of the service
			 * @param[in] _service handle on the service provided
			 * @return true if the service is acepted or false if not
			 */
			bool provideService(const std::string& _serviceName, ememory::SharedPtr<zeus::Service>& _service);
		private:
			/**
			 * @brief When receive data from the websocket ... call this ...
			 * @param[in] _value New input buffer
			 */
			void onClientData(ememory::SharedPtr<zeus::Buffer> _value);
		public:
			/**
			 * @brief Create a call on the interface gateway (threw the router)
			 * @param[in] _functionName name of the function to call
			 * @param[in] _args... multiple argument neededs
			 * @return a future that will contain the aswer when receiveed (need to transmit over ethernet)
			 */
			 /*
			template<class... _ARGS>
			zeus::FutureBase call(const std::string& _functionName, _ARGS&&... _args) {
				if (m_interfaceClient == nullptr) {
					ememory::SharedPtr<zeus::BufferAnswer> ret = zeus::BufferAnswer::create();
					ret->addError("NULLPTR", "call " + _functionName + " with no interface open");
					return zeus::FutureBase(0, ret);
				}
				return m_interfaceClient->call(ZEUS_NO_ID_CLIENT, ZEUS_ID_SERVICE_ROOT, _functionName, _args...);
			}
			*/
		private:
			/**
			 * @brief Internal (called when user change the Ip of the client interface)
			 */
			void onPropertyChangeIp();
			/**
			 * @brief  Internal (called when user change the port of the client interface)
			 */
			void onPropertyChangePort();
	};
}

