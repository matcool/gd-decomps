#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

using uchar = unsigned char;
using undefined = unsigned char;
using undefined2 = unsigned short;
using undefined4 = unsigned int;
using uint = unsigned int;
using float10 = long double;
using longlong = long long;

template <class T, class P>
inline __forceinline T& from(P* ptr, intptr_t offset) {
    return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(ptr) + offset);
}

// this->field\d+?_(0x[0-9a-fA-F]+)
// from<void>(this, $1)

float get_time_idk() {
    return time(nullptr) / 1000.f;
}

class $modify(PlayLayer) {
    void update(float delta) {
        m_totalTime = delta + m_totalTime;
        if (!m_debugPauseOff) {
            this->updateVisibility();
            return;
        }
        this->unk508 += delta;
        if (this->unk518 <= 0.0) {
            this->unk518 = get_time_idk();
        }
        if (m_level->m_levelType != GJLevelType::Local) {
            if (!m_shouldTryToKick) {
                if (0.0 < m_inlineCalculatedKickTime) {
                    float local_70 = get_time_idk();
                    double local_98 = (double)local_70 - m_inlineCalculatedKickTime;
                    double dVar17 = cocos2d::CCDirector::sharedDirector()->getActualDeltaTime() + m_accumulatedKickDeltaTime;
                    m_accumulatedKickDeltaTime = dVar17;
                    if ((0.0 <= (double)local_98) && ((double)local_98 <= 100.0)) {
                        if ((double)local_98 <= 2.0)
                            goto LAB_00602be9;
                        if ((double)local_98 <= dVar17 * 1.149999976158142) {
                            m_accumulatedKickCounter--;
                        } else {
                            m_accumulatedKickCounter++;
                        }
                        if (m_accumulatedKickCounter < 0) {
                            m_accumulatedKickCounter = 0;
                        }
                        if (2 < m_accumulatedKickCounter) {
                            m_shouldTryToKick = true;
                            m_kickCheckDeltaSnapshot = ((float)rand() / 32767.0) * 10.0;
                            m_inlineCalculatedKickTime = (double)local_70;
                        }
                        if (m_shouldTryToKick)
                            goto LAB_00602be9;
                    }
                    m_inlineCalculatedKickTime = 0.0;
                } else {
                    m_inlineCalculatedKickTime = get_time_idk();
                }
            } else {
                double dVar17 = get_time_idk();
                if ((double)m_kickCheckDeltaSnapshot < dVar17 - m_inlineCalculatedKickTime) {
                    this->onQuit();
                }
            }
        }

    LAB_00602be9:
        m_effectManager->updateColorEffects(delta);
        m_effectManager->updatePulseEffects(delta);
        m_effectManager->updateOpacityEffects(delta);
        m_effectManager->updateSpawnTriggers(delta);
        m_player1->m_collisionLog->removeAllObjects();
        m_player1->m_collisionLog1->removeAllObjects();
        // not in geode! no idea what they are either..
        from<int>(m_player1, 0x4b8) = 0;
        from<int>(m_player1, 0x4bc) = 0;
        if (m_isDualMode) {
            m_player2->m_collisionLog->removeAllObjects();
            m_player2->m_collisionLog1->removeAllObjects();
            from<int>(m_player2, 0x4b8) = 0;
            from<int>(m_player2, 0x4bc) = 0;
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
        for (int uVar13 = 0; uVar13 < m_screenRingObjects->count(); uVar13++) {
            auto* pCVar7 = (GameObject*)m_screenRingObjects->objectAtIndex(uVar13);
            pCVar7->m_unk36D = false;
        }
        double local_84 = delta * 60.0;
        for (int uVar13 = 0; uVar13 < this->unk358->count(); ++uVar13) {
            // i have no idea what object this is, its never referenced anywhere
            auto* pCVar7 = (CCNode*)this->unk358->objectAtIndex(uVar13);
            pCVar7->update(local_84);
        };
        double local_9c = local_84 * 4.0;
        double local_98;
        double dVar17;
        if (local_9c < 0.0) {
            local_98 = local_9c - 0.5;
            dVar17 = ceil((double)local_98);
        } else {
            local_98 = local_9c + 0.5;
            dVar17 = floor((double)local_98);
        }
        float local_8c = dVar17;
        float fVar16 = 4.0;
        if (4.0 <= (float)local_8c) {
            if ((float)local_9c < 0.0) {
                local_98 = (double)((float)local_9c - 0.5);
                dVar17 = ceil((double)local_98);
            } else {
                local_98 = (double)((float)local_9c + 0.5);
                dVar17 = floor((double)local_98);
            }
            local_9c = (float)dVar17;
            fVar16 = (float)local_9c;
        }
        int iVar14 = (int)fVar16;
        m_player1->m_unk64C = m_player1->getPosition();
        if (m_isDualMode) {
            m_player2->m_unk64C = m_player2->getPosition();
        }
        float fStack_80 = local_84 / fVar16;
        local_9c = delta / (float)iVar14;
        m_effectManager->preCollisionCheck();
        if (0 < iVar14) {
            for (int pPVar12 = 0; pPVar12 < iVar14; ++pPVar12) {
                float pPStack_7c = this->unknown5f4;
                if ((float)pPStack_7c != 0.0) {
                    this->unknown5f4 = 0.0;
                    m_player1->updateTimeMod((float)pPStack_7c);
                    if (m_isDualMode) {
                        m_player2->updateTimeMod((float)pPStack_7c);
                    }
                }
                if (!m_isDead) {
                    m_time = (double)(float)local_9c + m_time;
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
                bool cStack_85 = m_isDualMode;
                float pPVar18 = m_player1->getPosition().y - m_player1->m_firstPosition.y;
                if (!m_isDead) {
                    fVar16 = fabs(pPVar18 - m_player1->m_unk69C);
                    if (fStack_80 * 16.0 < fVar16) {
                        pPVar18 = 0;
                    }
                    dVar17 = (double)m_player1->m_playerSpeed * m_player1->m_xVelocity * (double)fStack_80;
                } else {
                    dVar17 = 0.0;
                }
                m_effectManager->m_velocity = (float)dVar17;
                if (m_isDead) {
                    pPVar18 = 0;
                }
                m_effectManager->m_acceleration = (float)pPVar18;
                m_effectManager->prepareMoveActions(local_9c, pPVar12 < (iVar14 - 1));
                this->processMoveActionsStep(local_9c);
                if (!m_antiCheatPassed && m_antiCheatObject != nullptr) {
                    m_antiCheatObject->setStartPos(ccp(m_antiCheatObject->getStartPos().x, m_player1->getPosition().y));
                }
                this->updateCollisionBlocks();
                if (!m_isDead) {
                    m_player1->update(fStack_80);
                    this->checkCollisions(m_player1, fStack_80);
                    m_player1->updateSpecial((float)local_9c);
                    if ((cStack_85) && (m_isDualMode)) {
                        m_player2->update(fStack_80);
                        this->checkCollisions(m_player2, fStack_80);
                        m_player2->updateSpecial((float)local_9c);
                    }
                    this->checkSpawnObjects();
                }
            }
        }

        m_player1->updateCheckpointTest();
        m_effectManager->postCollisionCheck();
        this->updateQueuedLabels();
        m_player1->updateRotation(local_84);
        if (((((m_isDualMode) &&
               (m_player2->updateRotation(local_84),
                m_isDualMode)) &&
              (m_player1->m_isBall)) &&
             ((
               m_player2->m_isBall &&
                   (!m_levelSettings->m_twoPlayerMode)))) &&
            (m_player1->m_isUpsideDown == m_player2->m_isUpsideDown)) {
            float fVar15 =  m_player1->getScaleX();
            float local_78_x = (float)(m_player1->getScaleX() * (float10)30.0 * (float10)0.5);
            fVar15 = m_player2->getScaleX();
            float local_98 = m_player2->getScaleX() * (float10)30.0 * (float10)0.5;
            float fVar16 = fabs(m_player1->getPosition().y - m_player2->getPosition().y);
            float local_9c = local_78_x;
            if (fVar16 < local_98 + local_78_x + 5.0) {
                if ((m_player1->m_isSliding) ||
                    (m_player2->m_isSliding)) {
                    // this is code for when the two balls touch in the same gravity in dual mode

                    auto* pPVar3 = m_player2;
                    auto* this_03 = pPVar3;
                    auto* local_8c = m_player1;
                    if (!m_player1->m_isSliding && pPVar3->m_isSliding) {
                        this_03 = m_player1;
                        local_8c = pPVar3;
                        local_9c = local_98;
                    }
                    this_03->flipGravity(!this_03->m_isUpsideDown, true);
                    this_03->m_yVelocity = this_03->m_isUpsideDown ? 2.0 : -2.0;
                    auto pCVar8 = local_8c->getPosition();
                    auto* this_02 = CCCircleWave::create(local_9c + 2.0,local_9c * 4.0,0.3,false,false);
                    this_02->m_color = local_8c->m_playerColor1;
                    this_02->m_lineWidth = 4;
                    this_02->setPosition(pCVar8);
                    m_objectLayer->addChild(this_02, 0);
                    this_02->m_filled = 1;
                    this_02->followObject(local_8c, false);
                }
            }
        }
        for (int uVar13 = 0; uVar13 < m_screenRingObjects->count(); ++uVar13) {
            auto* pCVar7 = (GameObject*)m_screenRingObjects->objectAtIndex(uVar13);
            // pCVar7->updateState();
            if (!pCVar7->m_isDontFade) {
                pCVar7->powerOffObject();
            }
        }
        bool bVar4 = false;
        CCPoint CStack_68;
        CCPoint CStack_60;
        if (!m_player1->m_isLocked) {
            auto pCVar8 = m_player1->getPosition();
            auto local_98 = pCVar8;
            m_player1->m_position = m_player1->getPosition();
            if (m_isDualMode) {
                m_player2->m_position = m_player2->getPosition();
            }
            fVar16 = m_mirrorTransition;
            if ((fVar16 != 0.0) && (fVar16 != 1.0)) {
                if (m_cameraFlip == -1.0) {
                    fVar16 = 1.0 - fVar16;
                }
                bVar4 = true;
                auto local_58 = CCPoint(fVar16 * 150.0, 0.0);
                auto pCVar8 = m_player1->getPosition();
                CStack_68 = pCVar8 + local_58;
                if (m_isDualMode) {
                    pCVar8 = m_player2->getPosition();
                    CStack_60 = pCVar8 + local_58;
                }
            }
        }
        this->updateCamera(local_84);
        this->updateVisibility();
        m_player1->m_unk688 = m_totalTime;
        m_player2->m_unk688 = m_totalTime;
        if (!m_isAudioMeteringSupported) {
            auto* this_01 = m_audioEffectsLayer;
            // FIXME: add AudioEffectsLayer members to geode
            from<float>(this_01, 0x1a4) = (float)from<float>(this_01, 0x1a4) + delta;
            if (from<CCArray*>(this_01, 0x1a0) != (CCArray*)0x0) {
                this_01->audioStep(local_84);
            }
        }
        this->updateLevelColors();
        if (bVar4) {
            m_player1->setPosition(CStack_68);
            if (m_isDualMode) {
                m_player2->setPosition(CStack_60);
            }
        }
        this->updateProgressbar();
        this->updateEffectPositions();
    }
};
