/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#include <jus/TcpString.h>
#include <jus/debug.h>
#include <ethread/tools.h>
#include <unistd.h>

jus::TcpString::TcpString(enet::Tcp _connection) :
  m_connection(std::move(_connection)),
  m_thread(nullptr),
  m_observerElement(nullptr),
  m_observerRawElement(nullptr),
  m_threadAsync(nullptr) {
	m_threadRunning = false;
	m_threadAsyncRunning = false;
}

jus::TcpString::TcpString() :
  m_connection(),
  m_thread(nullptr),
  m_observerElement(nullptr),
  m_observerRawElement(nullptr),
  m_threadAsync(nullptr) {
	m_threadRunning = false;
	m_threadAsyncRunning = false;
}

void jus::TcpString::setInterface(enet::Tcp _connection) {
	m_connection = std::move(_connection);
}

jus::TcpString::~TcpString() {
	disconnect();
}

void jus::TcpString::setInterfaceName(const std::string& _name) {
	ethread::setName(*m_thread, "Tcp-" + _name);
}

void jus::TcpString::threadCallback() {
	ethread::setName("TcpString-input");
	// get datas:
	while (    m_threadRunning == true
	        && m_connection.getConnectionStatus() == enet::Tcp::status::link) {
		// READ section data:
		if (m_observerElement != nullptr) {
			std::string data = std::move(read());
			JUS_VERBOSE("Receive data: '" << data << "'");
			if (data.size() != 0) {
				m_lastReceive = std::chrono::steady_clock::now();
				m_observerElement(std::move(data));
			}
		} else if (m_observerRawElement != nullptr) {
			jus::Buffer data = std::move(readRaw());
			if (data.size() != 0) {
				m_observerRawElement(std::move(data));
			}
		}
	}
	m_threadRunning = false;
	JUS_DEBUG("End of thread");
}

bool jus::TcpString::isActive() const {
	return m_threadRunning;
}

void jus::TcpString::connect(bool _async){
	JUS_DEBUG("connect [START]");
	m_threadRunning = true;
	m_thread = new std::thread([&](void *){ this->threadCallback();}, nullptr);
	if (m_thread == nullptr) {
		m_threadRunning = false;
		JUS_ERROR("creating callback thread!");
		return;
	}
	m_threadAsyncRunning = true;
	m_threadAsync = new std::thread([&](void *){ this->threadAsyncCallback();}, nullptr);
	if (m_threadAsync == nullptr) {
		m_threadAsyncRunning = false;
		JUS_ERROR("creating async sender thread!");
		return;
	}
	
	while (    _async == false
	        && m_threadRunning == true
	        && m_connection.getConnectionStatus() != enet::Tcp::status::link) {
		usleep(50000);
	}
	//ethread::setPriority(*m_receiveThread, -6);
	if (_async == true) {
		JUS_DEBUG("connect [STOP] async mode");
	} else {
		JUS_DEBUG("connect [STOP]");
	}
}

void jus::TcpString::disconnect(bool _inThreadStop){
	JUS_DEBUG("disconnect [START]");
	m_threadRunning = false;
	m_threadAsyncRunning = false;
	if (m_connection.getConnectionStatus() == enet::Tcp::status::link) {
		uint32_t size = 0xFFFFFFFF;
		m_connection.write(&size, 4);
	}
	if (m_connection.getConnectionStatus() != enet::Tcp::status::unlink) {
		m_connection.unlink();
	}
	if (m_threadAsync != nullptr) {
		m_threadAsync->join();
		delete m_threadAsync;
		m_threadAsync = nullptr;
	}
	if (_inThreadStop == false) {
		if (m_thread != nullptr) {
			m_thread->join();
			delete m_thread;
			m_thread = nullptr;
		}
	}
	JUS_DEBUG("disconnect [STOP]");
}

int32_t jus::TcpString::write(const std::string& _data) {
	if (m_threadRunning == false) {
		return -2;
	}
	if (_data.size() == 0) {
		return 0;
	}
	uint32_t size = _data.size();
	m_lastSend = std::chrono::steady_clock::now();
	m_connection.write(&size, 4);
	return m_connection.write(_data.c_str(), _data.size());
}

std::string jus::TcpString::read() {
	JUS_VERBOSE("Read [START]");
	if (m_threadRunning == false) {
		JUS_DEBUG("Read [END] Disconected");
		return "";
	}
	// TODO : Do it better with a correct way to check data size ...
	JUS_VERBOSE("Read [START]");
	std::string out;
	uint32_t size = 0;
	int32_t len = m_connection.read(&size, 4);
	if (len != 4) {
		JUS_ERROR("Protocol error occured ...");
	} else {
		if (size == -1) {
			JUS_WARNING("Remote close connection");
			m_threadRunning = false;
			//m_connection.unlink();
		} else {
			int64_t offset = 0;
			out.resize(size);
			while (offset != size) {
				len = m_connection.read(&out[offset], size-offset);
				offset += len;
				if (len == 0) {
					JUS_WARNING("Read No data");
					//break;
				}
				/*
				else if (size != offset) {
					JUS_ERROR("Protocol error occured .2. ==> concat (offset=" << offset << " size=" << size);
				}
				*/
			}
		}
	}
	JUS_VERBOSE("Read [STOP]");
	return out;
}


void jus::TcpString::threadAsyncCallback() {
	ethread::setName("Async-sender");
	// get datas:
	while (    m_threadAsyncRunning == true
	        && m_connection.getConnectionStatus() == enet::Tcp::status::link) {
		if (m_threadAsyncList.size() == 0) {
			usleep(10000);
			continue;
		}
		std::unique_lock<std::mutex> lock(m_threadAsyncMutex);
		auto it = m_threadAsyncList.begin();
		while (it != m_threadAsyncList.end()) {
			bool ret = (*it)(this);
			if (ret == true) {
				// Remove it ...
				it = m_threadAsyncList.erase(it);
			} else {
				++it;
			}
		}
	}
	m_threadRunning = false;
	JUS_DEBUG("End of thread");
}

