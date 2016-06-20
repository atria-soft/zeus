/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <enet/TcpClient.h>
#include <zeus/Client.h>
#include <zeus/debug.h>

#include <unistd.h>

zeus::Client::Client() :
  propertyIp(this, "ip", "127.0.0.1", "Ip to connect server", &zeus::Client::onPropertyChangeIp),
  propertyPort(this, "port", 1983, "Port to connect server", &zeus::Client::onPropertyChangePort) {
	
}

zeus::Client::~Client() {
	
}

void zeus::Client::onClientData(zeus::Buffer& _value) {
	ZEUS_ERROR("Get Data On the Communication interface that is not understand ... : " << _value.generateHumanString());
}

zeus::ServiceRemote zeus::Client::getService(const std::string& _name) {
	return zeus::ServiceRemote(m_interfaceClient, _name);
}

void zeus::Client::onPropertyChangeIp() {
	disconnect();
}

void zeus::Client::onPropertyChangePort(){
	disconnect();
}


bool zeus::Client::connect(const std::string& _remoteUserToConnect){
	ZEUS_DEBUG("connect [START]");
	disconnect();
	enet::Tcp connection = std::move(enet::connectTcpClient(*propertyIp, *propertyPort));
	m_interfaceClient = std::make_shared<zeus::TcpString>();
	if (m_interfaceClient == nullptr) {
		ZEUS_ERROR("Allocate connection error");
		return false;
	}
	m_interfaceClient->connect(this, &zeus::Client::onClientData);
	m_interfaceClient->setInterface(std::move(connection), false);
	m_interfaceClient->connect();
	
	ZEUS_WARNING("Request connect user " << _remoteUserToConnect);
	zeus::Future<bool> ret = call("connectToUser", _remoteUserToConnect, "zeus-client");
	ret.wait();
	if (ret.hasError() == true) {
		ZEUS_WARNING("Can not connect to user named: '" << _remoteUserToConnect << "' ==> return error");
		return false;
	}
	if (ret.get() == true) {
		ZEUS_WARNING("    ==> accepted connection");
	} else {
		ZEUS_WARNING("    ==> Refuse connection");
	}
	return ret.get();
}

void zeus::Client::disconnect() {
	ZEUS_DEBUG("disconnect [START]");
	if (m_interfaceClient != nullptr) {
		m_interfaceClient->disconnect();
		m_interfaceClient.reset();
	} else {
		ZEUS_VERBOSE("Nothing to disconnect ...");
	}
	ZEUS_DEBUG("disconnect [STOP]");
}