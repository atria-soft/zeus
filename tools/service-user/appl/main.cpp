/** @file
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <appl/debug.hpp>
#include <zeus/Service.hpp>
#include <etk/etk.hpp>
#include <zeus/zeus.hpp>

#include <mutex>
#include <ejson/ejson.hpp>

#include <etk/stdTools.hpp>

#include <zeus/service/User.hpp>
#include <zeus/service/registerUser.hpp>

static std::mutex g_mutex;
static std::string g_basePath;
static std::string g_baseDBName = std::string(SERVICE_NAME) + "-database.json";
static ejson::Document g_database;

namespace appl {
	class SystemService : public zeus::service::User {
		private:
			ememory::SharedPtr<zeus::ClientProperty> m_client;
			std::string m_userName;
		public:
			/*
			SystemService(ememory::SharedPtr<zeus::ClientProperty> _client, const std::string& _userName) :
			  m_client(_client),
			  m_userName(_userName) {
				APPL_WARNING("New SystemService ... for user: ");
			}
			*/
			SystemService(uint16_t _clientId) {
				APPL_WARNING("New SystemService ... for user: " << _clientId);
			}
			~SystemService() {
				APPL_WARNING("Delete service-user interface.");
			}
		public:
			std::vector<std::string> clientGroupsGet(std::string _clientName) {
				APPL_WARNING("call clientGroupsGet : " << _clientName);
				std::vector<std::string> out;
				if (m_client == nullptr) {
					return out;
				}
				// TODO: check if basished ...
				if (m_client->getName() != "") {
					std::unique_lock<std::mutex> lock(g_mutex);
					std::vector<std::string> out;
					ejson::Object clients = g_database["client"].toObject();
					if (clients.exist() == false) {
						// Section never created
						return out;
					}
					ejson::Object client = clients[m_client->getName()].toObject();
					if (clients.exist() == false) {
						// No specificity for this client (in case it have no special right)
						return out;
					}
					if (client["tocken"].toString().get() != "") {
						out.push_back("connected");
					}
					// TODO: check banishing ...
					ejson::Array groups = client["group"].toArray();
					for (auto it : groups) {
						out.push_back(it.toString().get());
					}
				}
				// TODO: Check default visibility ... (if user want to have default visibility at Noone ==> then public must be removed...
				if (true) {
					out.push_back("public");
				}
				return out;
			}
			bool checkTocken(std::string _clientName, std::string _tocken) {
				std::unique_lock<std::mutex> lock(g_mutex);
				APPL_INFO("Check TOCKEN for : '" << _clientName << "' tocken='" << _clientName << "'");
				ejson::Object clients = g_database["client"].toObject();
				if (clients.exist() == false) {
					APPL_INFO("     ==> return false");
					// Section never created
					return false;
				}
				ejson::Object client = clients[_clientName].toObject();
				if (clients.exist() == false) {
					APPL_INFO("     ==> return false");
					// No specificity for this client (in case it have no special right)
					return false;
				}
				// TODO: check banishing ...
				// TODO: Do it better ...
				std::string registerTocken = client["tocken"].toString().get();
				if (registerTocken == _tocken) {
					APPL_INFO("     ==> return true");
					return true;
				}
				APPL_INFO("     ==> return false");
				return false;
			}
			bool checkAuth(std::string _password) {
				std::unique_lock<std::mutex> lock(g_mutex);
				APPL_INFO("Check AUTH for : '" << _password << "'");
				std::string pass = g_database["password"].toString().get();
				if (pass == "") {
					// pb password
					return false;
				}
				if (pass == _password) {
					return true;
				}
				return false;
			}
			std::vector<std::string> filterClientServices(std::string _clientName, std::vector<std::string> _currentList) {
				std::unique_lock<std::mutex> lock(g_mutex);
				APPL_INFO("Filter services : '" << _clientName << "' " << _currentList);
				// When connected to our session ==> we have no control access ...
				if (_clientName == m_userName) {
					APPL_INFO("    return all");
					return _currentList;
				}
				std::vector<std::string> out;
				APPL_TODO("Filter service list ==> not implemented...");
				return out;
			}
	};
}

ETK_EXPORT_API bool SERVICE_IO_init(int _argc, const char *_argv[], std::string _basePath) {
	g_basePath = _basePath;
	std::unique_lock<std::mutex> lock(g_mutex);
	APPL_WARNING("Load USER: " << g_basePath);
	bool ret = g_database.load(g_basePath + g_baseDBName);
	if (ret == false) {
		APPL_WARNING("    ==> LOAD error");
	}
	return true;
}

ETK_EXPORT_API bool SERVICE_IO_uninit() {
	std::unique_lock<std::mutex> lock(g_mutex);
	APPL_DEBUG("Store User Info:");
	bool ret = g_database.storeSafe(g_basePath + g_baseDBName);
	if (ret == false) {
		APPL_WARNING("    ==> Store error");
		return false;
	}
	APPL_WARNING("delete USER [STOP]");
	return true;
}

ZEUS_SERVICE_USER_DECLARE(appl::SystemService);
