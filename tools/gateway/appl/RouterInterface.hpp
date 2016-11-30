/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <zeus/WebServer.hpp>
#include <appl/GateWay.hpp>
#include <appl/IOInterface.hpp>

namespace appl {
	class GateWay;
	class clientSpecificInterface : public appl::IOInterface {
		public:
			zeus::WebServer* m_interfaceWeb;
		public:
			std::string m_clientName;
			clientSpecificInterface();
			~clientSpecificInterface();
			bool start(appl::GateWay* _gateway, zeus::WebServer* _interfaceWeb, uint16_t _id);
			void send(ememory::SharedPtr<zeus::Buffer> _data);
			//void answerProtocolError(uint32_t _transactionId, const std::string& _errorHelp);
			zeus::WebServer* getInterface() {
				return m_interfaceWeb;
			}
	};
	
	class RouterInterface {
		private:
			enum clientState m_state; // state machine ..
			std::vector<ememory::SharedPtr<clientSpecificInterface>> m_listClients;
		private:
			appl::GateWay* m_gateway;
			zeus::WebServer m_interfaceWeb;
		public:
			RouterInterface(const std::string& _ip, uint16_t _port, std::string _userName, appl::GateWay* _gateway);
			virtual ~RouterInterface();
			void stop();
			void onClientData(ememory::SharedPtr<zeus::Buffer> _value);
			bool isAlive();
			void send(const ememory::SharedPtr<zeus::Buffer>& _data);
			void clean();
	};
}
