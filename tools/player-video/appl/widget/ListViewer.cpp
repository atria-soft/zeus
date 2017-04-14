/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license GPL v3 (see license file)
 */

#include <appl/debug.hpp>
#include <ewol/widget/Widget.hpp>



#include <appl/debug.hpp>
#include <appl/widget/ListViewer.hpp>
#include <ewol/object/Manager.hpp>
#include <etk/tool.hpp>

#include <egami/egami.hpp>
#include <zeus/zeus.hpp>
#include <zeus/Client.hpp>
#include <zeus/service/ProxyVideo.hpp>
#include <zeus/ProxyFile.hpp>
#include <zeus/ObjectRemote.hpp>
#include <echrono/Steady.hpp>
#include <zeus/FutureGroup.hpp>
#include <etk/stdTools.hpp>
#include <zeus/ProxyMedia.hpp>
#include <ejson/ejson.hpp>

appl::widget::ListViewer::ListViewer() :
  signalSelect(this, "select", "Select a media to view") {
	addObjectType("appl::widget::ListViewer");
	propertyCanFocus.setDirectCheck(true);
	setLimitScrolling(0.2);
	setMouseLimit(1);
}

void appl::widget::ListViewer::init() {
	ewol::Widget::init();
	markToRedraw();
	//m_compImageVideo.setSource("DATA:Video.svg", 128);
	//m_compImageAudio.setSource("DATA:MusicNote.svg", 128);
}

appl::widget::ListViewer::~ListViewer() {
	
}

void appl::widget::ListViewer::searchElements(std::string _filter) {
	m_listElement.clear();
	m_listDisplay.clear();
	if (m_clientProp == nullptr) {
		APPL_ERROR("No client Availlable ...");
		return;
	}
	if (_filter == "film") {
		_filter = "'type' == 'film' AND 'production-methode' == 'picture'";
	} else if (_filter == "annimation") {
		_filter = "'type' == 'film' AND 'production-methode' == 'draw'";
	} else if (_filter == "tv-show") {
		_filter = "'type' == 'tv-show' AND 'production-methode' == 'picture'";
	} else if (_filter == "tv-annimation") {
		_filter = "'type' == 'tv-show' AND 'production-methode' == 'draw'";
	} else if (_filter == "theater") {
		_filter = "'type' == 'theater'";
	} else if (_filter == "one-man") {
		_filter = "'type' == 'one-man'";
	} else if (_filter == "courses") {
		_filter = "'type' == 'courses'";
	} else {
		_filter = "*";
	}
	
	m_clientProp->connect();
	if (m_clientProp->connection.isAlive() == false) {
		APPL_ERROR("Conection is not alive anymore ...");
		return;
	}
	
	bool retSrv = m_clientProp->connection.waitForService("video");
	if (retSrv == false) {
		APPL_ERROR(" ==> SERVICE not availlable or not started");
		return;
	}
	// get all the data:
	zeus::service::ProxyVideo remoteServiceVideo = m_clientProp->connection.getService("video");
	// remove all media (for test)
	if (remoteServiceVideo.exist() == false) {
		APPL_ERROR("    ==> Service does not exist : 'video'");
		return;
	}
	zeus::Future<std::vector<uint32_t>> listElem = remoteServiceVideo.getSQL(_filter).wait();
	if (listElem.hasError() == true) {
		APPL_ERROR("    ==> Can not get element from video service <with fileter ! '" << _filter << "' : " << listElem.getErrorType() << " : " << listElem.getErrorHelp());
		return;
	}
	std::vector<uint32_t> returnValues = listElem.get();
	APPL_INFO("Get some Values: " << returnValues << "");
	for (auto &it : returnValues) {
		auto elem = ememory::makeShared<ElementProperty>();
		if (elem == nullptr) {
			APPL_ERROR("Can not allocate element... " << it);
			continue;
		}
		elem->m_id = it;
		elem->m_metadataUpdated = false;
		// TODO : Type the "andThen" to simplify user experience
		// TODO : Add the reference on the typed future in the function andTrn ... ==> then we can add later the cancel
		
		// Get the media
		zeus::ProxyMedia media = remoteServiceVideo.get(it).waitFor(echrono::seconds(2000)).get();
		if (media.exist() == false) {
			APPL_ERROR("get media error");
			continue;
		}
		
		appl::widget::ListViewerShared tmpWidget = ememory::staticPointerCast<appl::widget::ListViewer>(sharedFromThis());
		media.getMetadata("title")
		    .andThen([=](zeus::Future<std::string> _fut) mutable {
		             	APPL_INFO("    [" << elem->m_id << "] get title: " << _fut.get());
		             	{
		             		std::unique_lock<std::mutex> lock(elem->m_mutex);
		             		elem->m_title = _fut.get();
		             	}
		             	tmpWidget->markToRedraw();
		             	return true;
		             });
		media.getMetadata("series-name")
		    .andThen([=](zeus::Future<std::string> _fut) mutable {
		             	APPL_ERROR("    [" << elem->m_id << "] get serie: " << _fut.get());
		             	{
		             		std::unique_lock<std::mutex> lock(elem->m_mutex);
		             		elem->m_serie = _fut.get();
		             	}
		             	tmpWidget->markToRedraw();
		             	return true;
		             });
		media.getMetadata("saison")
		    .andThen([=](zeus::Future<std::string> _fut) mutable {
		             	APPL_INFO("    [" << elem->m_id << "] get saison: " << _fut.get());
		             	{
		             		std::unique_lock<std::mutex> lock(elem->m_mutex);
		             		elem->m_saison = _fut.get();
		             	}
		             	tmpWidget->markToRedraw();
		             	return true;
		             });
		media.getMetadata("episode")
		    .andThen([=](zeus::Future<std::string> _fut) mutable {
		             	APPL_INFO("    [" << elem->m_id << "] get episode: " << _fut.get());
		             	{
		             		std::unique_lock<std::mutex> lock(elem->m_mutex);
		             		elem->m_episode = _fut.get();
		             	}
		             	tmpWidget->markToRedraw();
		             	return true;
		             });
		media.getMetadata("description")
		    .andThen([=](zeus::Future<std::string> _fut) mutable {
		             	APPL_INFO("    [" << elem->m_id << "] get description: " << _fut.get());
		             	{
		             		std::unique_lock<std::mutex> lock(elem->m_mutex);
		             		elem->m_description = _fut.get();
		             	}
		             	tmpWidget->markToRedraw();
		             	return true;
		             });
		media.getMetadata("production-methode")
		    .andThen([=](zeus::Future<std::string> _fut) mutable {
		             	APPL_INFO("    [" << elem->m_id << "] get production-methode: " << _fut.get());
		             	{
		             		std::unique_lock<std::mutex> lock(elem->m_mutex);
		             		elem->m_productMethode = _fut.get();
		             	}
		             	tmpWidget->markToRedraw();
		             	return true;
		             });
		media.getMetadata("type")
		    .andThen([=](zeus::Future<std::string> _fut) mutable {
		             	APPL_INFO("    [" << elem->m_id << "] get type: " << _fut.get());
		             	{
		             		std::unique_lock<std::mutex> lock(elem->m_mutex);
		             		elem->m_type = _fut.get();
		             	}
		             	tmpWidget->markToRedraw();
		             	return true;
		             });
		media.getMineType()
		    .andThen([=](zeus::Future<std::string> _fut) mutable {
		             	APPL_INFO("    [" << elem->m_id << "] get mine-type: " << _fut.get());
		             	{
		             		std::unique_lock<std::mutex> lock(elem->m_mutex);
		             		elem->m_mineType = _fut.get();
		             		if (etk::start_with(elem->m_mineType, "video") == true) {
		             			// TODO : Optimise this ...
		             			elem->m_thumb = egami::load("DATA:Video.svg", ivec2(128,128));
		             		} else if (etk::start_with(elem->m_mineType, "audio") == true) {
		             			// TODO : Optimise this ...
		             			elem->m_thumb = egami::load("DATA:MusicNote.svg", ivec2(128,128));
		             		}
		             	}
		             	tmpWidget->markToRedraw();
		             	return true;
		             });
		elem->m_metadataUpdated = true;
		//elem->m_thumb = remoteServiceVideo.mediaThumbGet(it, 128).wait().get();
		m_listElement.push_back(elem);
	}
	APPL_INFO("Request All is done");
}

void appl::widget::ListViewer::onDraw() {
	m_text.draw();
	for (auto &it : m_listDisplay) {
		if (it == nullptr) {
			continue;
		}
		it->draw();
	}
	ewol::widget::WidgetScrolled::onDraw();
}

void appl::widget::ListViewer::onRegenerateDisplay() {
	ewol::widget::WidgetScrolled::onRegenerateDisplay();
	//!< Check if we really need to redraw the display, if not needed, we redraw the previous data ...
	if (needRedraw() == false) {
		return;
	}
	std::u32string errorString = U"No element Availlable";
	
	m_text.clear();
	// to know the size of one line : 
	vec3 minSize = m_text.calculateSize(char32_t('A'));
	if (m_listElement.size() == 0) {
		int32_t paddingSize = 2;
		
		vec2 tmpMax = propertyMaxSize->getPixel();
		
		/*
		if (tmpMax.x() <= 999999) {
			m_text.setTextAlignement(0, tmpMax.x()-2*paddingSize, ewol::compositing::alignLeft);
		}
		*/
		vec3 curentTextSize = m_text.calculateSizeDecorated(errorString);
		
		ivec2 localSize = m_minSize;
		
		// no change for the text orogin : 
		vec3 tmpTextOrigin((m_size.x() - m_minSize.x()) / 2.0,
		                   (m_size.y() - m_minSize.y()) / 2.0,
		                   0);
		
		if (propertyFill->x() == true) {
			localSize.setX(m_size.x());
			tmpTextOrigin.setX(0);
		}
		if (propertyFill->y() == true) {
			localSize.setY(m_size.y());
			tmpTextOrigin.setY(m_size.y() - 2*paddingSize - curentTextSize.y());
		}
		tmpTextOrigin += vec3(paddingSize, paddingSize, 0);
		localSize -= vec2(2*paddingSize,2*paddingSize);
		
		tmpTextOrigin.setY( tmpTextOrigin.y() + (m_minSize.y()-2*paddingSize) - minSize.y());
		
		vec2 textPos(tmpTextOrigin.x(), tmpTextOrigin.y());
		
		vec3 drawClippingPos(paddingSize, paddingSize, -0.5);
		vec3 drawClippingSize((m_size.x() - paddingSize),
		                      (m_size.y() - paddingSize),
		                      1);
		
		// clean the element
		m_text.reset();
		m_text.setDefaultColorFg(etk::color::red);
		m_text.setPos(tmpTextOrigin);
		//APPL_INFO("Regenerate display : " << tmpTextOrigin << "  " << m_origin << "  " << m_size);
		//APPL_VERBOSE("[" << getId() << "] {" << errorString << "} display at pos : " << tmpTextOrigin);
		m_text.setTextAlignement(tmpTextOrigin.x(), tmpTextOrigin.x()+localSize.x(), ewol::compositing::alignLeft);
		m_text.setClipping(drawClippingPos, drawClippingSize);
		m_text.printDecorated(errorString);
		// call the herited class...
		WidgetScrolled::onRegenerateDisplay();
	}
	
	// Here the real Display get a square in pixel of 2cm x 2cm:
	vec2 realPixelSize = gale::Dimension(vec2(3,3), gale::distance::centimeter).getPixel();
	// TODO : Understand Why this not work ...
	realPixelSize = vec2(minSize.y()*4,minSize.y()*4+6); // add arbitrary 6 pixel ...
	// This will generate the number of element that can be displayed:
	int32_t verticalNumber = m_size.y() / realPixelSize.y() + 2; // +1 for the not entire view of element and +1 for the moving element view ...
	//verticalNumber = 10;
	//       APPL_WARNING("We can see " << verticalNumber << " elements: " << realPixelSize);
	// TODO : Remove this ... bad optim ...
	//m_listDisplay.clear();
	if (m_listDisplay.size() != verticalNumber) {
		m_listDisplay.clear();
		for (size_t iii=0; iii<verticalNumber; ++iii) {
			auto elem = ememory::makeShared<ElementDisplayed>();
			m_listDisplay.push_back(elem);
		}
	}
	int32_t offset = m_originScrooled.y() / realPixelSize.y();
	APPL_VERBOSE("origin scrolled : " << m_originScrooled << " nb Pixel/element = " << realPixelSize.y() << " offset=" << offset);
	for (size_t iii=0; iii<m_listDisplay.size(); ++iii) {
		auto elem = m_listDisplay[iii];
		if (elem != nullptr) {
			elem->m_idCurentElement = offset + iii;
			if (offset + iii < m_listElement.size()) {
				elem->m_property = m_listElement[offset + iii];
			} else {
				elem->m_property.reset();
			}
			//switch(iii%6) {
			switch((offset + iii)%6) {
				case 0:
					elem->m_bgColor = etk::color::red;
					break;
				case 1:
					elem->m_bgColor = etk::color::green;
					break;
				case 2:
					elem->m_bgColor = etk::color::orange;
					break;
				case 3:
					elem->m_bgColor = etk::color::purple;
					break;
				case 4:
					elem->m_bgColor = etk::color::gray;
					break;
				case 5:
					elem->m_bgColor = etk::color::cyan;
					break;
				default:
					elem->m_bgColor = etk::color::blue;
					break;
			}
		} else {
			APPL_ERROR("create nullptr");
		}
	}
	
	//display only ofsetted element
	float realOffset = float(int32_t(m_originScrooled.y() / realPixelSize.y())) * realPixelSize.y();
	// Now we request display of the elements:
	vec2 elementSize = vec2(m_size.x(), int32_t(realPixelSize.y()));
	vec2 startPos = vec2(-m_originScrooled.x(), m_size.y()) - vec2(0, elementSize.y()-(m_originScrooled.y()-realOffset));
	for (auto &it : m_listDisplay) {
		if (it == nullptr) {
			startPos -= vec2(0, elementSize.y());
			continue;
		}
		if (    startPos.y() - elementSize.y() <= m_size.y()
		     && startPos.y() + elementSize.y() >= 0) {
			it->generateDisplay(startPos, elementSize);
		} else {
			it->clear();
		}
		startPos -= vec2(0, elementSize.y());
	}
	m_maxSize.setX(m_size.x());
	m_maxSize.setY((float)m_listElement.size()*elementSize.y());
	// call the herited class...
	ewol::widget::WidgetScrolled::onRegenerateDisplay();
}

/*
		public:
			ememory::SharedPtr<appl::ElementProperty> m_property;
			int32_t m_idCurentElement;
			etk::Color<float> m_bgColor;
			ewol::compositing::Image m_image;
			ewol::compositing::Text m_text;
			ewol::compositing::Drawing m_draw;
*/

void appl::ElementDisplayed::generateDisplay(vec2 _startPos, vec2 _size) {
	m_image.clear();
	m_text.clear();
	m_draw.clear();
	if (m_property == nullptr) {
		return;
	}
	//APPL_INFO("Regenrate size :  " << _startPos << " " << _size);
	// --------------------------------------------
	// -- Create a simple border
	// --------------------------------------------
	_startPos += vec2(3,3);
	_size -= vec2(3,3)*2;
	m_draw.setPos(_startPos);
	m_draw.setColor(etk::color::black);
	m_draw.rectangleWidth(_size);
	_startPos += vec2(2,2);
	_size -= vec2(2,2)*2;
	m_draw.setPos(_startPos);
	m_draw.setColor(m_bgColor);
	m_draw.rectangleWidth(_size);
	
	m_pos = _startPos;
	m_size = _size;
	
	std::unique_lock<std::mutex> lock(m_property->m_mutex);
	
	// --------------------------------------------
	// -- Display text...
	// --------------------------------------------
	m_text.reset();
	m_text.setDefaultColorFg(etk::color::black);
	vec3 minSize = m_text.calculateSize(char32_t('A'));
	float borderOffset = 4;
	vec2 originText = _startPos + vec2(_size.y()+borderOffset, _size.y()-minSize.y() - borderOffset);
	m_text.setPos(originText);
	//APPL_INFO("Regenerate display : " << tmpTextOrigin << "  " << m_origin << "  " << m_size);
	//APPL_VERBOSE("[" << getId() << "] {" << errorString << "} display at pos : " << tmpTextOrigin);
	m_text.setTextAlignement(originText.x(), originText.x()+_size.x()-_size.y(), ewol::compositing::alignDisable);
	// TODO: m_text.setClipping(originText, vec2(originText.x()+_size.x()-_size.y(), _size.y()));
	//m_text.setClipping(drawClippingPos, drawClippingSize);
	std::string textToDisplay = "<b>" + m_property->m_title + "</b><br/>";
	bool newLine = false;
	if (m_property->m_serie != "") {
		textToDisplay += "<i>Serie: <b>" + m_property->m_serie + "</b></i><br/>";
	}
	if (m_property->m_saison != "") {
		textToDisplay += "<i>Saison: <b>" + m_property->m_saison + "</b></i> ";
		newLine = true;
	}
	if (m_property->m_episode != "") {
		textToDisplay += "<i>Episode: <b>" + m_property->m_episode + "</b></i> ";
		newLine = true;
	}
	if (m_property->m_type != "") {
		textToDisplay += "    <i>type: <b>" + m_property->m_type + "</b></i> ";
		if (m_property->m_productMethode != "") {
			textToDisplay += " / " + m_property->m_productMethode + " ";
		}
		newLine = true;
	}
	if (newLine == true) {
		textToDisplay += "<br/>";
	}
	textToDisplay += "<i>" + m_property->m_description + "</i>";
	m_text.printDecorated(textToDisplay);
	
	// --------------------------------------------
	// -- Display Image...
	// --------------------------------------------
	if (etk::start_with(m_property->m_mineType, "video") == true) {
		m_image.setSource("DATA:Video.svg", 128);
	} else if (etk::start_with(m_property->m_mineType, "audio") == true) {
		m_image.setSource("DATA:MusicNote.svg", 128);
	} else {
		APPL_INFO("Set image: Unknow type '" << m_property->m_mineType << "'");
		m_image.setSource("DATA:Home.svg", 128);
	}
	m_image.setPos(_startPos+vec2(10,10));
	m_image.print(vec2(_size.y(), _size.y())-vec2(20,20));
}


bool appl::widget::ListViewer::onEventInput(const ewol::event::Input& _event) {
	if (ewol::widget::WidgetScrolled::onEventInput(_event) == true) {
		return true;
	}
	APPL_VERBOSE("Event on BT : " << _event << " size=" << m_size << " maxSize=" << m_maxSize);
	vec2 relativePos = relativePosition(_event.getPos());
	int32_t findId = -1;
	for (size_t iii=0; iii<m_listDisplay.size(); ++iii) {
		if (m_listDisplay[iii] == nullptr) {
			continue;
		}
		if(    relativePos.x() < m_listDisplay[iii]->m_pos.x()
		    || relativePos.y() < m_listDisplay[iii]->m_pos.y()
		    || relativePos.x() > m_listDisplay[iii]->m_pos.x() + m_listDisplay[iii]->m_size.x()
		    || relativePos.y() > m_listDisplay[iii]->m_pos.y() + m_listDisplay[iii]->m_size.y() ) {
			continue;
		}
		findId = iii;
		break;
	}
	if (findId == -1) {
		return false;
	}
	if (_event.getId() == 1) {
		if(_event.getStatus() == gale::key::status::pressSingle) {
			APPL_DEBUG("Select element : " << findId << "  " << m_listDisplay[findId]->m_idCurentElement);
			ememory::SharedPtr<appl::ElementProperty> prop = m_listDisplay[findId]->m_property;
			if (prop != nullptr) {
				std::string fullTitle;
				if (prop->m_serie != "") {
					fullTitle += prop->m_serie + "-";
				}
				if (prop->m_saison != "") {
					fullTitle += "s" + prop->m_saison + "-";
				}
				if (prop->m_episode != "") {
					fullTitle += "e" + prop->m_episode + "-";
				}
				fullTitle += prop->m_title;
				APPL_DEBUG("info element : " << prop->m_id << " title: " << fullTitle);
				signalSelect.emit(prop->m_id);
			}
			return true;
		}
	}
	return false;
}