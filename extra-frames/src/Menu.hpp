#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>

using namespace geode::prelude;

// Geode 5: Popup is no longer templated; setup lives in init().
class ExtraFramesPopup : public Popup {
protected:
	CCLabelBMFont* m_bufferLabel = nullptr;
	CCLabelBMFont* m_coyoteLabel = nullptr;

	CCMenuItemSpriteExtra* makeStepper(char const* text, int tag) {
		auto spr = ButtonSprite::create(text, 20, true, "bigFont.fnt", "GJ_button_01.png", 25.f, 0.6f);
		auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ExtraFramesPopup::onStep));
		btn->setTag(tag);
		return btn;
	}

	CCLabelBMFont* makeText(char const* text, float scale) {
		auto label = CCLabelBMFont::create(text, "bigFont.fnt");
		label->setScale(scale);
		return label;
	}

	bool init() {
		if (!Popup::init(280.f, 200.f)) return false;

		this->setTitle("Extra Frames");
		auto mod = Mod::get();

		// Row 1: enabled toggle
		m_mainLayer->addChildAtPosition(makeText("Enabled", 0.5f), Anchor::Center, ccp(-60, 40));
		auto toggler = CCMenuItemToggler::createWithStandardSprites(
			this, menu_selector(ExtraFramesPopup::onToggle), 0.8f
		);
		toggler->toggle(mod->getSettingValue<bool>("enabled"));
		m_buttonMenu->addChildAtPosition(toggler, Anchor::Center, ccp(60, 40));

		// Row 2: buffer window
		m_mainLayer->addChildAtPosition(makeText("Buffer", 0.5f), Anchor::Center, ccp(-80, 0));
		m_buttonMenu->addChildAtPosition(makeStepper("-", 1), Anchor::Center, ccp(20, 0));
		m_bufferLabel = makeText("", 0.6f);
		m_mainLayer->addChildAtPosition(m_bufferLabel, Anchor::Center, ccp(60, 0));
		m_buttonMenu->addChildAtPosition(makeStepper("+", 2), Anchor::Center, ccp(100, 0));

		// Row 3: coyote window
		m_mainLayer->addChildAtPosition(makeText("Coyote", 0.5f), Anchor::Center, ccp(-80, -40));
		m_buttonMenu->addChildAtPosition(makeStepper("-", 3), Anchor::Center, ccp(20, -40));
		m_coyoteLabel = makeText("", 0.6f);
		m_mainLayer->addChildAtPosition(m_coyoteLabel, Anchor::Center, ccp(60, -40));
		m_buttonMenu->addChildAtPosition(makeStepper("+", 4), Anchor::Center, ccp(100, -40));

		auto hint = makeText("1 tick = ~4.2ms. Rebind keys in this mod's settings.", 0.28f);
		m_mainLayer->addChildAtPosition(hint, Anchor::Center, ccp(0, -80));

		this->refreshLabels();
		return true;
	}

	void refreshLabels() {
		auto mod = Mod::get();
		m_bufferLabel->setString(fmt::format("{}", mod->getSettingValue<int64_t>("buffer-ticks")).c_str());
		m_coyoteLabel->setString(fmt::format("{}", mod->getSettingValue<int64_t>("coyote-ticks")).c_str());
	}

	void onToggle(CCObject* sender) {
		// The toggler reports its state from before the click.
		auto on = !static_cast<CCMenuItemToggler*>(sender)->isToggled();
		Mod::get()->setSettingValue<bool>("enabled", on);
	}

	void onStep(CCObject* sender) {
		int tag = static_cast<CCNode*>(sender)->getTag();
		bool buffer = tag == 1 || tag == 2;
		int delta = (tag == 1 || tag == 3) ? -1 : 1;
		char const* key = buffer ? "buffer-ticks" : "coyote-ticks";

		auto value = Mod::get()->getSettingValue<int64_t>(key) + delta;
		value = std::clamp<int64_t>(value, 0, 24);
		Mod::get()->setSettingValue<int64_t>(key, value);
		this->refreshLabels();
	}

public:
	static ExtraFramesPopup* create() {
		auto ret = new ExtraFramesPopup();
		if (ret->init()) {
			ret->autorelease();
			return ret;
		}
		delete ret;
		return nullptr;
	}

	static void open() {
		if (auto scene = CCScene::get(); scene && !scene->getChildByID("extra-frames-popup"_spr)) {
			auto popup = create();
			popup->setID("extra-frames-popup"_spr);
			popup->show();
		}
	}
};
