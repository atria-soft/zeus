/** @file
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <etk/types.hpp>
#include <zeus/Buffer.hpp>
#include <functional>
#include <ememory/memory.hpp>


namespace zeus {
	class FutureBase;
	/**
	 * @brief Data interface of the future (the future can be copied, but the data need to stay...
	 */
	class FutureData {
		public:
			//using ObserverFinish = std::function<bool(zeus::FutureBase)>; //!< Define an Observer: function pointer
			using Observer = std::function<bool(zeus::FutureBase)>; //!< Define an Observer: function pointer
		public:
			uint32_t m_transactionId; //!< waiting answer data
			uint32_t m_source; //!< Source of the message.
			bool m_isSynchronous; //!< the future is synchronous. (call when receive data)
			ememory::SharedPtr<zeus::Buffer> m_returnData; //!< all buffer concatenate or last buffer if synchronous
			Observer m_callbackThen; //!< observer callback When data arrive and NO error appear
			Observer m_callbackElse; //!< observer callback When data arrive and AN error appear
			//Observer m_callbackAbort; //!< observer callback When Action is abort by user
			std::chrono::steady_clock::time_point m_sendTime; //!< time when the future has been sended request
			std::chrono::steady_clock::time_point m_receiveTime; //!< time when the future has receve answer
	};
}

