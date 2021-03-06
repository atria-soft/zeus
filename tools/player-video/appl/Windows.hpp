/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license GPL v3 (see license file)
 */
#pragma once

#include <ewol/widget/Windows.hpp>
#include <ewol/widget/Composer.hpp>
#include <appl/widget/ListViewer.hpp>
#include <appl/widget/Player.hpp>


namespace appl {
	class Windows;
	using WindowsShared = ememory::SharedPtr<appl::Windows>;
	using WindowsWeak = ememory::WeakPtr<appl::Windows>;
	class Windows : public ewol::widget::Windows {
		protected:
			Windows();
			void init();
			ewol::widget::ComposerShared m_composer;
			ememory::SharedPtr<ClientProperty> m_clientProp;
			appl::widget::ListViewerShared m_listViewer;
			appl::widget::PlayerShared m_player;
			etk::Vector<etk::String> m_list;
			int32_t m_id;
		public:
			DECLARE_FACTORY(Windows);
		public: // callback functions
			void onCallbackBack();
			void onCallbackPrevious();
			void onCallbackPlay(const bool& _isPressed);
			void onCallbackNext();
			void addFile(const etk::String& _file);
			void onCallbackSeekRequest(const float& _value);
			
			
			void onCallbackConnectionValidate(const ememory::SharedPtr<ClientProperty>& _prop);
			void onCallbackConnectionError(const ememory::SharedPtr<ClientProperty>& _prop);
			void onCallbackConnectionCancel();
			
			void onCallbackShortCut(const etk::String& _value);
			void onCallbackMenuEvent(const etk::String& _value);
		protected:
			void load_db();
			void store_db();
			void onCallbackSelectFilms();
			void onCallbackSelectAnnimation();
			void onCallbackSelectTVShow();
			void onCallbackSelectTvAnnimation();
			void onCallbackSelectTeather();
			void onCallbackSelectOneManShow();
			void onCallbackSelectSourses();
			void onCallbackSelectMedia(const uint32_t& _value);
			void onCallbackSelectBack() {
				onCallbackMenuEvent("menu:back");
			}
			void onCallbackSelectGroup() {
				onCallbackMenuEvent("menu:group");
			}
			void onCallbackSelectHome() {
				onCallbackMenuEvent("menu:home");
			}
			void onCallbackPlayerPrevious();
			void onCallbackPlayerNext();
			void onCallbackPlayerFinished();
		protected:
			bool m_fullScreen;
	};
}
