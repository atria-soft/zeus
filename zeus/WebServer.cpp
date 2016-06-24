/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#include <zeus/WebServer.h>
#include <zeus/debug.h>
#include <ethread/tools.h>
#include <unistd.h>


ememory::SharedPtr<zeus::Buffer> zeus::createBaseCall(uint64_t _transactionId, const std::string& _functionName, const uint32_t& _serviceId) {
	ememory::SharedPtr<zeus::Buffer> obj = zeus::Buffer::create();
	if (obj == nullptr) {
		return nullptr;
	}
	obj->setServiceId(_serviceId);
	obj->setCall(_functionName);
	obj->setTransactionId(_transactionId);
	return obj;
}

void zeus::createParam(int32_t _paramId, const ememory::SharedPtr<zeus::Buffer>& _obj) {
	// Finish recursive parse ...
}



zeus::WebServer::WebServer() :
  m_connection(),
  m_observerElement(nullptr),
  m_threadAsync(nullptr) {
	m_threadAsyncRunning = false;
	m_transmissionId = 1;
}

zeus::WebServer::WebServer(enet::Tcp _connection, bool _isServer) :
  m_connection(),
  m_observerElement(nullptr),
  m_threadAsync(nullptr) {
	m_threadAsyncRunning = false;
	m_transmissionId = 1;
	setInterface(std::move(_connection), _isServer);
}

void zeus::WebServer::setInterface(enet::Tcp _connection, bool _isServer) {
	m_connection.setInterface(std::move(_connection), _isServer);
	m_connection.connect(this, &zeus::WebServer::onReceiveData);
	if (_isServer == true) {
		m_connection.connectUri(this, &zeus::WebServer::onReceiveUri);
		m_connection.start();
	} else {
		std::vector<std::string> protocols;
		protocols.push_back("zeus/0.8");
		protocols.push_back("zeus/1.0");
		m_connection.start("/stupidName", protocols);
	}
}

zeus::WebServer::~WebServer() {
	disconnect();
}

void zeus::WebServer::setInterfaceName(const std::string& _name) {
	//ethread::setName(*m_thread, "Tcp-" + _name);
}


bool zeus::WebServer::isActive() const {
	return m_connection.isAlive();
}

void zeus::WebServer::connect(bool _async){
	ZEUS_DEBUG("connect [START]");
	m_threadAsyncRunning = true;
	m_threadAsync = new std::thread([&](void *){ this->threadAsyncCallback();}, nullptr);
	if (m_threadAsync == nullptr) {
		m_threadAsyncRunning = false;
		ZEUS_ERROR("creating async sender thread!");
		return;
	}
	
	while (    _async == false
	        && m_threadAsyncRunning == true
	        && m_connection.isAlive() != true) {
		usleep(50000);
	}
	//ethread::setPriority(*m_receiveThread, -6);
	if (_async == true) {
		ZEUS_DEBUG("connect [STOP] async mode");
	} else {
		ZEUS_DEBUG("connect [STOP]");
	}
}

void zeus::WebServer::disconnect(bool _inThreadStop){
	ZEUS_DEBUG("disconnect [START]");
	m_threadAsyncRunning = false;
	if (m_connection.isAlive() == true) {
		m_connection.controlClose();
	}
	m_connection.stop(_inThreadStop);
	if (m_threadAsync != nullptr) {
		m_threadAsync->join();
		delete m_threadAsync;
		m_threadAsync = nullptr;
	}
	ZEUS_DEBUG("disconnect [STOP]");
}

class SendAsyncBinary {
	private:
		std::vector<zeus::ActionAsyncClient> m_async;
		uint64_t m_transactionId;
		uint32_t m_serviceId;
		uint32_t m_partId;
	public:
		SendAsyncBinary(uint64_t _transactionId, const uint32_t& _serviceId, std::vector<zeus::ActionAsyncClient> _async) :
		  m_async(std::move(_async)),
		  m_transactionId(_transactionId),
		  m_serviceId(_serviceId),
		  m_partId(1) {
			
		}
		bool operator() (zeus::WebServer* _interface){
			auto it = m_async.begin();
			while (it != m_async.end()) {
				bool ret = (*it)(_interface, m_serviceId, m_transactionId, m_partId);
				if (ret == true) {
					// Remove it ...
					it = m_async.erase(it);
				} else {
					++it;
				}
				m_partId++;
			}
			if (m_async.size() == 0) {
				ememory::SharedPtr<zeus::Buffer> obj = zeus::Buffer::create();
				if (obj == nullptr) {
					return true;
				}
				obj->setType(zeus::Buffer::typeMessage::data);
				obj->setServiceId(m_serviceId);
				obj->setTransactionId(m_transactionId);
				obj->setPartId(m_partId);
				obj->setPartFinish(true);
				_interface->writeBinary(obj);
				return true;
			}
			return false;
		}
};

int32_t zeus::WebServer::writeBinary(const ememory::SharedPtr<zeus::Buffer>& _obj) {
	if (m_connection.isAlive() == false) {
		return -2;
	}
	if (_obj->haveAsync() == true) {
		_obj->setPartFinish(false);
	}
	if (_obj->writeOn(m_connection) == true) {
		if (_obj->haveAsync() == true) {
			addAsync(SendAsyncBinary(_obj->getTransactionId(), _obj->getServiceId(), std::move(_obj->moveAsync())));
		}
		return 1;
	}
	return -1;
}

bool zeus::WebServer::onReceiveUri(const std::string& _uri, const std::vector<std::string>& _protocols) {
	ZEUS_INFO("Receive Header uri: " << _uri);
	for (auto &it : _protocols) {
		if (it == "zeus/1.0") {
			m_connection.setProtocol(it);
			break;
		}
	}
	if (_uri == "/stupidName") {
		return true;
	}
	return false;
}

void zeus::WebServer::onReceiveData(std::vector<uint8_t>& _frame, bool _isBinary) {
	ememory::SharedPtr<zeus::Buffer> dataRaw = zeus::Buffer::create();
	if (_isBinary == true) {
		ZEUS_ERROR("Receive non binary frame ...");
		disconnect(true);
		return;
	}
	dataRaw->composeWith(_frame);
	newBuffer(dataRaw);
}

void zeus::WebServer::ping() {
	m_connection.controlPing();
}

void zeus::WebServer::newBuffer(const ememory::SharedPtr<zeus::Buffer>& _buffer) {
	ZEUS_VERBOSE("Receive :" << *_buffer);
	zeus::FutureBase future;
	uint64_t tid = _buffer->getTransactionId();
	if (tid == 0) {
		ZEUS_ERROR("Get a Protocol error ... No ID ...");
		/*
		if (obj["error"].toString().get() == "PROTOCOL-ERROR") {
			ZEUS_ERROR("Get a Protocol error ...");
			std::unique_lock<std::mutex> lock(m_mutex);
			for (auto &it : m_pendingCall) {
				if (it.isValid() == false) {
					continue;
				}
				it.appendData(obj);
			}
			m_pendingCall.clear();
		} else {
			ZEUS_ERROR("call with no ID ==> error ...");
		}
		*/
		return;
	}
	{
		std::unique_lock<std::mutex> lock(m_pendingCallMutex);
		auto it = m_pendingCall.begin();
		while (it != m_pendingCall.end()) {
			if (it->second.isValid() == false) {
				it = m_pendingCall.erase(it);
				continue;
			}
			if (it->second.getTransactionId() != tid) {
				++it;
				continue;
			}
			future = it->second;
			break;
		}
	}
	if (future.isValid() == false) {
		// not a pending call ==> simple event or call ...
		if (m_observerElement != nullptr) {
			m_observerElement(_buffer);
		}
		return;
	}
	bool ret = future.appendData(_buffer);
	if (ret == true) {
		std::unique_lock<std::mutex> lock(m_pendingCallMutex);
		auto it = m_pendingCall.begin();
		while (it != m_pendingCall.end()) {
			if (it->second.isValid() == false) {
				it = m_pendingCall.erase(it);
				continue;
			}
			if (it->second.getTransactionId() != tid) {
				++it;
				continue;
			}
			it = m_pendingCall.erase(it);
			break;
		}
	}
}

void zeus::WebServer::addAsync(zeus::WebServer::ActionAsync _elem) {
	std::unique_lock<std::mutex> lock(m_threadAsyncMutex);
	m_threadAsyncList2.push_back(_elem);
}

void zeus::WebServer::threadAsyncCallback() {
	ethread::setName("Async-sender");
	ZEUS_INFO("Async Sender [START]...");
	// get datas:
	while (    m_threadAsyncRunning == true
	        && m_connection.isAlive() == true) {
		if (m_threadAsyncList2.size() != 0) {
			std::unique_lock<std::mutex> lock(m_threadAsyncMutex);
			for (auto &it : m_threadAsyncList2) {
				m_threadAsyncList.push_back(it);
			}
			m_threadAsyncList2.clear();
		}
		if (m_threadAsyncList.size() == 0) {
			usleep(10000);
			continue;
		}
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
	m_threadAsyncRunning = false;
	ZEUS_INFO("Async Sender [STOP]");
}


zeus::FutureBase zeus::WebServer::callBinary(uint64_t _transactionId,
                                             const ememory::SharedPtr<zeus::Buffer>& _obj,
                                             zeus::FutureData::ObserverFinish _callback,
                                             const uint32_t& _serviceId) {
	ZEUS_VERBOSE("Send [START] ");
	if (isActive() == false) {
		ememory::SharedPtr<zeus::Buffer> obj = zeus::Buffer::create();
		obj->setType(zeus::Buffer::typeMessage::answer);
		obj->addError("NOT-CONNECTED", "Client interface not connected (no TCP)");
		return zeus::FutureBase(_transactionId, obj, _callback);
	}
	zeus::FutureBase tmpFuture(_transactionId, _callback);
	{
		std::unique_lock<std::mutex> lock(m_pendingCallMutex);
		m_pendingCall.push_back(std::make_pair(uint64_t(0), tmpFuture));
	}
	writeBinary(_obj);
	ZEUS_VERBOSE("Send [STOP]");
	return tmpFuture;
}

zeus::FutureBase zeus::WebServer::callForward(uint32_t _clientId,
                                              const ememory::SharedPtr<zeus::Buffer>& _buffer,
                                              uint64_t _singleReferenceId,
                                              zeus::FutureData::ObserverFinish _callback) {
	ZEUS_VERBOSE("Call Forward [START]");
	//zeus::FutureBase ret = callBinary(id, _Buffer, async, _callback);
	//ret.setSynchronous();
	
	if (isActive() == false) {
		ememory::SharedPtr<zeus::Buffer> obj = zeus::Buffer::create();
		obj->setType(zeus::Buffer::typeMessage::answer);
		obj->addError("NOT-CONNECTED", "Client interface not connected (no TCP)");
		return zeus::FutureBase(0, obj, _callback);
	}
	uint64_t id = getId();
	_buffer->setTransactionId(id);
	_buffer->setClientId(_clientId);
	zeus::FutureBase tmpFuture(id, _callback);
	tmpFuture.setSynchronous();
	{
		std::unique_lock<std::mutex> lock(m_pendingCallMutex);
		m_pendingCall.push_back(std::make_pair(_singleReferenceId, tmpFuture));
	}
	writeBinary(_buffer);
	ZEUS_VERBOSE("Send Forward [STOP]");
	return tmpFuture;
}

void zeus::WebServer::callForwardMultiple(uint32_t _clientId,
                                          const ememory::SharedPtr<zeus::Buffer>& _buffer,
                                          uint64_t _singleReferenceId){
	if (_buffer == nullptr) {
		return;
	}
	// subMessage ... ==> try to forward message:
	std::unique_lock<std::mutex> lock(m_pendingCallMutex);
	for (auto &itCall : m_pendingCall) {
		ZEUS_INFO(" compare : " << itCall.first << " =?= " << _singleReferenceId);
		if (itCall.first == _singleReferenceId) {
			// Find element ==> transit it ...
			_buffer->setTransactionId(itCall.second.getTransactionId());
			_buffer->setClientId(_clientId);
			writeBinary(_buffer);
			return;
		}
	}
	ZEUS_ERROR("Can not transfer part of a message ...");
}

void zeus::WebServer::answerError(uint64_t _clientTransactionId, const std::string& _errorValue, const std::string& _errorHelp, uint32_t _clientId) {
	ememory::SharedPtr<zeus::Buffer> answer = zeus::Buffer::create();
	if (answer == nullptr) {
		return;
	}
	answer->setType(zeus::Buffer::typeMessage::answer);
	answer->setTransactionId(_clientTransactionId);
	answer->setClientId(_clientId);
	answer->addError(_errorValue, _errorHelp);
	writeBinary(answer);
}


void zeus::WebServer::answerVoid(uint64_t _clientTransactionId, uint32_t _clientId) {
	ememory::SharedPtr<zeus::Buffer> answer = zeus::Buffer::create();
	if (answer == nullptr) {
		return;
	}
	answer->setType(zeus::Buffer::typeMessage::answer);
	answer->setTransactionId(_clientTransactionId);
	answer->setClientId(_clientId);
	answer->addParameter();
	writeBinary(answer);
}