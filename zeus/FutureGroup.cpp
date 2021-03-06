/** @file
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <zeus/FutureGroup.hpp>
#include <zeus/debug.hpp>


#include <etk/typeInfo.hpp>
ETK_DECLARE_TYPE(zeus::FutureGroup);

void zeus::FutureGroup::add(const zeus::FutureBase& _fut) {
	m_listFuture.pushBack(_fut);
}

void zeus::FutureGroup::wait() const {
	waitFor();
}

void zeus::FutureGroup::waitFor(echrono::Duration _delta) const {
	waitUntil(echrono::Steady::now() + _delta);
}

void zeus::FutureGroup::waitUntil(echrono::Steady _endTime) const {
	bool allIsFinished = false;
	while (    echrono::Steady::now() < _endTime
	        && allIsFinished == false) {
		allIsFinished = true;
		for (auto &it : m_listFuture) {
			if (it.isFinished() == false) {
				allIsFinished = false;
			}
		}
		ethread::sleepMilliSeconds(10);
	}
	if (allIsFinished == false) {
		ZEUS_WARNING("Group Wait timeout ...");
	}
}
