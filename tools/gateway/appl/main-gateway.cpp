/** @file
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <appl/debug.hpp>
#include <appl/GateWay.hpp>
#include <etk/etk.hpp>
#include <etk/path/fileSystem.hpp>
#include <zeus/zeus.hpp>
#include <etk/Allocator.hpp>
#include <etk/uri/uri.hpp>


#include <etk/stdTools.hpp>

#define GATEWAY_ENABLE_LAUNCHER

#ifdef GATEWAY_ENABLE_LAUNCHER
#include <ethread/Mutex.hpp>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <ethread/Thread.hpp>
#include <etk/stdTools.hpp>
#include <zeus/Object.hpp>
#include <zeus/Client.hpp>
#include <zeus/zeus.hpp>

typedef bool (*SERVICE_IO_init_t)(int _argc, const char *_argv[], etk::Uri _basePath);
typedef bool (*SERVICE_IO_uninit_t)();
typedef void (*SERVICE_IO_peridic_call_t)();
typedef zeus::Object* (*SERVICE_IO_instanciate_t)(uint32_t, ememory::SharedPtr<zeus::WebServer>&, uint32_t);

class PlugginAccess {
	private:
		etk::String m_name;
		etk::String m_fullName;
		void* m_handle;
		SERVICE_IO_init_t m_SERVICE_IO_init;
		SERVICE_IO_uninit_t m_SERVICE_IO_uninit;
		SERVICE_IO_peridic_call_t m_SERVICE_IO_peridic_call;
		SERVICE_IO_instanciate_t m_SERVICE_IO_instanciate;
	public:
		PlugginAccess(const etk::String& _name, const etk::String& _fullName) :
		  m_name(_name),
		  m_fullName(_fullName),
		  m_handle(null),
		  m_SERVICE_IO_init(null),
		  m_SERVICE_IO_uninit(null),
		  m_SERVICE_IO_instanciate(null) {
			etk::Path srv = etk::path::getBinaryDirectory() / ".." / "lib" / "lib" + m_fullName + "-impl.so";
			APPL_PRINT("++++++++++++++++++++++++++++++++");
			APPL_PRINT("++ srv: '" << m_name << "' ");
			APPL_PRINT("++++++++++++++++++++++++++++++++");
			APPL_PRINT("At position: '" << srv << "'");
			APPL_PRINT("with full name=" << m_fullName);
			m_handle = dlopen(srv.getNative().c_str(), RTLD_LAZY);
			if (!m_handle) {
				APPL_ERROR("Can not load Lbrary:" << dlerror());
				return;
			}
			char *error = null;
			m_SERVICE_IO_init = (SERVICE_IO_init_t)dlsym(m_handle, "SERVICE_IO_init");
			error = dlerror();
			if (error != null) {
				m_SERVICE_IO_init = null;
				APPL_WARNING("Can not function SERVICE_IO_init :" << error);
			}
			m_SERVICE_IO_uninit = (SERVICE_IO_uninit_t)dlsym(m_handle, "SERVICE_IO_uninit");
			error = dlerror();
			if (error != null) {
				m_SERVICE_IO_uninit = null;
				APPL_WARNING("Can not function SERVICE_IO_uninit :" << error);
			}
			m_SERVICE_IO_peridic_call = (SERVICE_IO_peridic_call_t)dlsym(m_handle, "SERVICE_IO_peridic_call");
			error = dlerror();
			if (error != null) {
				m_SERVICE_IO_uninit = null;
				APPL_WARNING("Can not function SERVICE_IO_uninit :" << error);
			}
			m_SERVICE_IO_instanciate = (SERVICE_IO_instanciate_t)dlsym(m_handle, "SERVICE_IO_instanciate");
			error = dlerror();
			if (error != null) {
				m_SERVICE_IO_instanciate = null;
				APPL_WARNING("Can not function SERVICE_IO_instanciate:" << error);
			}
		}
		~PlugginAccess() {
			
		}
		bool init(int _argc, const char *_argv[], etk::Uri _basePath) {
			if (m_SERVICE_IO_init == null) {
				return false;
			}
			if (_basePath.isEmpty() == true) {
				_basePath = "USER_DATA:///" + m_name;
				APPL_PRINT("Use base path: " << _basePath);
			} else {
				_basePath /= m_name;
			}
			return (*m_SERVICE_IO_init)(_argc, _argv, _basePath);
		}
		bool publish(zeus::Client& _client) {
			if (m_SERVICE_IO_instanciate == null) {
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
			if (m_SERVICE_IO_uninit == null) {
				return false;
			}
			return (*m_SERVICE_IO_uninit)();
		}
		void peridic_call() {
			if (m_SERVICE_IO_peridic_call == null) {
				return;
			}
			(*m_SERVICE_IO_peridic_call)();
		}
		const etk::String getName() const {
			return m_name;
		}
};
#endif


int main(int _argc, const char *_argv[]) {
	etk::init(_argc, _argv);
	zeus::init(_argc, _argv);
	appl::GateWay basicGateway;
	#ifdef GATEWAY_ENABLE_LAUNCHER
	etk::Uri basePath;
	etk::Vector<etk::String> services;
	zeus::Client m_client;
	// The default service port is 1985
	m_client.propertyPort.set(1985);
	#endif
	// default delay to disconnect is 30 seconds:
	uint32_t routerDisconnectionDelay = 30;
	for (int32_t iii=0; iii<_argc ; ++iii) {
		etk::String data = _argv[iii];
		APPL_PRINT("parameter:  [" << iii << "] '" << data << "'");
		if (etk::start_with(data, "--user=") == true) {
			basicGateway.propertyUserName.set(etk::String(&data[7]));
		} else if (data == "--no-router") {
			basicGateway.propertyRouterNo.set(true);
		} else if (etk::start_with(data, "--router-ip=") == true) {
			basicGateway.propertyRouterIp.set(etk::String(&data[12]));
		} else if (etk::start_with(data, "--router-port=") == true) {
			basicGateway.propertyRouterPort.set(etk::string_to_uint16_t(etk::String(&data[14])));
		} else if (etk::start_with(data, "--direct-ip=") == true) {
			basicGateway.propertyDirectIp.set(etk::String(&data[12]));
		} else if (etk::start_with(data, "--direct-port=") == true) {
			basicGateway.propertyDirectPort.set(etk::string_to_uint16_t(etk::String(&data[14])));
		} else if (etk::start_with(data, "--router-delay=") == true) {
			int32_t value = etk::string_to_int32_t(etk::String(&data[15]));
			if (value == -1) {
				routerDisconnectionDelay = 999999999;
			} else if (value == 0) {
				// do nothing
			} else {
				routerDisconnectionDelay = value;
			}
		} else if (etk::start_with(data, "--service-extern=") == true) {
			bool value = false;
			if (data == "--service-extern=true") {
				value = true;
			}
			basicGateway.propertyServiceExtern.set(value);
		} else if (etk::start_with(data, "--service-ip=") == true) {
			basicGateway.propertyServiceIp.set(etk::String(&data[13]));
			#ifdef GATEWAY_ENABLE_LAUNCHER
				m_client.propertyIp.set(etk::String(&data[13]));
			#endif
		} else if (etk::start_with(data, "--service-port=") == true) {
			basicGateway.propertyServicePort.set(etk::string_to_uint16_t(etk::String(&data[15])));
			#ifdef GATEWAY_ENABLE_LAUNCHER
				m_client.propertyPort.set(etk::string_to_uint16_t(etk::String(&data[15])));
			#endif
		} else if (etk::start_with(data, "--service-max=") == true) {
			basicGateway.propertyServiceMax.set(etk::string_to_uint16_t(etk::String(&data[14])));
		#ifdef GATEWAY_ENABLE_LAUNCHER
		} else if (etk::start_with(data, "--base-path=") == true) {
			basePath = etk::Path(etk::String(&data[12]));
		} else if (etk::start_with(data, "--srv=") == true) {
			services.pushBack(etk::String(&data[6]));
		#endif
		} else if (    data == "-h"
		            || data == "--help") {
			APPL_PRINT(etk::getApplicationName() << " - help : ");
			APPL_PRINT("    " << _argv[0] << " [options]");
			APPL_PRINT("        --user=XXX                   Name of the user that we are connected.");
			APPL_PRINT("        --no-router                  Router connection disable ==> this enable the direct donnection of external client like on the router");
			APPL_PRINT("        --router-ip=XXX              Router connection IP (default: " << basicGateway.propertyRouterIp.get() << ")");
			APPL_PRINT("        --router-port=XXX            Router connection PORT (default: " << basicGateway.propertyRouterPort.get() << ")");
			APPL_PRINT("        --direct-ip=XXX              Direct connection IP (default: " << basicGateway.propertyDirectIp.get() << ")");
			APPL_PRINT("        --direct-port=XXX            Direct connection PORT (default: " << basicGateway.propertyDirectPort.get() << " if 0 ==> not availlable)");
			APPL_PRINT("        --service-extern=frue/false  Disable the external service connection ==> remove open port ...(default: " << basicGateway.propertyServiceExtern.get() << ")");
			APPL_PRINT("        --service-ip=XXX             Service connection IP (default: " << basicGateway.propertyServiceIp.get() << ")");
			APPL_PRINT("        --service-port=XXX           Service connection PORT (default: " << basicGateway.propertyServicePort.get() << ")");
			APPL_PRINT("        --service-max=XXX            Service Maximum IO (default: " << basicGateway.propertyServiceMax.get() << ")");
			APPL_PRINT("        --router-delay=XXX           Delay before disconnect from the router (default: " << routerDisconnectionDelay << "; 0=automatic set by the gateway; -1=never disconnect; other the time)");
			#ifdef GATEWAY_ENABLE_LAUNCHER
			APPL_PRINT("        specific for internal launcher:");
			APPL_PRINT("        --base-path=XXX              base path to search data (default: 'USERDATA:')");
			APPL_PRINT("        --srv=XXX                    service path (N)");
			#endif
			return -1;
		}
	}
	#ifdef GATEWAY_ENABLE_LAUNCHER
	etk::Vector<etk::Pair<etk::String,etk::String>> listAvaillableServices;
	if (services.size() != 0) {
		// find all services:
		etk::Path dataPath(etk::path::getBinaryDirectory() / ".." / "share");
		etk::Vector<etk::Path> listSubPath = etk::path::list(dataPath, etk::path::LIST_FOLDER);
		APPL_DEBUG(" Base data path: " << dataPath);
		APPL_DEBUG(" SubPath: " << listSubPath);
		for (auto &it: listSubPath) {
			if (etk::path::exist(it / "zeus") == true) {
				etk::Path dataPath(it / "zeus");
				etk::Vector<etk::Path> listServices = etk::path::list(dataPath, etk::path::LIST_FILE);
				for (auto &it2: listServices) {
					if (it2.getExtention() != "srv") {
						continue;
					}
					etk::String nameFileSrv = it2.getFileName();
					etk::Vector<etk::String> spl = etk::String(nameFileSrv.begin(), nameFileSrv.end()-4).split("-service-");
					if (spl.size() != 2) {
						APPL_ERROR("reject service, wrong format ... '" << it2 << "' missing XXX-service-SERVICE-NAME.srv");
						continue;
					}
					APPL_INFO("find service : " << it2);
					listAvaillableServices.pushBack(etk::makePair(spl[1], etk::String(nameFileSrv.begin(), nameFileSrv.end()-4)));
				}
			} else {
				// not check the second path ==> no service availlable
			}
		}
	}
	#endif
	int32_t countMemeCheck = 0;
	APPL_INFO("==================================");
	APPL_INFO("== ZEUS gateway start            ==");
	APPL_INFO("==================================");
	basicGateway.start();
	#ifdef GATEWAY_ENABLE_LAUNCHER
	if (services.size() == 0) {
		bool routerAlive = true;
	#endif
		while (routerAlive == true) {
			ethread::sleepMilliSeconds((100));
			basicGateway.cleanIO();
			if (countMemeCheck++ >= 200) {
				countMemeCheck = 0;
				ETK_MEM_SHOW_LOG(true);
			}
			routerAlive = basicGateway.checkIsAlive(echrono::seconds(routerDisconnectionDelay));
			if (routerAlive == false) {
				APPL_WARNING("Router is Dead or Timeout");
			}
		}
	#ifdef GATEWAY_ENABLE_LAUNCHER
	} else {
		bool routerAlive = true;
		etk::Vector<ememory::SharedPtr<PlugginAccess>> listElements;
		if (    services.size() == 1
		     && services[0] == "all") {
			for (auto &it: listAvaillableServices) {
				ememory::SharedPtr<PlugginAccess> tmp = ememory::makeShared<PlugginAccess>(it.first, it.second);
				listElements.pushBack(tmp);
			}
		} else {
			for (auto &it: services) {
				// find the real service name:
				bool find = false;
				for (auto &it2: listAvaillableServices) {
					if (it2.first == it) {
						ememory::SharedPtr<PlugginAccess> tmp = ememory::makeShared<PlugginAccess>(it2.first, it2.second);
						listElements.pushBack(tmp);
						find = true;
						break;
					}
				}
				if (find == false) {
					APPL_ERROR("Can not find the service: " << it);
				}
			}
		}
		APPL_WARNING("=================================================");
		APPL_WARNING("== INIT All service");
		APPL_WARNING("=================================================");
		for (auto &it: listElements) {
			APPL_WARNING("    init: " << it->getName());
			it->init(_argc, _argv, basePath);
		}
		APPL_WARNING("=================================================");
		APPL_WARNING("== check client connected:");
		APPL_WARNING("=================================================");
		if (m_client.connect() == false) {
			APPL_WARNING("    ==> not connected");
			return -1;
		}
		APPL_WARNING("=================================================");
		APPL_WARNING("== Publish all service");
		APPL_WARNING("=================================================");
		for (auto &it: listElements) {
			APPL_WARNING("    publish: " << it->getName());
			it->publish(m_client);
		}
		uint32_t iii = 0;
		while (    m_client.isAlive() == true
		        && routerAlive == true) {
			m_client.pingIsAlive();
			m_client.displayConnectedObject();
			m_client.cleanDeadObject();
			for (auto &it: listElements) {
				it->peridic_call();
			}
			basicGateway.cleanIO();
			routerAlive = basicGateway.checkIsAlive(echrono::seconds(routerDisconnectionDelay));
			if (routerAlive == false) {
				APPL_WARNING("Router is Dead or Timeout");
			} else {
				elog::flush();
				ethread::sleepMilliSeconds(1000);
				if (countMemeCheck++ >= 20) {
					countMemeCheck = 0;
					ETK_MEM_SHOW_LOG(true);
				}
				APPL_INFO("gateway in waiting ... " << iii << "/inf");
			}
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
	elog::flush();
	return 0;
}
