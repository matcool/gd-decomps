#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

template <class T, class P>
inline __forceinline T& from(P* ptr, intptr_t offset) {
    return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(ptr) + offset);
}

// this->field\d+?_(0x[0-9a-fA-F]+)
// from<void>(this, $1)

// this is some semi-inlined static function on windows, which does the same as this one i hope.
// besides, the time is only used for the anti cheat so who cares!
float getTimeInSeconds() {
    return time(nullptr);
}

// using members from geode#9fb1c4d

class $modify(PlayLayer) {
    void update(float delta) {
        m_totalTime += delta;
        if (!m_debugPauseOff) {
            this->updateVisibility();
            return;
        }
        this->unk508 += delta;
        if (this->unk518 <= 0.0) {
            this->unk518 = getTimeInSeconds();
        }
        if (m_level->m_levelType != GJLevelType::Local) {
            if (!m_shouldTryToKick) {
                if (m_inlineCalculatedKickTime > 0.0) {
                    const float timeSeconds = getTimeInSeconds();
                    const double timeSinceLastKickAttempt = timeSeconds - m_inlineCalculatedKickTime;
                    m_accumulatedKickDeltaTime = CCDirector::sharedDirector()->getActualDeltaTime() + m_accumulatedKickDeltaTime;
                    if (timeSinceLastKickAttempt >= 0.0 && timeSinceLastKickAttempt <= 100.0) {
                        if (timeSinceLastKickAttempt > 2.0) {
                            if (timeSinceLastKickAttempt <= m_accumulatedKickDeltaTime * 1.15) {
                                m_accumulatedKickCounter--;
                            } else {
                                m_accumulatedKickCounter++;
                            }
                            if (m_accumulatedKickCounter < 0) {
                                m_accumulatedKickCounter = 0;
                            }
                            if (m_accumulatedKickCounter > 2) {
                                m_shouldTryToKick = true;
                                m_kickCheckDeltaSnapshot = (static_cast<float>(rand()) / float(RAND_MAX)) * 10.0;
                                m_inlineCalculatedKickTime = timeSeconds;
                            } else {
                                m_inlineCalculatedKickTime = 0.0;
                            }
                        }
                    } else {
                        m_inlineCalculatedKickTime = 0.0;
                    }
                } else {
                    m_inlineCalculatedKickTime = getTimeInSeconds();
                }
            } else if (m_kickCheckDeltaSnapshot < getTimeInSeconds() - m_inlineCalculatedKickTime) {
                this->onQuit();
            }
        }

        m_effectManager->updateColorEffects(delta);
        m_effectManager->updatePulseEffects(delta);
        m_effectManager->updateOpacityEffects(delta);
        m_effectManager->updateSpawnTriggers(delta);
        m_player1->m_collisionLog->removeAllObjects();
        m_player1->m_collisionLog1->removeAllObjects();
        // no idea what these are..
        m_player1->m_unk4B8 = 0;
        m_player1->m_unk4BC = 0;
        if (m_isDualMode) {
            m_player2->m_collisionLog->removeAllObjects();
            m_player2->m_collisionLog1->removeAllObjects();
            m_player2->m_unk4B8 = 0;
            m_player2->m_unk4BC = 0;
        }
        if (!m_player1->m_isLocked) {
            m_player1->setPosition(m_player1->m_position);
        }
        if (m_isDualMode && !m_player2->m_isLocked) {
            m_player2->setPosition(m_player2->m_position);
        }
        m_player1->m_touchingRings->removeAllObjects();
        if (m_isDualMode) {
            m_player2->m_touchingRings->removeAllObjects();
        }
        for (unsigned int i = 0; i < m_screenRingObjects->count(); i++) {
            auto* object = static_cast<GameObject*>(m_screenRingObjects->objectAtIndex(i));
            object->m_unk36D = false;
        }

        const double delta60 = delta * 60.0;

        for (unsigned int i = 0; i < this->unk358->count(); ++i) {
            // i have no idea what type this is, its never referenced anywhere
            auto* node = static_cast<CCNode*>(this->unk358->objectAtIndex(i));
            node->update(delta60);
        };

        // sub step calculation code:
        // usually gd will always run 4 sub steps per frame,
        // however on fpses < 60 the step count will be greater than 4
        // as to try to match 60 fps physics.
        // for example: 20 fps
        // delta = 1 / 20
        // delta240 = delta * 60 * 4 = 1/20 * 240 = 12
        // 12 < 0 is false so floor(12 + 0.5) = floor(12.5) = 12.0
        // 12.0 is greater than 4, so use that as the number of substeps
        //
        // fpses greater than 60 will never have 240 / fps be greater than 4, 
        // so they will always be 4 steps
        // unless you got delta to be negative in which case it would do the ceil and you could maybe get 5 steps,
        // but good luck trying to get the delta to be negative

        const double delta240 = delta60 * 4.0;
        const float newStepCount = delta240 < 0.0 ? ceil(delta240 - 0.5) : floor(delta240 + 0.5);
        int subStepCount = 4;
        if (newStepCount >= 4.0) {
            subStepCount = static_cast<int>(newStepCount);
        }

        m_player1->m_unk64C = m_player1->getPosition();
        if (m_isDualMode) {
            m_player2->m_unk64C = m_player2->getPosition();
        }

        const float stepDelta60 = delta60 / (float)subStepCount;
        const double stepDelta = delta / (float)subStepCount;

        m_effectManager->preCollisionCheck();
        if (subStepCount > 0) {
            for (int curStep = 0; curStep < subStepCount; curStep++) {
                // this is set every time the player changes speed,
                // only to then get set to 0 and set the actual m_playerSpeed
                // o_O
                const float somePlayerSpeed = this->unknown5f4;
                if (somePlayerSpeed != 0.0) {
                    this->unknown5f4 = 0.0;
                    m_player1->updateTimeMod(somePlayerSpeed);
                    if (m_isDualMode) {
                        m_player2->updateTimeMod(somePlayerSpeed);
                    }
                }
                if (!m_isDead) {
                    m_time += stepDelta;
                    if (!this->unk42A) {
                        if (m_isPaused) {
                            this->saveRecordAction(false, m_player1);
                        }
                    } else {
                        this->saveRecordAction(true, m_player1);
                    }
                    if (!this->unk42C) {
                        if (m_isPlayer2Frozen) {
                            this->saveRecordAction(false, m_player2);
                        }
                    } else {
                        this->saveRecordAction(true, m_player2);
                    }
                    this->updateReplay(m_time);
                }

                m_spawnedGroups->removeAllObjects();

                const bool wasDualMode = m_isDualMode;
                float yDifference = m_player1->getPosition().y - m_player1->m_firstPosition.y;
                if (!m_isDead) {
                    if (stepDelta60 * 16.0 < fabs(yDifference - m_player1->m_unk69C)) {
                        yDifference = 0;
                    }
                    m_effectManager->m_velocity = m_player1->m_playerSpeed * m_player1->m_xVelocity * stepDelta60;
                } else {
                    m_effectManager->m_velocity = 0.0;
                }
                if (m_isDead) {
                    yDifference = 0;
                }
                // is this really acceleration?
                m_effectManager->m_acceleration = yDifference;
                const bool isLastSubStep = curStep == (subStepCount - 1);
                m_effectManager->prepareMoveActions(stepDelta, !isLastSubStep);
                this->processMoveActionsStep(stepDelta);
                if (!m_antiCheatPassed && m_antiCheatObject != nullptr) {
                    m_antiCheatObject->setStartPos(ccp(m_antiCheatObject->getStartPos().x, m_player1->getPosition().y));
                }
                this->updateCollisionBlocks();
                if (!m_isDead) {
                    m_player1->update(stepDelta60);
                    this->checkCollisions(m_player1, stepDelta60);
                    m_player1->updateSpecial(stepDelta);
                    if (wasDualMode && m_isDualMode) {
                        m_player2->update(stepDelta60);
                        this->checkCollisions(m_player2, stepDelta60);
                        m_player2->updateSpecial(stepDelta);
                    }
                    this->checkSpawnObjects();
                }
            }
        }

        m_player1->updateCheckpointTest();
        m_effectManager->postCollisionCheck();
        this->updateQueuedLabels();
        m_player1->updateRotation(delta60);
        if (m_isDualMode) {
            m_player2->updateRotation(delta60);

            const bool bothBall = m_player1->m_isBall && m_player2->m_isBall;
            const bool matchingGravity = m_player1->m_isUpsideDown == m_player2->m_isUpsideDown;
            if (!m_levelSettings->m_twoPlayerMode && bothBall && matchingGravity) {
                const float halfPlayer1Size = m_player1->getScaleX() * 30.f * 0.5f;
                const float halfPlayer2Size = m_player2->getScaleX() * 30.f * 0.5f;
                const float yDistance = fabs(m_player1->getPosition().y - m_player2->getPosition().y);
                if (yDistance < halfPlayer2Size + halfPlayer1Size + 5.0 && (m_player1->m_isSliding || m_player2->m_isSliding)) {
                    // this is code for when the two balls touch in the same gravity in dual mode
                    auto* playerToSwitch = m_player2;
                    auto* otherPlayer = m_player1;
                    float otherPlayerSize = halfPlayer1Size;
                    if (!m_player1->m_isSliding && m_player2->m_isSliding) {
                        playerToSwitch = m_player1;
                        otherPlayer = m_player2;
                        otherPlayerSize = halfPlayer2Size;
                    }
                    playerToSwitch->flipGravity(!playerToSwitch->m_isUpsideDown, true);
                    playerToSwitch->m_yVelocity = playerToSwitch->m_isUpsideDown ? 2.0 : -2.0;
                    auto* circle = CCCircleWave::create(otherPlayerSize + 2.f, otherPlayerSize * 4.f, 0.3f, false, false);
                    circle->m_color = otherPlayer->m_playerColor1;
                    circle->m_lineWidth = 4;
                    circle->setPosition(otherPlayer->getPosition());
                    m_objectLayer->addChild(circle, 0);
                    circle->m_filled = 1;
                    circle->followObject(otherPlayer, false);
                }
            }
        }

        for (unsigned int i = 0; i < m_screenRingObjects->count(); ++i) {
            auto* object = static_cast<GameObject*>(m_screenRingObjects->objectAtIndex(i));
            // inlined GameObject::updateState()
            if (!object->m_unk36D) {
                object->powerOffObject();
            }
        }

        bool isInMirrorTransition = false;
        CCPoint player1TransitionPos;
        CCPoint player2TransitionPos;
        if (!m_player1->m_isLocked) {
            m_player1->m_position = m_player1->getPosition();
            if (m_isDualMode) {
                m_player2->m_position = m_player2->getPosition();
            }
            float mirror = m_mirrorTransition;
            // if currently in transition
            if (mirror != 0.0 && mirror != 1.0) {
                if (m_cameraFlip == -1.0) {
                    mirror = 1.0 - mirror;
                }
                isInMirrorTransition = true;
                const auto transitionOffset = CCPoint(mirror * 150.0, 0.0);
                player1TransitionPos = m_player1->getPosition() + transitionOffset;
                if (m_isDualMode) {
                    player2TransitionPos = m_player2->getPosition() + transitionOffset;
                }
            }
        }

        this->updateCamera(delta60);
        this->updateVisibility();
        
        m_player1->m_unk688 = m_totalTime;
        m_player2->m_unk688 = m_totalTime;
        if (!m_isAudioMeteringSupported) {
            auto* audioEffects = m_audioEffectsLayer;
            audioEffects->m_unk1A4 += delta;
            if (audioEffects->m_unk1A0 != nullptr) {
                audioEffects->audioStep(delta60);
            }
        }
        this->updateLevelColors();
        if (isInMirrorTransition) {
            m_player1->setPosition(player1TransitionPos);
            if (m_isDualMode) {
                m_player2->setPosition(player2TransitionPos);
            }
        }
        this->updateProgressbar();
        this->updateEffectPositions();
    }
};
