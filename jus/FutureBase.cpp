/** @file
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#include <jus/FutureBase.h>
#include <jus/debug.h>
#include <unistd.h>

jus::FutureBase::FutureBase(const jus::FutureBase& _base):
  m_data(_base.m_data) {
	
}

jus::FutureBase::FutureBase() {
	m_data = nullptr;
}

jus::FutureBase::FutureBase(uint64_t _transactionId, jus::FutureData::ObserverFinish _callback) {
	m_data = std::make_shared<jus::FutureData>();
	if (m_data == nullptr) {
		return;
	}
	m_data->m_transactionId = _transactionId;
	m_data->m_isFinished = false;
	m_data->m_callbackFinish = _callback;
}

ejson::Object jus::FutureBase::getRaw() {
	if (m_data == nullptr) {
		return ejson::Object();
	}
	return m_data->m_returnData;
}

jus::FutureBase::FutureBase(uint64_t _transactionId, bool _isFinished, ejson::Object _returnData, jus::FutureData::ObserverFinish _callback) {
	m_data = std::make_shared<jus::FutureData>();
	if (m_data == nullptr) {
		return;
	}
	m_data->m_transactionId = _transactionId;
	m_data->m_isFinished = _isFinished;
	m_data->m_returnData = _returnData;
	m_data->m_callbackFinish = _callback;
	if (    m_data->m_isFinished == true
	     && m_data->m_callbackFinish != nullptr) {
		m_data->m_callbackFinish(*this);
	}
}

jus::FutureBase jus::FutureBase::operator= (const jus::FutureBase& _base) {
	m_data = _base.m_data;
	return *this;
}

void jus::FutureBase::setAnswer(const ejson::Object& _returnValue) {
	if (m_data == nullptr) {
		JUS_ERROR(" Not a valid future ...");
		return;
	}
	m_data->m_returnData = _returnValue;
	m_data->m_isFinished = true;
	if (m_data->m_callbackFinish != nullptr) {
		m_data->m_callbackFinish(*this);
	}
}

uint64_t jus::FutureBase::getTransactionId() {
	if (m_data == nullptr) {
		return 0;
	}
	return m_data->m_transactionId;
}

bool jus::FutureBase::hasError() {
	if (m_data == nullptr) {
		return true;
	}
	return m_data->m_returnData.valueExist("error");
}

std::string jus::FutureBase::getErrorType() {
	if (m_data == nullptr) {
		return "NULL_PTR";
	}
	return m_data->m_returnData["error"].toString().get();
}

std::string jus::FutureBase::getErrorHelp() {
	if (m_data == nullptr) {
		return "Thsi is a nullptr future";
	}
	return m_data->m_returnData["error-help"].toString().get();
}

bool jus::FutureBase::isValid() {
	return m_data != nullptr;
}

bool jus::FutureBase::isFinished() {
	if (m_data == nullptr) {
		return true;
	}
	return m_data->m_isFinished;
}

jus::FutureBase& jus::FutureBase::wait() {
	while (isFinished() == false) {
		// TODO : Do it better ... like messaging/mutex_locked ...
		usleep(10000);
	}
	return *this;
}

jus::FutureBase& jus::FutureBase::waitFor(std::chrono::microseconds _delta) {
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	while (    std::chrono::steady_clock::now() - start < _delta
	        && isFinished() == false) {
		// TODO : Do it better ... like messaging/mutex_locked ...
		usleep(10000);
		start = std::chrono::steady_clock::now();
	}
	return *this;
}

jus::FutureBase& jus::FutureBase::waitUntil(std::chrono::steady_clock::time_point _endTime) {
	while (    std::chrono::steady_clock::now() < _endTime
	        && isFinished() == false) {
		// TODO : Do it better ... like messaging/mutex_locked ...
		usleep(10000);
	}
	return *this;
}

