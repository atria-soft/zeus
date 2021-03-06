/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
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
			etk::String m_clientName;
			clientSpecificInterface();
			~clientSpecificInterface();
			bool start(appl::GateWay* _gateway, zeus::WebServer* _interfaceWeb, uint16_t _id);
			void send(ememory::SharedPtr<zeus::Message> _data);
			//void answerProtocolError(uint32_t _transactionId, const etk::String& _errorHelp);
			zeus::WebServer* getInterface();
			bool isConnected();
	};
	
	class RouterInterface {
		private:
			enum clientState m_state; // state machine ..
			etk::Vector<ememory::SharedPtr<clientSpecificInterface>> m_listClients; // TODO : Maybe remove this ...
		private:
			appl::GateWay* m_gateway;
			zeus::WebServer m_interfaceWeb;
		public:
			RouterInterface(const etk::String& _ip, uint16_t _port, etk::String _userName, appl::GateWay* _gateway, uint32_t _directPort);
			virtual ~RouterInterface();
			void stop();
			void onClientData(ememory::SharedPtr<zeus::Message> _value);
			bool isAlive();
			void send(const ememory::SharedPtr<zeus::Message>& _data);
			void clean();
			echrono::Steady getLastTransmission();
	};
}

