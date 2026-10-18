#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;

// Only cube-style jumping is affected; ship/ufo/wave/etc. are left alone.
static bool isCube(PlayerObject* p) {
	return !(p->m_isShip || p->m_isBird || p->m_isBall || p->m_isDart ||
	         p->m_isRobot || p->m_isSpider || p->m_isSwing);
}

class $modify(ExtraFramesLayer, GJBaseGameLayer) {
	struct Fields {
		bool pending = false;       // a buffered click is waiting for the ground
		bool releaseQueued = false; // player let go while the click was buffered
		int bufferLeft = 0;
		int releaseDelay = 0;
		int airTicks = 0;           // ticks since leaving the ground
		bool jumped = false;        // left the ground by jumping (no coyote jump)
		bool firing = false;        // guard so our own pushButton isn't re-intercepted
	};

	bool active() {
		return Mod::get()->getSettingValue<bool>("enabled") && m_player1 && isCube(m_player1);
	}

	void fire() {
		auto f = m_fields.self();
		f->firing = true;
		GJBaseGameLayer::pushButton(1, true);
		f->firing = false;
		f->jumped = true;
	}

	void processCommands(float dt) {
		auto f = m_fields.self();

		if (active()) {
			bool onGround = m_player1->m_isOnGround;

			if (onGround) {
				f->airTicks = 0;
				f->jumped = false;
			} else {
				f->airTicks++;
			}

			// Deliver a buffered click on landing.
			if (f->pending) {
				if (onGround) {
					fire();
					f->pending = false;
					if (f->releaseQueued) {
						f->releaseDelay = 1; // keep it down one tick so the jump registers
					}
				} else if (--f->bufferLeft <= 0) {
					f->pending = false;
					f->releaseQueued = false;
				}
			}

			// Deferred release of a buffered tap.
			if (f->releaseDelay > 0 && --f->releaseDelay == 0) {
				f->releaseQueued = false;
				f->firing = true;
				GJBaseGameLayer::releaseButton(1, true);
				f->firing = false;
			}
		}

		GJBaseGameLayer::processCommands(dt);
	}

	void pushButton(int button, bool player1) {
		auto f = m_fields.self();
		if (f->firing || !player1 || button != 1 || !active()) {
			return GJBaseGameLayer::pushButton(button, player1);
		}

		if (m_player1->m_isOnGround) {
			f->jumped = true;
			return GJBaseGameLayer::pushButton(button, player1);
		}

		// Coyote time: just walked off a ledge (not jumped), treat as grounded.
		int coyote = Mod::get()->getSettingValue<int64_t>("coyote-ticks");
		if (!f->jumped && f->airTicks <= coyote) {
			m_player1->m_isOnGround = true;
			f->jumped = true;
			return GJBaseGameLayer::pushButton(button, player1);
		}

		// Input buffering: remember the click until we land.
		int buffer = Mod::get()->getSettingValue<int64_t>("buffer-ticks");
		if (buffer > 0) {
			f->pending = true;
			f->releaseQueued = false;
			f->bufferLeft = buffer;
			return; // swallow for now
		}

		GJBaseGameLayer::pushButton(button, player1);
	}

	void releaseButton(int button, bool player1) {
		auto f = m_fields.self();
		if (!f->firing && player1 && button == 1 && f->pending) {
			f->releaseQueued = true; // finish the tap after the buffered jump fires
			return;
		}
		GJBaseGameLayer::releaseButton(button, player1);
	}
};
