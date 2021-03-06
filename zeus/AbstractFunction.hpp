/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once
#include <zeus/debug.hpp>
#include <zeus/message/ParamType.hpp>
#include <zeus/message/Message.hpp>
#include <zeus/message/Call.hpp>
#include <zeus/Raw.hpp>
#include <ememory/memory.hpp>


namespace zeus {
	extern const etk::String g_threadKeyTransactionId;
	extern const etk::String g_threadKeyTransactionSource;
	extern const etk::String g_threadKeyTransactionDestination;
	/**
	 * @bried check if the compilater order the function element call in order or backOrder
	 */
	bool checkOrderFunctionParameter();
	/**
	 * @brief Interface to store a function and call it after with a @ref zeus::Message
	 */
	class AbstractFunction {
		protected:
			/**
			 * @brief Constructor
			 * @param[in] _name Nmae of the function
			 */
			AbstractFunction(const etk::String& _name);
		public:
			/**
			 * @brief generic virtual destructor
			 */
			virtual ~AbstractFunction() = default;
		public:
			/**
			 * @brief Under type of the call methode
			 */
			enum class type {
				unknow, //!< Does not know the type of the call
				global, //!< This is a global function
				local, //!< This is a local fucntion
				service, //!< This call a service function (global function like "srv.xxx")
				object, //!< this is for service instance call
			};
		protected:
			enum type m_type; //!< Type of the subCall (to permit to call many type of call)
		public:
			/**
			 * @brief Get the tyope of the call that is needed to do.
			 * @return Type of the call.
			 */
			enum type getType() const;
			/**
			 * @brief Set the type of the call that must be done for this function
			 * @param[in] _type New type of the call.
			 */
			void setType(enum type _type);
		protected:
			etk::String m_name; //!< name of the function
		public:
			/**
			 * @brief Get the name of the function.
			 * @return Function name
			 */
			const etk::String& getName() const;
		protected:
			etk::String m_description; //!< description of the function
		public:
			/**
			 * @brief Get the description of the function
			 * @return The description string of the function (same as doxygen 'brief')
			 */
			const etk::String& getDescription() const;
			/**
			 * @brief Set a new description of the function
			 * @param[in] _desc Descriptive string
			 */
			void setDescription(const etk::String& _desc);
		protected:
			etk::Vector<etk::Pair<etk::String, etk::String>> m_paramsDescription; //!< List of the parameter descriptions.
		public:
			/**
			 * @brief Set the parameter name and description
			 * @param[in] _idParam Number of the parameter
			 * @param[in] _name Name of the parameter
			 * @param[in] _desc Description of the parameter
			 */
			void setParam(int32_t _idParam, const etk::String& _name, const etk::String& _desc);
			/**
			 * @brief Set the parameter name and description of the last parameter not set (increment id every element)
			 * @param[in] _name Name of the parameter
			 * @param[in] _desc Description of the parameter
			 */
			void addParam(const etk::String& _name, const etk::String& _desc);
		protected:
			etk::String m_returnDescription; //!< Return description of the Function
		public:
			/**
			 * @brief Set the return description of the Function
			 * @param[in] _desc Description of the return parameter
			 */
			void setReturn(const etk::String& _desc);
		public:
			/**
			 * @brief Get the prototype of the function with the parameter name and type
			 * @return The fucntion like "void maFonction(int32 parameter_1, vector:string parameter_2);"
			 */
			etk::String getPrototype() const;
			/**
			 * @brief Get the signature of the function
			 * @return The signature of the function: "void(int32,vector:string);"
			 */
			virtual etk::String getSignature() const;
			/**
			 * @brief Get the string of the type of the return value
			 * @return type string of the return value
			 */
			virtual etk::String getPrototypeReturn() const = 0;
			/**
			 * @brief Get the list of type of the parameter
			 * @return List of types (zeus singature mode)
			 */
			virtual etk::Vector<etk::String> getPrototypeParam() const = 0;
			/**
			 * @brief Execute the function with all parameter properties
			 * @param[in] _interfaceClient Web interface to anser values
			 * @param[in] _obj Call object
			 * @param[in] _class Pointer on the object that might be call (note: brut cast)
			 */
			virtual void execute(ememory::SharedPtr<zeus::WebServer> _interfaceClient,
			                     ememory::SharedPtr<zeus::message::Call> _obj,
			                     void* _class=null) = 0;
	};
}

#include <zeus/WebServer.hpp>

