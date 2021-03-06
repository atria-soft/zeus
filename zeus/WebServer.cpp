/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#include <zeus/WebServer.hpp>
#include <zeus/debug.hpp>
#include <zeus/ObjectRemote.hpp>
#include <ethread/tools.hpp>

#include <zeus/message/Data.hpp>


#include <etk/typeInfo.hpp>
ETK_DECLARE_TYPE(zeus::WebServer);


ememory::SharedPtr<zeus::message::Call> zeus::createBaseCall(const ememory::SharedPtr<zeus::WebServer>& _iface,
                                                             uint64_t _transactionId,
                                                             const uint32_t& _source,
                                                             const uint32_t& _destination,
                                                             const etk::String& _functionName) {
	ememory::SharedPtr<zeus::message::Call> obj;
	obj = zeus::message::Call::create(_iface);
	if (obj == null) {
		return null;
	}
	obj->setSource(_source);
	obj->setDestination(_destination);
	obj->setCall(_functionName);
	obj->setTransactionId(_transactionId);
	return obj;
}

void zeus::createParam(int32_t _paramId,
                       ememory::SharedPtr<zeus::message::Call> _obj) {
	// Finish recursive parse ...
}


static uint32_t interfaceId = 1;


uint16_t zeus::WebServer::getId() {
	uint16_t tmp = m_transmissionId++;
	if (m_transmissionId == 0) {
		// TODO: correct this bug ...
		ZEUS_ERROR("NOW you can have error on signals ...");
		m_transmissionId++;
	}
	return tmp;
}

zeus::WebServer::WebServer() :
  m_connection(),
  m_processingPool(20),
  m_localAddress(0),
  m_localIdObjectIncrement(1),
  m_interfaceId(0),
  m_transmissionId(1),
  m_observerElement(null),
  m_threadAsync(null) {
	m_interfaceId = interfaceId++;
	m_threadAsyncRunning = false;
	
}

zeus::WebServer::WebServer(enet::Tcp _connection, bool _isServer) :
  m_connection(),
  m_processingPool(20),
  m_localAddress(0),
  m_localIdObjectIncrement(1),
  m_interfaceId(0),
  m_transmissionId(1),
  m_observerElement(null),
  m_threadAsync(null) {
	m_interfaceId = interfaceId++;
	m_threadAsyncRunning = false;
	setInterface(etk::move(_connection), _isServer);
}

void zeus::WebServer::setInterface(enet::Tcp _connection, bool _isServer, const etk::String& _userName) {
	m_connection.setInterface(etk::move(_connection), _isServer);
	m_connection.connect(this, &zeus::WebServer::onReceiveData);
	if (_isServer == true) {
		m_connection.connectUri(this, &zeus::WebServer::onReceiveUri);
		m_connection.start();
	} else {
		etk::Vector<etk::String> protocols;
		protocols.pushBack("zeus/0.8");
		protocols.pushBack("zeus/1.0");
		m_connection.start("/" + _userName, protocols);
	}
}

zeus::WebServer::~WebServer() {
	ZEUS_WARNING("destroy WebServer ...  [START]");
	ZEUS_INFO("disconnect ...");
	disconnect();
	ZEUS_WARNING("disconnect (done) ");
	ZEUS_INFO("stop thread pool ...");
	m_processingPool.stop();
	ZEUS_INFO("stop thread pool (done)");
	ethread::sleepMilliSeconds((100)); // TODO : Remove this ...
	ZEUS_INFO("stop thread join ...");
	m_processingPool.join();
	ZEUS_INFO("stop thread join (done)");
	ZEUS_WARNING("destroy WebServer ...  [STOP]");
}

void zeus::WebServer::setInterfaceName(const etk::String& _name) {
	//ethread::setName(*m_thread, "Tcp-" + _name);
}

void zeus::WebServer::addWebObj(ememory::SharedPtr<zeus::WebObj> _obj) {
	ethread::UniqueLock lock(m_listObjectMutex);
	m_listObject.pushBack(_obj);
}

void zeus::WebServer::addWebObjRemote(ememory::SharedPtr<zeus::ObjectRemoteBase> _obj) {
	ethread::UniqueLock lock(m_listRemoteObjectMutex);
	m_listRemoteObject.pushBack(_obj);
}

void zeus::WebServer::interfaceRemoved(etk::Vector<uint16_t> _list) {
	ZEUS_WARNING("Remove interface : " << _list);
	for (int32_t iii=0; iii < _list.size(); ++iii) {
		ethread::UniqueLock lock(m_listRemoteObjectMutex);
		// Call All remote Object object
		for (auto it=m_listRemoteObject.begin();
		    it != m_listRemoteObject.end();
		    /* no increment */) {
			ememory::SharedPtr<zeus::ObjectRemoteBase> tmp = it->lock();
			if (tmp == null) {
				it = m_listRemoteObject.erase(it);
				continue;
			}
			if (tmp->getRemoteInterfaceId() == _list[iii]) {
				tmp->setRemoteObjectRemoved();
				it = m_listRemoteObject.erase(it);
				continue;
			}
			++it;
		}
	}
	for (int32_t iii=0; iii < _list.size(); ++iii) {
		ethread::UniqueLock lock(m_listObjectMutex);
		// Call All remote Object object
		for (auto it=m_listObject.begin();
		    it != m_listObject.end();
		    /* no increment */) {
			if (*it == null) {
				it = m_listObject.erase(it);
				continue;
			}
			(*it)->rmRemoteInterface(_list[iii]);
			++it;
		}
	}
	for (int32_t iii=0; iii < _list.size(); ++iii) {
		ethread::UniqueLock lock(m_pendingCallMutex);
		auto it = m_pendingCall.begin();
		while (it != m_pendingCall.end()) {
			if (it->second.isValid() == false) {
				it = m_pendingCall.erase(it);
				continue;
			}
			if (it->second.getSource()>>16 != _list[iii]) {
				++it;
				continue;
			}
			it->second.remoteObjectDestroyed();
			it = m_pendingCall.erase(it);
		}
	}
}


bool zeus::WebServer::isActive() const {
	return m_connection.isAlive();
}

void zeus::WebServer::connect(bool _async){
	ZEUS_DEBUG("connect [START]");
	m_threadAsyncRunning = true;
	m_threadAsync = ETK_NEW(ethread::Thread, [&](){ threadAsyncCallback();}, "webServerAsync");
	if (m_threadAsync == null) {
		m_threadAsyncRunning = false;
		ZEUS_ERROR("creating async sender thread!");
		return;
	}
	while (    _async == false
	        && m_threadAsyncRunning == true
	        && m_connection.isAlive() != true) {
		ethread::sleepMilliSeconds((50));
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
	if (m_threadAsync != null) {
		m_threadAsync->join();
		ETK_DELETE(ethread::Thread, m_threadAsync);
		m_threadAsync = null;
	}
	ZEUS_DEBUG("disconnect [STOP]");
}

class SendAsyncBinary {
	private:
		etk::Vector<zeus::ActionAsyncClient> m_async;
		uint64_t m_transactionId;
		uint32_t m_source;
		uint32_t m_destination;
		uint32_t m_partId;
	public:
		SendAsyncBinary(uint64_t _transactionId, const uint32_t& _source, const uint32_t& _destination, etk::Vector<zeus::ActionAsyncClient> _async) :
		  m_async(etk::move(_async)),
		  m_transactionId(_transactionId),
		  m_source(_source),
		  m_destination(_destination),
		  m_partId(1) {
			
		}
		// TODO : Use shared ptr instaed of pointer ....
		bool operator() (zeus::WebServer* _interface) {
			auto it = m_async.begin();
			while (it != m_async.end()) {
				bool ret = (*it)(_interface, m_source, m_destination, m_transactionId, m_partId);
				if (ret == true) {
					// Remove it ...
					it = m_async.erase(it);
				} else {
					++it;
				}
				m_partId++;
			}
			if (m_async.size() == 0) {
				ememory::SharedPtr<zeus::message::Data> obj = zeus::message::Data::create(_interface->sharedFromThis());
				if (obj == null) {
					return true;
				}
				//obj->setInterfaceId(m_interfaceId);
				obj->setSource(m_source);
				obj->setDestination(m_destination);
				obj->setTransactionId(m_transactionId);
				obj->setPartId(m_partId);
				obj->setPartFinish(true);
				_interface->writeBinary(obj);
				return true;
			}
			return false;
		}
};

//#define ZEUS_LOG_INPUT_OUTPUT ZEUS_WARNING
#define ZEUS_LOG_INPUT_OUTPUT ZEUS_VERBOSE


int32_t zeus::WebServer::writeBinary(ememory::SharedPtr<zeus::Message> _obj) {
	if (m_connection.isAlive() == false) {
		return -2;
	}
	if (_obj->haveAsync() == true) {
		_obj->setPartFinish(false);
	}
	_obj->setInterfaceId(m_interfaceId);
	ZEUS_LOG_INPUT_OUTPUT("Send    :" << _obj);
	if (_obj->writeOn(m_connection) == true) {
		if (_obj->haveAsync() == true) {
			addAsync(SendAsyncBinary(_obj->getTransactionId(),
			                         _obj->getSource(),
			                         _obj->getDestination(),
			                         etk::move(_obj->moveAsync())));
		}
		return 1;
	}
	return -1;
}

etk::String zeus::WebServer::onReceiveUri(const etk::String& _uri, const etk::Vector<etk::String>& _protocols) {
	ZEUS_INFO("Receive Header uri: " << _uri);
	bool findProtocol = false;
	for (auto &it : _protocols) {
		if (it == "zeus/1.0") {
			m_connection.setProtocol(it);
			findProtocol = true;
			break;
		}
	}
	if (findProtocol == false) {
		ZEUS_ERROR("Disable connection request URI='" << _uri << "' with wrong protocol:" << _protocols);
		return "CLOSE";
	}
	// TODO : Add better return on specific user ...
	if (m_observerRequestUri != null) {
		etk::Map<etk::String,etk::String> options;
		etk::String uri;
		size_t pos = _uri.find('?');
		if (pos == etk::String::npos) {
			uri = _uri;
		} else {
			uri = _uri.extract(0, pos);
			etk::String opt = _uri.extract(pos+1, _uri.size());
			etk::Vector<etk::String> listValue = opt.split('&');
			for (auto &it: listValue) {
				pos = it.find('=');
				if (pos == etk::String::npos) {
					options.set(it, "");
				} else {
					options.set(it.extract(0,pos), it.extract(pos+1, it.size()));
				}
			}
		}
		return m_observerRequestUri(uri, options);
	}
	if (_uri == "/") {
		return "OK";
	}
	ZEUS_ERROR("Disable connection all time the uri is not accepted by the server if the URI is not '/' URI='" << _uri << "'");
	return "CLOSE";
}

void zeus::WebServer::onReceiveData(etk::Vector<uint8_t>& _frame, bool _isBinary) {
	if (_isBinary == true) {
		ZEUS_ERROR("Receive non binary frame ...");
		disconnect(true);
		return;
	}
	ememory::SharedPtr<zeus::Message> dataRaw = zeus::Message::create(sharedFromThis(), _frame);
	if (dataRaw == null) {
		ZEUS_ERROR("Message Allocation ERROR ... ");
		disconnect(true);
		return;
	}
	if (    m_localAddress != 0
	     && dataRaw->getSource() == 0
	     && dataRaw->getDestination() == 0) {
		ZEUS_ERROR("Protocol ERROR ... " << dataRaw);
		disconnect(true);
		return;
	}
	dataRaw->setInterfaceId(m_interfaceId);
	newMessage(dataRaw);
}

void zeus::WebServer::ping() {
	ZEUS_WARNING("send PING");
	m_connection.controlPing();
}

void zeus::WebServer::newMessage(ememory::SharedPtr<zeus::Message> _buffer) {
	ZEUS_LOG_INPUT_OUTPUT("Receive :" << _buffer);
	// if an adress id different ... just transmit it ... 
	if (m_localAddress != _buffer->getDestinationId()) {
		// TODO : Change the callback ...
		if (m_observerElement != null) {
			m_processingPool.async(
			    [=](){
			    	// not a pending call ==> simple event or call ...
			    	m_observerElement(_buffer); //!< all input arrive at the same element
			    },
			    8);
		}
		return;
	}
	if (    _buffer->getPartFinish() == false
	     && _buffer->getType() != zeus::message::type::data) {
		//ethread::UniqueLock lock(m_mutex);
		m_listPartialMessage.pushBack(_buffer);
		return;
	}
	if (_buffer->getType() == zeus::message::type::data) {
		// Add data in a previous buffer...
		//ethread::UniqueLock lock(m_mutex);
		auto it = m_listPartialMessage.begin();
		while (it != m_listPartialMessage.end()) {
			if (*it == null) {
				it = m_listPartialMessage.erase(it);
				continue;
			}
			if ((*it)->getDestination() != _buffer->getDestination()) {
				++it;
				continue;
			}
			if ((*it)->getTransactionId() == _buffer->getTransactionId()) {
				(*it)->appendMessage(_buffer);
				if (_buffer->getPartFinish() != true) {
					return;
				}
				(*it)->setPartFinish(true);
				_buffer = *it;
				it = m_listPartialMessage.erase(it);
				break;
			}
		}
	}
	if (_buffer->getPartFinish() != true) {
		ZEUS_ERROR("Get a buffer with no finished data ... (remove)" << _buffer);
		return;
	}
	
	// Try to find in the current call that has been done to add data in an answer :
	zeus::FutureBase future;
	uint64_t tid = _buffer->getTransactionId();
	// TODO : Check the UDI reaaly utility ...
	if (    _buffer->getType() == zeus::message::type::answer
	     || _buffer->getType() == zeus::message::type::data
	     || _buffer->getType() == zeus::message::type::event) {
		ethread::UniqueLock lock(m_pendingCallMutex);
		auto it = m_pendingCall.begin();
		while (it != m_pendingCall.end()) {
			if (it->second.isValid() == false) {
				it = m_pendingCall.erase(it);
				ZEUS_WARNING("Remove element that have wrong data...");
				continue;
			}
			if (it->second.getTransactionId() != tid) {
				++it;
				continue;
			}
			ZEUS_LOG_INPUT_OUTPUT("Find FUTURE ..." << _buffer);
			future = it->second;
			break;
		}
	}
	// Not find a pending call ==> execute it ...
	if (future.isValid() == false) {
		uint32_t dest = _buffer->getDestination();
		{
			ethread::UniqueLock lock(m_listObjectMutex);
			// Call local object
			for (auto &it : m_listObject) {
				if (it == null) {
					continue;
				}
				if (it->getFullId() == dest) {
					// send in an other async to syncronize the 
					m_processingPool.async(
					    [=](){
					    	ememory::SharedPtr<zeus::WebObj> tmpObj = it;
					    	ZEUS_LOG_INPUT_OUTPUT("PROCESS :  " << _buffer);
					    	tmpObj->receive(_buffer);
					    },
					    dest
					    );
					return;
				}
			}
		}
		{
			ethread::UniqueLock lock(m_listRemoteObjectMutex);
			// call local map object on remote object
			for (auto &it : m_listRemoteObject) {
				ememory::SharedPtr<zeus::ObjectRemoteBase> tmp = it.lock();
				if (tmp == null) {
					continue;
				}
				if (tmp->getFullId() == dest) {
					// send in an other async to syncronize the 
					m_processingPool.async(
					    [=](){
					    	ememory::SharedPtr<zeus::WebObj> tmpObj = tmp;
					    	ZEUS_LOG_INPUT_OUTPUT("PROCESS :  " << _buffer);
					    	tmpObj->receive(_buffer);
					    },
					    dest
					    );
					return;
				}
			}
		}
		if (m_observerElement != null) {
			m_processingPool.async(
			    [=](){
			    	// not a pending call ==> simple event or call ...
			    	m_observerElement(_buffer); //!< all input arrive at the same element
			    },
			    9);
		}
		return;
	}
	ZEUS_LOG_INPUT_OUTPUT("Add in puul to PROCESS FUTURE :  " << _buffer);
	m_processingPool.async(
	    [=](){
	    	zeus::FutureBase fut = future;
	    	ZEUS_LOG_INPUT_OUTPUT("PROCESS FUTURE :  " << _buffer);
	    	// add data ...
	    	bool ret = fut.setMessage(_buffer);
	    	if (ret == true) {
	    		ZEUS_LOG_INPUT_OUTPUT("    ==> start LOCK");
	    		ethread::UniqueLock lock(m_pendingCallMutex);
	    		ZEUS_LOG_INPUT_OUTPUT("    ==>       LOCK done");
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
	    		ZEUS_LOG_INPUT_OUTPUT("    ==>  end  LOCK");
	    	} else {
	    		ZEUS_LOG_INPUT_OUTPUT("    ==>  end");
	    	}
	    },
	    tid); // force at the transaction Id to have a correct order in the processing of the data ...
	
}

void zeus::WebServer::listObjects() {
	if (    m_listObject.size() == 0
	     && m_listRemoteObject.size() == 0) {
		return;
	}
	ZEUS_DEBUG("[" << m_interfaceId << "] Interface WebServer:");
	{
		ethread::UniqueLock lock(m_listObjectMutex);
		for (auto &it : m_listObject) {
			if (it == null) {
				continue;
			}
			it->display();
		}
	}
	{
		ethread::UniqueLock lock(m_listRemoteObjectMutex);
		for (auto &it : m_listRemoteObject) {
			ememory::SharedPtr<zeus::ObjectRemoteBase> tmpp = it.lock();
			if (tmpp == null) {
				continue;
			}
			tmpp->display();
		}
	}
}

void zeus::WebServer::cleanDeadObject() {
	//ethread::UniqueLock lock(m_mutex);
	if (    m_listObject.size() == 0
	     && m_listRemoteObject.size() == 0) {
		return;
	}
	ZEUS_INFO("Clean DEAD object... total=" << m_listObject.size());
	etk::Vector<ememory::SharedPtr<zeus::WebObj>> tmpToRemoveObjectInAsync;
	{
		ethread::UniqueLock lock(m_listObjectMutex);
		for (auto it=m_listObject.begin();
		     it!=m_listObject.end();
		     /* no auto increment*/) {
			if (*it == null) {
				it = m_listObject.erase(it);
				continue;
			}
			if ((*it)->haveRemoteConnected() == false) {
				tmpToRemoveObjectInAsync.pushBack(*it);
				it = m_listObject.erase(it);
				continue;
			}
			++it;
		}
	}
	tmpToRemoveObjectInAsync.clear();
	{
		ethread::UniqueLock lock(m_listRemoteObjectMutex);
		for (auto it=m_listRemoteObject.begin();
		     it!=m_listRemoteObject.end();
		     /* no auto increment*/) {
			if (it->expired() == true) {
				it = m_listRemoteObject.erase(it);
				continue;
			}
			++it;
		}
	}
}

bool zeus::WebServer::transferRemoteObjectOwnership(uint16_t _objectAddress, uint32_t _sourceAddress, uint32_t _destinataireAddress) {
	//ethread::UniqueLock lock(m_mutex);
	if (    m_listObject.size() == 0
	     && m_listRemoteObject.size() == 0) {
		return false;
	}
	{
		ethread::UniqueLock lock(m_listObjectMutex);
		for (auto &it : m_listObject) {
			if (it == null) {
				continue;
			}
			if (it->getObjectId() == _objectAddress) {
				return it->transferOwnership(_sourceAddress, _destinataireAddress);
			}
		}
	}
	{
		ethread::UniqueLock lock(m_listRemoteObjectMutex);
		for (auto &it : m_listRemoteObject) {
			ememory::SharedPtr<zeus::ObjectRemoteBase> tmp = it.lock();
			if (tmp == null) {
				continue;
			}
			if (tmp->getObjectId() == _objectAddress) {
				ZEUS_ERROR("return a remote Object is not permited ... ==> link directly to the original elements");
				return false;
			}
		}
	}
	return false;
}

bool zeus::WebServer::removeObjectOwnership(uint16_t _objectAddress, uint32_t _sourceAddress) {
	//ethread::UniqueLock lock(m_mutex);
	if (    m_listObject.size() == 0
	     && m_listRemoteObject.size() == 0) {
		return false;
	}
	{
		ethread::UniqueLock lock(m_listObjectMutex);
		for (auto &it : m_listObject) {
			if (it == null) {
				continue;
			}
			ZEUS_INFO("1 Remove ownership of " << it->getObjectId() << " == " << _objectAddress);
			if (it->getObjectId() == _objectAddress) {
				return it->removeOwnership(_sourceAddress);
			}
		}
	}
	{
		ethread::UniqueLock lock(m_listRemoteObjectMutex);
		for (auto &it : m_listRemoteObject) {
			ememory::SharedPtr<zeus::ObjectRemoteBase> tmp = it.lock();
			if (tmp == null) {
				continue;
			}
			ZEUS_INFO("2 Remove ownership of " << tmp->getObjectId() << " == " << _objectAddress);
			if (tmp->getObjectId() == _objectAddress) {
				ZEUS_ERROR("return a remote Object is not permited ... ==> link directly to the original elements");
				return false;
			}
		}
	}
	return false;
}

void zeus::WebServer::addAsync(zeus::WebServer::ActionAsync _elem) {
	ethread::UniqueLock lock(m_threadAsyncMutex);
	m_threadAsyncList2.pushBack(_elem);
	ZEUS_DEBUG("ADD element to send ... " << m_threadAsyncList2.size());
}

void zeus::WebServer::threadAsyncCallback() {
	ethread::setName("Async-sender");
	ZEUS_INFO("Async Sender [START]...");
	// get datas:
	while (    m_threadAsyncRunning == true
	        && m_connection.isAlive() == true) {
		if (m_threadAsyncList2.size() != 0) {
			ethread::UniqueLock lock(m_threadAsyncMutex);
			for (auto &it : m_threadAsyncList2) {
				ZEUS_DEBUG("Add async");
				m_threadAsyncList.pushBack(it);
			}
			m_threadAsyncList2.clear();
		}
		if (m_threadAsyncList.size() == 0) {
			ethread::sleepMilliSeconds((10));
			continue;
		}
		auto it = m_threadAsyncList.begin();
		while (it != m_threadAsyncList.end()) {
			ZEUS_VERBOSE(" send DATA ... ");
			bool ret = (*it)(this);
			if (ret == true) {
				// Remove it ...
				ZEUS_DEBUG("Remove async");
				it = m_threadAsyncList.erase(it);
			} else {
				++it;
			}
		}
	}
	m_threadAsyncRunning = false;
	ZEUS_INFO("Async Sender [STOP]");
}


zeus::FutureBase zeus::WebServer::callBinary(ememory::SharedPtr<zeus::Message> _obj) {
	if (isActive() == false) {
		ZEUS_ERROR("Send [STOP] ==> not connected (no TCP)");
		ememory::SharedPtr<zeus::message::Answer> obj = zeus::message::Answer::create(sharedFromThis());
		obj->addError("NOT-CONNECTED", "Client interface not connected (no TCP)");
		return zeus::FutureBase(_obj->getTransactionId(), obj);
	}
	zeus::FutureBase tmpFuture(_obj->getTransactionId());
	{
		ethread::UniqueLock lock(m_pendingCallMutex);
		m_pendingCall.pushBack(etk::makePair(uint64_t(0), tmpFuture));
	}
	writeBinary(_obj);
	return tmpFuture;
}

void zeus::WebServer::answerError(uint32_t _clientTransactionId, uint32_t _source, uint32_t _destination, const etk::String& _errorValue, const etk::String& _errorHelp) {
	auto answer = zeus::message::Answer::create(sharedFromThis());
	if (answer == null) {
		return;
	}
	answer->setTransactionId(_clientTransactionId);
	answer->setSource(_source);
	answer->setDestination(_destination);
	answer->addError(_errorValue, _errorHelp);
	writeBinary(answer);
}

void zeus::WebServer::answerVoid(uint32_t _clientTransactionId, uint32_t _source, uint32_t _destination) {
	auto answer = zeus::message::Answer::create(sharedFromThis());
	if (answer == null) {
		return;
	}
	answer->setTransactionId(_clientTransactionId);
	answer->setSource(_source);
	answer->setDestination(_destination);
	answer->addParameter();
	writeBinary(answer);
}
