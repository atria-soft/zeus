/** @file
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <zeus/FutureData.hpp>

namespace zeus {
	/**
	 * @brief Generic zeus Future interface to get data asynchronously
	 */
	class FutureBase {
		public:
			ememory::SharedPtr<zeus::FutureData> m_data; // Reference on the data
		public:
			/**
			 * @brief Copy contructor of a FutureBase
			 * @param[in] _base the FutureBase to copy
			 */
			FutureBase(const zeus::FutureBase& _base);
			/**
			 * @brief contructor of a FutureBase
			 */
			FutureBase();
			/**
			 * @brief Contructor of the FutureBase with an ofserver
			 * @param[in] _transactionId Transaction waiting answer
			 * @param[in] _source Client/sevice Id waiting answer
			 */
			FutureBase(uint32_t _transactionId, uint32_t _source=0);
			/**
			 * @brief Contructor of the FutureBase for direct error answer
			 * @param[in] _transactionId Transaction waiting answer
			 * @param[in] _isFinished set state finish or not
			 * @param[in] _returnData Set return value
			 * @param[in] _source Source that is waiting for answer
			 */
			FutureBase(uint32_t _transactionId, ememory::SharedPtr<zeus::Buffer> _returnData, uint32_t _source=0);
			/**
			 * @brief Attach callback on all return type of value
			 * @param[in] _callback Handle on the function to call in all case
			 */
			void andAll(zeus::FutureData::Observer _callback);
			/**
			 * @brief Attach callback on a specific return action (SUCESS)
			 * @param[in] _callback Handle on the function to call in case of sucess on the call
			 */
			void andThen(zeus::FutureData::Observer _callback);
			/**
			 * @brief Attach callback on a specific return action (ERROR)
			 * @param[in] _callback Handle on the function to call in case of error on the call
			 */
			void andElse(zeus::FutureData::Observer _callback);
			/*
			/ **
			 * @brief Attach callback on a specific return action (ABORT)
			 * @param[in] _callback Handle on the function to call in case of abort on the call
			 * /
			void andAbort(zeus::FutureData::Observer _callback); // an abort is  requested in the actiron ...
			*/
			/**
			 * @brief Asignement operator with an other future
			 * @param[in] _base Generic base Future
			 * @return the reference on the local element
			 */
			zeus::FutureBase operator= (const zeus::FutureBase& _base);
			/**
			 * @brief Add data on the call/answer
			 * @param[in] _returnValue Returned buffer
			 * @return return true if an error occured
			 */
			bool setBuffer(ememory::SharedPtr<zeus::Buffer> _returnValue);
			/**
			 * @brief Get the transaction Id of the Future
			 * @return Transaction Id requested or 0
			 */
			uint32_t getTransactionId() const;
			/**
			 * @brief Get the client Id of the Future
			 * @return Client id requested or 0
			 */
			uint32_t getSource() const;
			/**
			 * @brief check if the answer have an error
			 * @return return true if an error is registered
			 */
			bool hasError() const;
			/**
			 * @brief get type of the error
			 * @return the string of the error type
			 */
			std::string getErrorType() const;
			/**
			 * @brief get help of the error
			 * @return the string of the error help
			 */
			std::string getErrorHelp() const;
			/**
			 * @brief Check if the Futur is a valid data
			 * @return return true if the data is valid
			 */
			bool isValid() const;
			/**
			 * @brief Check if the futur have finish receiving data
			 * @return status of the fisnish state
			 */
			bool isFinished() const;
			/**
			 * @brief Wait the Future receive data
			 * @return reference on the current futur
			 */
			const FutureBase& wait() const;
			/**
			 * @brief Wait the Future receive data
			 * @param[in] _delta delay to wait the data arrive
			 * @return reference on the current futur
			 */
			const FutureBase& waitFor(std::chrono::microseconds _delta = std::chrono::seconds(30)) const;
			/**
			 * @brief Wait the Future receive data
			 * @param[in] _endTime tiem to wait the data
			 * @return reference on the current futur
			 */
			const FutureBase& waitUntil(std::chrono::steady_clock::time_point _endTime) const;
			/**
			 * @brief Get the Buffer receive
			 * @return pointer on the receive data
			 */
			ememory::SharedPtr<zeus::Buffer> getRaw();
			/**
			 * @brief Get duration of the current trasaction take
			 * @return Tile in nanosecond to wait answer
			 */
			std::chrono::nanoseconds getTransmitionTime() const;
	};
}

