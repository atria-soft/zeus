/** @file
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <appl/debug.hpp>
#include <appl/GateWay.hpp>
#include <etk/etk.hpp>
#include <zeus/zeus.hpp>


#include <etk/stdTools.hpp>

#define GATEWAY_ENABLE_LAUNCHER

#ifdef GATEWAY_ENABLE_LAUNCHER
#include <mutex>
#include <etk/os/FSNode.hpp>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <thread>
#include <etk/stdTools.hpp>
#include <zeus/Object.hpp>
#include <zeus/Client.hpp>
#include <zeus/zeus.hpp>

typedef bool (*SERVICE_IO_init_t)(int _argc, const char *_argv[], std::string _basePath);
typedef bool (*SERVICE_IO_uninit_t)();
typedef void (*SERVICE_IO_peridic_call_t)();
typedef zeus::Object* (*SERVICE_IO_instanciate_t)(uint32_t, ememory::SharedPtr<zeus::WebServer>&, uint32_t);

class PlugginAccess {
	private:
		std::string m_name;
		void* m_handle;
		SERVICE_IO_init_t m_SERVICE_IO_init;
		SERVICE_IO_uninit_t m_SERVICE_IO_uninit;
		SERVICE_IO_peridic_call_t m_SERVICE_IO_peridic_call;
		SERVICE_IO_instanciate_t m_SERVICE_IO_instanciate;
		zeus::Client m_client;
	public:
		PlugginAccess(const std::string& _name) :
		  m_name(_name),
		  m_handle(nullptr),
		  m_SERVICE_IO_init(nullptr),
		  m_SERVICE_IO_uninit(nullptr),
		  m_SERVICE_IO_instanciate(nullptr) {
			std::string srv = etk::FSNodeGetApplicationPath() + "/../lib/libzeus-service-" + m_name + "-impl.so";
			APPL_PRINT("Try to open service with name: '" << m_name << "' at position: '" << srv << "'");
			m_handle = dlopen(srv.c_str(), RTLD_LAZY);
			if (!m_handle) {
				APPL_ERROR("Can not load Lbrary:" << dlerror());
				return;
			}
			char *error = nullptr;
			m_SERVICE_IO_init = (SERVICE_IO_init_t)dlsym(m_handle, "SERVICE_IO_init");
			error = dlerror();
			if (error != nullptr) {
				m_SERVICE_IO_init = nullptr;
				APPL_WARNING("Can not function SERVICE_IO_init :" << error);
			}
			m_SERVICE_IO_uninit = (SERVICE_IO_uninit_t)dlsym(m_handle, "SERVICE_IO_uninit");
			error = dlerror();
			if (error != nullptr) {
				m_SERVICE_IO_uninit = nullptr;
				APPL_WARNING("Can not function SERVICE_IO_uninit :" << error);
			}
			m_SERVICE_IO_peridic_call = (SERVICE_IO_peridic_call_t)dlsym(m_handle, "SERVICE_IO_peridic_call");
			error = dlerror();
			if (error != nullptr) {
				m_SERVICE_IO_uninit = nullptr;
				APPL_WARNING("Can not function SERVICE_IO_uninit :" << error);
			}
			m_SERVICE_IO_instanciate = (SERVICE_IO_instanciate_t)dlsym(m_handle, "SERVICE_IO_instanciate");
			error = dlerror();
			if (error != nullptr) {
				m_SERVICE_IO_instanciate = nullptr;
				APPL_WARNING("Can not function SERVICE_IO_instanciate:" << error);
			}
		}
		~PlugginAccess() {
			
		}
		bool init(int _argc, const char *_argv[], std::string _basePath) {
			if (m_SERVICE_IO_init == nullptr) {
				return false;
			}
			
			if (_basePath.size() == 0) {
				_basePath = "USERDATA:" + m_name + "/";
				APPL_PRINT("Use base path: " << _basePath);
			}
			return (*m_SERVICE_IO_init)(_argc, _argv, _basePath);
		}
		bool publish(zeus::Client& _client) {
			if (m_SERVICE_IO_instanciate == nullptr) {
				return false;
			}
			_client.serviceAdd(m_name,  [=](uint32_t _transactionId, ememory::SharedPtr<zeus::WebServer>& _iface, uint32_t _destination) {
			                            	(*m_SERVICE_IO_instanciate)(_transactionId, _iface, _destination);
			                            });
			return true;
		}
		bool disconnect(zeus::Client& _client) {
			_client.serviceRemove(m_name);
			return true;
		}
		bool uninit() {
			if (m_SERVICE_IO_uninit == nullptr) {
				return false;
			}
			return (*m_SERVICE_IO_uninit)();
		}
		void peridic_call() {
			if (m_SERVICE_IO_peridic_call == nullptr) {
				return;
			}
			(*m_SERVICE_IO_peridic_call)();
		}
};
#endif


int main(int _argc, const char *_argv[]) {
	etk::init(_argc, _argv);
	zeus::init(_argc, _argv);
	appl::GateWay basicGateway;
	#ifdef GATEWAY_ENABLE_LAUNCHER
	std::string basePath;
	std::vector<std::string> services;
	zeus::Client m_client;
	#endif
	for (int32_t iii=0; iii<_argc ; ++iii) {
		std::string data = _argv[iii];
		if (etk::start_with(data, "--user=") == true) {
			basicGateway.propertyUserName.set(std::string(&data[7]));
		} else if (data == "--no-router") {
			basicGateway.propertyRouterNo.set(true);
		} else if (etk::start_with(data, "--router-ip=") == true) {
			basicGateway.propertyRouterIp.set(std::string(&data[12]));
		} else if (etk::start_with(data, "--router-port=") == true) {
			basicGateway.propertyRouterPort.set(etk::string_to_uint16_t(std::string(&data[14])));
		} else if (etk::start_with(data, "--service-ip=") == true) {
			basicGateway.propertyServiceIp.set(std::string(&data[13]));
			#ifdef GATEWAY_ENABLE_LAUNCHER
				m_client.propertyIp.set(std::string(&data[13]));
			#endif
		} else if (etk::start_with(data, "--service-port=") == true) {
			basicGateway.propertyServicePort.set(etk::string_to_uint16_t(std::string(&data[15])));
			#ifdef GATEWAY_ENABLE_LAUNCHER
				m_client.propertyPort.set(etk::string_to_uint16_t(std::string(&data[15])));
			#endif
		} else if (etk::start_with(data, "--service-max=") == true) {
			basicGateway.propertyServiceMax.set(etk::string_to_uint16_t(std::string(&data[14])));
		#ifdef GATEWAY_ENABLE_LAUNCHER
		} else if (etk::start_with(data, "--base-path=") == true) {
			basePath = std::string(&data[12]);
		} else if (etk::start_with(data, "--srv=") == true) {
			services.push_back(std::string(&data[6]));
		#endif
		} else if (    data == "-h"
		            || data == "--help") {
			APPL_PRINT(etk::getApplicationName() << " - help : ");
			APPL_PRINT("    " << _argv[0] << " [options]");
			APPL_PRINT("        --user=XXX           Name of the user that we are connected.");
			APPL_PRINT("        --no-router          Router connection disable ==> this enable the direct donnection of external client like on the router");
			APPL_PRINT("        --router-ip=XXX      Router connection IP (default: 1.7.0.0.1)");
			APPL_PRINT("        --router-port=XXX    Router connection PORT (default: 1984)");
			APPL_PRINT("        --service-ip=XXX     Service connection IP (default: 1.7.0.0.1)");
			APPL_PRINT("        --service-port=XXX   Service connection PORT (default: 1985)");
			APPL_PRINT("        --service-max=XXX    Service Maximum IO (default: 15)");
			#ifdef GATEWAY_ENABLE_LAUNCHER
			APPL_PRINT("        specific for internal launcher:");
			APPL_PRINT("        --base-path=XXX      base path to search data (default: 'USERDATA:')");
			APPL_PRINT("        --srv=XXX            service path (N)");
			#endif
			return -1;
		}
	}
	APPL_INFO("==================================");
	APPL_INFO("== ZEUS gateway start            ==");
	APPL_INFO("==================================");
	basicGateway.start();
	#ifdef GATEWAY_ENABLE_LAUNCHER
	if (services.size() == 0) {
	#endif
		while (true) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			basicGateway.cleanIO();
		}
	#ifdef GATEWAY_ENABLE_LAUNCHER
	} else {
		std::vector<ememory::SharedPtr<PlugginAccess>> listElements;
		for (auto &it: services) {
			ememory::SharedPtr<PlugginAccess> tmp = ememory::makeShared<PlugginAccess>(it);
			listElements.push_back(tmp);
		}
		for (auto &it: listElements) {
			it->init(_argc, _argv, basePath);
		}
		if (m_client.connect() == false) {
			return -1;
		}
		for (auto &it: listElements) {
			it->publish(m_client);
		}
		uint32_t iii = 0;
		while(m_client.isAlive() == true) {
			m_client.pingIsAlive();
			m_client.displayConnectedObject();
			m_client.cleanDeadObject();
			for (auto &it: listElements) {
				it->peridic_call();
			}
			basicGateway.cleanIO();
			std::this_thread::sleep_for(std::chrono::seconds(1));
			APPL_INFO("service in waiting ... " << iii << "/inf");
			iii++;
		}
		for (auto &it: listElements) {
			it->disconnect(m_client);
		}
		m_client.disconnect();
		APPL_INFO("Stop service*** ==> flush internal datas ...");
		for (auto &it: listElements) {
			it->uninit();
		}
	}
	#endif
	basicGateway.stop();
	APPL_INFO("==================================");
	APPL_INFO("== ZEUS gateway stop             ==");
	APPL_INFO("==================================");
	return 0;
}