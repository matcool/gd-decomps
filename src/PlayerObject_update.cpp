#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

#include "utils.hpp"

class $modify(PlayerObject) {
    void update(float dt) {
		if (from<double>(this, 0x560) > 0.0) {
			double mixFactor = (from<double>(this, 0x688) - from<double>(this, 0x560)) - (double)from<float>(this, 0x56c);
			if (mixFactor < (double)from<float>(this, 0x568)) {
				if (mixFactor <= 0.0 && from<bool>(this, 0x63f)) {
					return;
				}
				this->setColor(GameToolbox::multipliedColorValue(
					from<ccColor3B>(this, 0x570),
					from<ccColor3B>(this, 0x60a),
					(float)(mixFactor / (double)from<float>(this, 0x568))
				));
				this->setSecondColor(GameToolbox::multipliedColorValue(
					from<ccColor3B>(this, 0x573),
					from<ccColor3B>(this, 0x60d),
					(float)(mixFactor / (double)from<float>(this, 0x568))
				));
			} else {
				from<double>(this, 0x560) = 0.0;
				this->setColor(from<ccColor3B>(this, 0x60a));
				this->setSecondColor(from<ccColor3B>(this, 0x60d));
			}
		}

		if (from<bool>(this, 0x63f)) {
			log::info("its set!");
			return;
		}

		// GameObject::m_firstPosition ?
		from<CCPoint>(this, 0x424) = this->getPosition();

		from<float>(this, 0x69c) = 0.0;
		if (!m_isLocked) {
			float dtSlow = dt * 0.9f;
			
			this->updateJump(dtSlow);
			
			if (m_isDashing) {
				m_yVelocity = 0.0;
			}

			float velX = dt * m_xVelocity * m_playerSpeed;
			float velY = dtSlow * m_yVelocity;

			if (m_isDashing) {
				velY = from<float>(this, 0x46c) * velX;
			} else if (m_isDart) {
				velY = (m_isHolding ? 1.0f : -1.0f) * (m_isUpsideDown ? -1.0f : 1.0f) * velX;
				if (m_vehicleSize != 1.0) {
					velY *= 2.0f;
				}
			}

			from<float>(this, 0x69c) = velY;
			CCPoint velocity { velX, velY };
			// PlayerObject::setPosition(const CCPoint&), does a lot of stuff
			this->setPosition(this->getPosition() + velocity);
		}

		// particle system stuff
		if (m_isShip || m_isBird || m_isDart) {
			if (m_isShip) {
				if (!m_isHolding || (m_isInPlayLayer && this->levelFlipping()) || m_isHidden) {
					if (from<bool>(this, 0x53e)) {
						m_shipBoostParticles->stopSystem();
					}
					from<bool>(this, 0x53e) = false;
				} else {
					if (!from<bool>(this, 0x53e)) {
						m_shipBoostParticles->resumeSystem();
					}
					from<bool>(this, 0x53e) = true;
				}
			}
			if (
				!m_isDart && m_isSliding &&
				!m_isLocked && !m_isHidden &&
				(
					(!m_isUpsideDown && m_yVelocity > -1.0) ||
					(m_yVelocity <= 1.0 && m_yVelocity != 1.0)
				)
			) {
				m_vehicleGroundParticles->resumeSystem();
			} else {
				m_vehicleGroundParticles->stopSystem();
			}
		} else {
			if (!m_isSliding || (m_isInPlayLayer && this->levelFlipping() || m_isLocked) || m_isHidden) {
				if (from<bool>(this, 0x53d) && this->getActionByTag(2) == nullptr) {
					// disable ground particles when not on ground
					auto* sequenceAction = cocos2d::CCSequence::create(
						cocos2d::CCDelayTime::create(0.06f),
						cocos2d::CCCallFunc::create(this, callfunc_selector(PlayerObject::deactivateParticle)),
						nullptr
					);
					sequenceAction->setTag(2);
					this->runAction(sequenceAction);
				}
			} else {
				if (!from<bool>(this, 0x53d)) {
					m_playerGroundParticles->resumeSystem();
				}
				from<bool>(this, 0x53d) = true;
				this->stopActionByTag(2);
			}
		}

		m_waveTrail->m_pulseSize = (from<float>(this, 0x694) - 0.1f) * 2.1f + 0.4f;
		if (m_isInPlayLayer) {
			m_waveTrail->clearBehindXPos(GameManager::sharedState()->getPlayLayer()->m_cameraPosition.x);
		}

		// robot particles and flame sprite
		auto* flameSprite = m_robotSprite->m_flameSprite;
		if (!m_isRobot || !m_isHolding || m_canRobotJump || !m_isRising || m_decelerationRate <= 0.27 || 1.5 <= m_decelerationRate) {
			if (flameSprite->isVisible() && flameSprite->getActionByTag(8) == nullptr) {
				flameSprite->stopAllActions();
				auto* sequenceAction = cocos2d::CCSequence::create(
					cocos2d::CCScaleTo::create(0.15f, 0.05f, 0.1f),
					cocos2d::CCHide::create(),
					nullptr
				);
				sequenceAction->setTag(8);
				flameSprite->runAction(sequenceAction);
				m_robotJumpParticles->stopSystem();
			}
		} else {
			if (!flameSprite->isVisible()) {
				flameSprite->stopAllActions();
				flameSprite->setScale(0.1f);
				flameSprite->setVisible(true);
				auto* sequenceAction = cocos2d::CCSequence::create(
					cocos2d::CCScaleTo::create(0.05f, 1.44f, 0.9f),
					cocos2d::CCScaleTo::create(0.05f, 0.72f, 1.35f),
					cocos2d::CCScaleTo::create(0.05f, 1.8f, 0.9f),
					cocos2d::CCScaleTo::create(0.05f, 0.9f, 1.35f),
					cocos2d::CCScaleTo::create(0.05f, 1.8f, 0.9f),
					nullptr
				);
				flameSprite->runAction(sequenceAction);
				m_robotJumpParticles->resumeSystem();
			}
		}
		if (!m_isRobot || !m_isHolding || m_canRobotJump || !m_isRising || m_decelerationRate <= 0.1f || 1.5f <= m_decelerationRate || m_isHidden) {
			if (m_robotJumpParticles->isActive()) {
				m_robotJumpParticles->stopSystem();
			}
		} else {
			if (!m_robotJumpParticles->isActive()) {
				m_robotJumpParticles->resumeSystem();
			}
		}

		from<bool>(this, 0x5b0) = false;

		if (m_ghostTrail != nullptr) {
			CCPoint somePoint { 0.f, 0.f };
			if (m_isSpider) {
				CCPoint pos = m_spiderSprite->m_headSpritePart->getPosition();
				somePoint = ccp(0.f, pos.y * 0.4 + 2.0);
			} else if (m_isRobot) {
				CCPoint pos = m_robotSprite->m_headSpritePart->getPosition();
				somePoint = ccp(0.f, pos.y * 0.4 + 7.0);
			}
			from<CCPoint>(m_ghostTrail, 0x120) = somePoint;
		}

		if (m_isDashing) {
			this->updateDashAnimation();
		}

		// PlayerObject::updateStateVariables()
		from<int>(this, 0x9d0) -= 1;
		from<int>(this, 0x9d4) -= 1;
		from<int>(this, 0x9d8) -= 1;
		from<int>(this, 0x9c8) -= 1;
		// PlayerObject::updateJumpVariables()
		m_isHolding2 = m_isHolding;
		m_hasJustHeld2 = m_hasJustHeld;
		from<bool>(this, 0x615) = false;
		from<bool>(this, 0x616) = false;
	}

    SET_DECOMP_PRIORITY("PlayerObject::update");
};