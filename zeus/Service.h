/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <zeus/TcpString.h>
#include <eproperty/Value.h>
#include <zeus/AbstractFunctionTypeDirect.h>
#include <zeus/AbstractFunctionTypeClass.h>
#include <zeus/debug.h>
#include <zeus/RemoteProcessCall.h>
#include <zeus/Future.h>

namespace zeus {
	class ClientProperty {
		public:
			ClientProperty(const std::string& _clientName="", const std::vector<std::string>& _groups = std::vector<std::string>()) :
			  m_name(_clientName),
			  m_groups(_groups) {
				
			}
		private:
			std::string m_name;
		public:
			void setName(const std::string& _name) {
				m_name = _name;
			}
			const std::string& getName() {
				return m_name;
			}
		private:
			std::vector<std::string> m_groups;
		public:
			void setGroups(std::vector<std::string> _groups) {
				m_groups = _groups;
			}
			const std::vector<std::string>& getGroups() {
				return m_groups;
			}
		private:
			std::vector<std::string> m_listAthorizedFunction;
		public:
			void addAuthorized(const std::string& _funcName) {
				m_listAthorizedFunction.push_back(_funcName);
			}
			bool isFunctionAuthorized(const std::string& _funcName) {
				return std::find(m_listAthorizedFunction.begin(), m_listAthorizedFunction.end(), _funcName) != m_listAthorizedFunction.end();
			}
	};
}
namespace zeus {
	class Service : public eproperty::Interface, public zeus::RemoteProcessCall {
		protected:
			std::mutex m_mutex;
		public:
			eproperty::Value<std::string> propertyIp;
			eproperty::Value<uint16_t> propertyPort;
		protected:
			ememory::SharedPtr<zeus::TcpString> m_interfaceClient;
			uint32_t m_id;
			std::vector<std::string> m_newData;
			std::vector<zeus::FutureCall> m_callMultiData;
		public:
			Service();
			virtual ~Service();
			void connect(const std::string& _serviceName, uint32_t _numberRetry = 1);
			void disconnect();
		private:
			void onClientData(zeus::Buffer& _value);
		public:
			void pingIsAlive();
			bool GateWayAlive();
		private:
			void onPropertyChangeIp();
			void onPropertyChangePort();
			/**
			 * @brief A extern client connect on specific user
			 * @param[in] _clientId Source session Id on the client
			 * @param[in] _userName User name of the client to connect
			 * @todo Set a relur like ==> service not availlable / service close / service maintenance / service right reject
			 */
			virtual void clientConnect(uint64_t _clientId, const std::string& _userName, const std::string& _clientName, const std::vector<std::string>& _groups) = 0;
			virtual void clientDisconnect(uint64_t _clientId) = 0;
			// Genenric function call:
			void callBinary(uint32_t _transactionId, zeus::Buffer& _obj);
			virtual void callBinary2(uint32_t _transactionId, uint64_t _clientId, const std::string& _call, zeus::Buffer& _obj) = 0;
			std::vector<std::string> getExtention();
		public:
			// Add Local fuction (depend on this class)
			template<class ZEUS_RETURN_VALUE,
			         class ZEUS_CLASS_TYPE,
			         class... ZEUS_FUNC_ARGS_TYPE>
			void advertise(std::string _name,
			               ZEUS_RETURN_VALUE (ZEUS_CLASS_TYPE::*_func)(ZEUS_FUNC_ARGS_TYPE... _args),
			               const std::string& _desc = "") {
				_name = "srv." + _name;
				for (auto &it : m_listFunction) {
					if (it == nullptr) {
						continue;
					}
					if (it->getName() == _name) {
						ZEUS_ERROR("Advertise function already bind .. ==> can not be done...: '" << _name << "'");
						return;
					}
				}
				AbstractFunction* tmp = createAbstractFunctionClass(_name, _desc, _func);
				if (tmp == nullptr) {
					ZEUS_ERROR("can not create abstract function ... '" << _name << "'");
					return;
				}
				tmp->setType(zeus::AbstractFunction::type::service);
				ZEUS_INFO("Add function '" << _name << "' in local mode");
				m_listFunction.push_back(tmp);
			}
		
			
	};
	template<class ZEUS_TYPE_SERVICE, class ZEUS_USER_ACCESS>
	class ServiceType : public zeus::Service {
		private:
			ZEUS_USER_ACCESS& m_getUserInterface;
			// no need of shared_ptr or unique_ptr (if service die all is lost and is client die, the gateway notify us...)
			std::map<uint64_t, std::pair<ememory::SharedPtr<ClientProperty>, ememory::SharedPtr<ZEUS_TYPE_SERVICE>>> m_interface;
		public:
			template<class ZEUS_RETURN_VALUE,
			         class ZEUS_CLASS_TYPE,
			         class... ZEUS_FUNC_ARGS_TYPE>
			void advertise(const std::string& _name,
			               ZEUS_RETURN_VALUE (ZEUS_CLASS_TYPE::*_func)(ZEUS_FUNC_ARGS_TYPE... _args),
			               const std::string& _desc = "") {
				if (etk::start_with(_name, "srv.") == true) {
					ZEUS_ERROR("Advertise function start with 'srv.' is not permited ==> only allow for internal service: '" << _name << "'");
					return;
				}
				for (auto &it : m_listFunction) {
					if (it == nullptr) {
						continue;
					}
					if (it->getName() == _name) {
						ZEUS_ERROR("Advertise function already bind .. ==> can not be done...: '" << _name << "'");
						return;
					}
				}
				AbstractFunction* tmp = createAbstractFunctionClass(_name, _desc, _func);
				if (tmp == nullptr) {
					ZEUS_ERROR("can not create abstract function ... '" << _name << "'");
					return;
				}
				tmp->setType(zeus::AbstractFunction::type::object);
				ZEUS_INFO("Add function '" << _name << "' in object mode");
				m_listFunction.push_back(tmp);
			}
			ServiceType(ZEUS_USER_ACCESS& _interface):
			  m_getUserInterface(_interface) {
				
			}
			bool isFunctionAuthorized(uint64_t _clientId, const std::string& _funcName) {
				auto it = m_interface.find(_clientId);
				if (it == m_interface.end()) {
					return false;
				}
				return it->second.first->isFunctionAuthorized(_funcName);
			}
			void clientConnect(uint64_t _clientId, const std::string& _userName, const std::string& _clientName, const std::vector<std::string>& _groups) {
				std::unique_lock<std::mutex> lock(m_mutex);
				ZEUS_DEBUG("connect: " << _clientId << " to '" << _userName << "'");
				ZEUS_DEBUG("    client name='" << _clientName << "'");
				ZEUS_DEBUG("    groups=" << etk::to_string(_groups));
				ememory::SharedPtr<ClientProperty> tmpProperty = std::make_shared<ClientProperty>(_clientName, _groups);
				ememory::SharedPtr<ZEUS_TYPE_SERVICE> tmpSrv = std::make_shared<ZEUS_TYPE_SERVICE>(m_getUserInterface.getUser(_userName), tmpProperty);
				m_interface.insert(std::make_pair(_clientId, std::make_pair(tmpProperty, tmpSrv)));
				// enable list of function availlable: 
				for (auto &it : m_listFunction) {
					if (it == nullptr) {
						continue;
					}
					tmpProperty->addAuthorized(it->getName());
				}
			}
			void clientDisconnect(uint64_t _clientId) {
				std::unique_lock<std::mutex> lock(m_mutex);
				ZEUS_DEBUG("disconnect: " << _clientId);
				auto it = m_interface.find(_clientId);
				if (it == m_interface.end()) {
					ZEUS_WARNING("disconnect ==> Not find Client ID " << _clientId);
					// noting to do ==> user never conected.
					return;
				}
				m_interface.erase(it);
			}
			void clientSetName(uint64_t _clientId, const std::string& _clientName) {
				std::unique_lock<std::mutex> lock(m_mutex);
				auto it = m_interface.find(_clientId);
				if (it == m_interface.end()) {
					ZEUS_ERROR("Change the client property but client was not created ...");
					return;
				}
				it->second.first->setName(_clientName);
			}
			void clientSetGroup(uint64_t _clientId, const std::vector<std::string>& _clientGroups) {
				std::unique_lock<std::mutex> lock(m_mutex);
				auto it = m_interface.find(_clientId);
				if (it == m_interface.end()) {
					ZEUS_ERROR("Change the client property but client was not created ...");
					return;
				}
				it->second.first->setGroups(_clientGroups);
			}
			void callBinary2(uint32_t _transactionId, uint64_t _clientId, const std::string& _call, zeus::Buffer& _obj) {
				auto it = m_interface.find(_clientId);
				if (it == m_interface.end()) {
					m_interfaceClient->answerError(_transactionId, "CLIENT-UNKNOW", "", _clientId);
					return;
				}
				for (auto &it2 : m_listFunction) {
					if (it2 == nullptr) {
						continue;
					}
					if (it2->getName() != _call) {
						continue;
					}
					switch (it2->getType()) {
						case zeus::AbstractFunction::type::object: {
							ZEUS_TYPE_SERVICE* elem = it->second.second.get();
							it2->execute(m_interfaceClient, _transactionId, _clientId, _obj, (void*)elem);
							return;
						}
						case zeus::AbstractFunction::type::local: {
							it2->execute(m_interfaceClient, _transactionId, _clientId, _obj, (void*)((RemoteProcessCall*)this));
							return;
						}
						case zeus::AbstractFunction::type::service: {
							it2->execute(m_interfaceClient, _transactionId, _clientId, _obj, (void*)this);
							return;
						}
						case zeus::AbstractFunction::type::global: {
							it2->execute(m_interfaceClient, _transactionId, _clientId, _obj, nullptr);
							return;
						}
						case zeus::AbstractFunction::type::unknow:
							ZEUS_ERROR("Can not call unknow type ...");
							break;
					}
				}
				m_interfaceClient->answerError(_transactionId, "FUNCTION-UNKNOW", "", _clientId);
				return;
			}
	};
}

