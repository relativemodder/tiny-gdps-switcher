#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;


std::string str_replace(std::string haystack, std::string needle, std::string replacement) {
	std::string input = std::string(haystack.c_str());
    std::string replace_word = needle; 
    std::string replace_by = replacement; 
  
    size_t pos = input.find(replace_word); 
  
    while (pos != std::string::npos) {
        input.replace(pos, replace_word.size(), replace_by); 

        pos = input.find(replace_word, 
                         pos + replace_by.size()); 
    }

	return input;
}


class $modify(SwitcherMenuLayer, MenuLayer) {
	void onOpenGDPSSwitcher(CCObject*) {
		geode::openSettingsPopup(Mod::get());
	} 


	bool init() {
		if (!MenuLayer::init()) return false;

		auto winSize = CCDirector::get()->getWinSize();

		auto menu = CCMenu::create();

		auto spr = ButtonSprite::create("Tiny GDPS Switcher");

		auto btn = CCMenuItemSpriteExtra::create(
			spr, this, menu_selector(SwitcherMenuLayer::onOpenGDPSSwitcher)
		);

		// some CCMenu*
		menu->addChild(btn);
		addChild(menu);

		menu->setPosition({ winSize.width - 220.f, 164.f });
		menu->setScale(0.6f);

		return true;
	}
};


void proxySend(CCHttpClient* self, CCHttpRequest* req) {
	if (!Mod::get()->getSettingValue<bool>("enabled")) {
		self->send(req);
		return;
	}

	auto new_request_url = std::string(req->getUrl());

	new_request_url = str_replace(
		new_request_url, "http://www.boomlings.com/database", 
		Mod::get()->getSettingValue<std::string>("address")
	);

	new_request_url = str_replace(
		new_request_url, "https://www.boomlings.com/database", 
		Mod::get()->getSettingValue<std::string>("address")
	);

	req->setUrl(new_request_url.c_str());

	self->send(req);
}


$execute {
    Mod::get()->hook(
        reinterpret_cast<void*>(
			geode::addresser::getNonVirtual(&cocos2d::extension::CCHttpClient::send)
        ),
        &proxySend,
        "cocos2d::extension::CCHttpClient::send",
        tulip::hook::TulipConvention::Thiscall
    );
}