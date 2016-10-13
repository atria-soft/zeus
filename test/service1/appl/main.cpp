/** @file
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <appl/debug.hpp>
#include <zeus/Service.hpp>
#include <etk/etk.hpp>
#include <zeus/zeus.hpp>


#include <etk/stdTools.hpp>
namespace appl {
	class User {
		public:
			User() {
				APPL_WARNING("new USER");
			}
			~User() {
				APPL_WARNING("delete USER");
			}
			
	};
	
	class UserManager {
		private:
			std::map<std::string, ememory::SharedPtr<appl::User>> m_listLoaded;
		public:
			UserManager() {
				
			}
			ememory::SharedPtr<appl::User> getUser(const std::string& _userName) {
				// TODO : Lock ...
				auto it = m_listLoaded.find(_userName);
				if (it != m_listLoaded.end()) {
					// User already loaded:
					return it->second;
				}
				// load New User:
				ememory::SharedPtr<appl::User> tmp(new appl::User);
				m_listLoaded.insert(std::make_pair(_userName, tmp));
				return tmp;
			}
	};
	class Calculator {
		public:
			Calculator() {
				APPL_WARNING("New Calculator ...");
			}
			~Calculator() {
				APPL_WARNING("delete Calculator ...");
			}
		private:
			ememory::SharedPtr<appl::User> m_user;
		private:
			ememory::SharedPtr<zeus::ClientProperty> m_client;
		public:
			double mul(double _val1, double _val2) {
				return _val1*_val2;
			}
		public:
			Calculator(ememory::SharedPtr<appl::User> _user, ememory::SharedPtr<zeus::ClientProperty> _client) :
			  m_user(_user),
			  m_client(_client) {
				//advertise("mul", &appl::Service1::mul, "simple multiplication to test double IO");
			}
	};
}


int main(int _argc, const char *_argv[]) {
	etk::init(_argc, _argv);
	zeus::init(_argc, _argv);
	ememory::SharedPtr<appl::UserManager> userMng = ememory::makeShared<appl::UserManager>();
	zeus::ServiceType<appl::Calculator, appl::UserManager> serviceInterface(userMng);
	serviceInterface.setDescription("Calculator interface");
	serviceInterface.setVersion("0.1.1");
	serviceInterface.addAuthor("Heero Yui", "yui.heero@gmail.com");
	zeus::AbstractFunction* func = serviceInterface.advertise("mul", &appl::Calculator::mul);
	if (func != nullptr) {
		func->setDescription("simple multiplication to test double IO");
		func->addParam("val1", "First Parameter To multiply");
		func->addParam("val2", "Second Parameter To multiply");
	}
	for (int32_t iii=0; iii<_argc ; ++iii) {
		std::string data = _argv[iii];
		if (etk::start_with(data, "--ip=") == true) {
			serviceInterface.propertyIp.set(std::string(&data[5]));
		} else if (etk::start_with(data, "--port=") == true) {
			serviceInterface.propertyPort.set(etk::string_to_uint16_t(std::string(&data[7])));
		} else if (    data == "-h"
		            || data == "--help") {
			APPL_PRINT(etk::getApplicationName() << " - help : ");
			APPL_PRINT("    " << _argv[0] << " [options]");
			APPL_PRINT("        --ip=XXX      Server connection IP (default: 1.7.0.0.1)");
			APPL_PRINT("        --port=XXX    Server connection PORT (default: 1983)");
			return -1;
		}
	}
	APPL_INFO("==================================");
	APPL_INFO("== ZEUS test service1 start      ==");
	APPL_INFO("==================================");
	serviceInterface.propertyNameService.set("serviceTest1");
	serviceInterface.connect();
	int32_t iii=0;
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		serviceInterface.pingIsAlive();
		APPL_INFO("service in waiting ... " << iii << "/inf");
		iii++;
	}
	serviceInterface.disconnect();
	APPL_INFO("==================================");
	APPL_INFO("== ZEUS test service1 stop       ==");
	APPL_INFO("==================================");
	return 0;
}
