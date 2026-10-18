#include <Geode/Geode.hpp>
#include <Geode/loader/GameEvent.hpp>
#include <Geode/loader/SettingV3.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/ui/Notification.hpp>

#include "Menu.hpp"

using namespace geode::prelude;

// Geode 5 has built-in keybind settings (see "toggle-key" / "menu-key" in mod.json),
// so the Custom Keybinds dependency is no longer needed.
$on_game(Loaded) {
	listenForKeybindSettingPresses("toggle-key", [](Keybind const&, bool down, bool repeat, double) {
		if (down && !repeat) {
			bool on = !Mod::get()->getSettingValue<bool>("enabled");
			Mod::get()->setSettingValue<bool>("enabled", on);
			Notification::create(
				on ? "Extra Frames: ON" : "Extra Frames: OFF",
				on ? NotificationIcon::Success : NotificationIcon::Info,
				0.8f
			)->show();
		}
	});

	listenForKeybindSettingPresses("menu-key", [](Keybind const&, bool down, bool repeat, double) {
		if (down && !repeat) {
			ExtraFramesPopup::open();
		}
	});
}

// Button on the pause screen for the same menu.
class $modify(ExtraFramesPause, PauseLayer) {
	void customSetup() {
		PauseLayer::customSetup();

		auto menu = this->getChildByID("right-button-menu");
		if (!menu) return;

		auto label = CCLabelBMFont::create("EF", "bigFont.fnt");
		auto spr = CircleButtonSprite::create(label);
		auto btn = CCMenuItemSpriteExtra::create(
			spr, this, menu_selector(ExtraFramesPause::onExtraFrames)
		);
		btn->setID("extra-frames-button"_spr);
		menu->addChild(btn);
		menu->updateLayout();
	}

	void onExtraFrames(CCObject*) {
		ExtraFramesPopup::open();
	}
};
