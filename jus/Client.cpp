/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <enet/TcpClient.h>
#include <jus/Client.h>
#include <jus/debug.h>

#include <unistd.h>

jus::Client::Client() :
  propertyIp(this, "ip", "127.0.0.1", "Ip to connect server", &jus::Client::onPropertyChangeIp),
  propertyPort(this, "port", 1983, "Port to connect server", &jus::Client::onPropertyChangePort),
  m_id(1) {
	m_interfaceClient.connect(this, &jus::Client::onClientData);
}

jus::Client::~Client() {
	
}

void jus::Client::onClientData(std::string _value) {
	m_newData.push_back(std::move(_value));
}

jus::ServiceRemote jus::Client::getService(const std::string& _name) {
	return jus::ServiceRemote(this, _name);
}

bool jus::Client::link(const std::string& _serviceName) {
	// TODO : Check the number of connection of this service ...
	bool ret = call_b("link", _serviceName);
	if (ret == false) {
		JUS_ERROR("Can not link with the service named: '" << _serviceName << "'");
	}
	return ret;
}

void jus::Client::unlink(const std::string& _serviceName) {
	bool ret = call_b("unlink", _serviceName);
	if (ret == false) {
		JUS_ERROR("Can not unlink with the service named: '" << _serviceName << "'");
	}
}

std::string jus::Client::asyncRead() {
	if (m_interfaceClient.isActive() == false) {
		return "";
	}
	int32_t iii = 5000;
	while (iii>0) {
		usleep(10000);
		if (m_newData.size() != 0) {
			break;
		}
		--iii;
	}
	if (iii == 0) {
		// Time-out ...
		return "";
	}
	std::string out;
	out = std::move(m_newData[0]);
	m_newData.erase(m_newData.begin());
	JUS_DEBUG("get async data: " << out);
	return out;
}

void jus::Client::onPropertyChangeIp() {
	disconnect();
}

void jus::Client::onPropertyChangePort(){
	disconnect();
}


void jus::Client::connect(const std::string& _remoteUserToConnect){
	disconnect();
	JUS_DEBUG("connect [START]");
	enet::Tcp connection = std::move(enet::connectTcpClient(*propertyIp, *propertyPort));
	m_interfaceClient.setInterface(std::move(connection));
	m_interfaceClient.connect();
	bool ret = call_b("connectToUser", _remoteUserToConnect, "jus-client");
	if (ret == false) {
		JUS_ERROR("Connection error");
	}
	JUS_DEBUG("connect [STOP]");
}

void jus::Client::disconnect() {
	JUS_DEBUG("disconnect [START]");
	m_interfaceClient.disconnect();
	JUS_DEBUG("disconnect [STOP]");
}

ejson::Object jus::Client::createBaseCall(const std::string& _functionName, const std::string& _service) {
	ejson::Object obj;
	if (_service.size() != 0) {
		obj.add("service", ejson::String(_service));
	}
	obj.add("call", ejson::String(_functionName));
	obj.add("id", ejson::Number(m_id++));
	obj.add("param", ejson::Array());
	return obj;
}

ejson::Object jus::Client::callJson(const ejson::Object& _obj) {
	JUS_VERBOSE("Call JSON [START] ");
	if (m_interfaceClient.isActive() == false) {
		return ejson::Object();
	}
	JUS_DEBUG("Call JSON '" << _obj.generateHumanString() << "'");
	m_interfaceClient.write(_obj.generateMachineString());
	std::string ret = asyncRead();
	JUS_VERBOSE("Call JSON [STOP]");
	return ejson::Object(ret);
}
