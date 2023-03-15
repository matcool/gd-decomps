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
        // local_44 = DAT_0071e000 ^ (uint)auStack_b4;
        this->m_totalTime = delta + this->m_totalTime;
        // local_78.x = (float)this;
        if (m_debugPauseOff == false) {
            this->updateVisibility();
            return;
        }
        this->unk508 = (double)delta + this->unk508;
        double dVar17 = 0.0;
        if (this->unk518 <= 0.0) {
            this->unk518 = get_time_idk();
        }
        if (this->m_level->m_levelType != GJLevelType::Local) {
            if (m_shouldTryToKick == false) {
                if (0.0 < m_inlineCalculatedKickTime) {
                    float local_70 = get_time_idk();
                    double local_98 = (double)local_70 - m_inlineCalculatedKickTime;
                    auto* pCVar5 = cocos2d::CCDirector::sharedDirector();
                    // is this director->getDeltaTime() ?
                    dVar17 = (double)*(float*)(pCVar5 + 0x68) + m_accumulatedKickDeltaTime;
                    m_accumulatedKickDeltaTime = dVar17;
                    if ((0.0 <= (double)local_98) && ((double)local_98 <= 100.0)) {
                        if ((double)local_98 <= 2.0)
                            goto LAB_00602be9;
                        if ((double)local_98 <= dVar17 * 1.149999976158142) {
                            m_accumulatedKickCounter = m_accumulatedKickCounter + -1;
                        } else {
                            m_accumulatedKickCounter = m_accumulatedKickCounter + 1;
                        }
                        if (m_accumulatedKickCounter < 0) {
                            m_accumulatedKickCounter = 0;
                        }
                        if (2 < m_accumulatedKickCounter) {
                            m_shouldTryToKick = true;
                            int iVar14 = rand();
                            m_kickCheckDeltaSnapshot = ((float)iVar14 / 32767.0) * 10.0;
                            m_inlineCalculatedKickTime = (double)local_70;
                        }
                        if (m_shouldTryToKick != false)
                            goto LAB_00602be9;
                    }
                    m_inlineCalculatedKickTime = 0.0;
                } else {
                    m_inlineCalculatedKickTime = get_time_idk();
                }
            } else {
                dVar17 = get_time_idk();
                if ((double)m_kickCheckDeltaSnapshot < dVar17 - m_inlineCalculatedKickTime) {
                    this->onQuit();
                }
            }
        }

    LAB_00602be9:
        auto* this_00 = m_effectManager;
        this_00->updateColorEffects(delta);
        this_00->updatePulseEffects(delta);
        this_00->updateOpacityEffects(delta);
        this_00->updateSpawnTriggers(delta);
        auto* pPVar12 = this->m_player1;
        pPVar12->m_collisionLog->removeAllObjects();
        pPVar12->m_collisionLog1->removeAllObjects();
        // not in geode! no idea what they are either..
        from<int>(pPVar12, 0x4b8) = 0;
        from<int>(pPVar12, 0x4bc) = 0;
        if (this->m_isDualMode != false) {
            pPVar12 = this->m_player2;
            pPVar12->m_collisionLog->removeAllObjects();
            pPVar12->m_collisionLog1->removeAllObjects();
            from<int>(pPVar12, 0x4b8) = 0;
            from<int>(pPVar12, 0x4bc) = 0;
        }
        pPVar12 = this->m_player1;
        if (pPVar12->m_isLocked == false) {
            // cocos2d::CCPoint::CCPoint(&local_98, &pPVar12->m_position);
            // (**(code**)(*(int*)&((this->m_player1)->field0_0x0).inherit.field0_0x0 +
            //             0x5c))();
            m_player1->setPosition(pPVar12->m_position);
        }
        if ((this->m_isDualMode != false) &&
            (pPVar12 = this->m_player2, pPVar12->m_isLocked == false)) {
            // cocos2d::CCPoint::CCPoint(&local_98, &pPVar12->m_position);
            // (**(code**)(*(int*)&((this->m_player2)->field0_0x0).inherit.field0_0x0 +
            //             0x5c))();
            m_player2->setPosition(pPVar12->m_position);
        }
        (this->m_player1)->m_touchingRings->removeAllObjects();
        if (this->m_isDualMode != false) {
            (this->m_player2)->m_touchingRings->removeAllObjects();
        }
        int uVar13 = 0;
        int uVar6 = this->m_screenRingObjects->count();
        if (uVar6 != 0) {
            do {
                auto* pCVar7 = this->m_screenRingObjects->objectAtIndex(uVar13);
                uVar13 = uVar13 + 1;
                // *(undefined*)((int)&pCVar7[0x1b].m_nTag + 1) = 0;
                uVar6 = this->m_screenRingObjects->count();
            } while (uVar13 < uVar6);
        }
        uVar13 = 0;
        uVar6 = this->unk358->count();
        if (uVar6 != 0) {
            do {
                auto* pCVar7 = this->unk358->objectAtIndex(uVar13);
                // (**(code**)((int)pCVar7->field0_0x0 + 0x10))();
                uVar13 = uVar13 + 1;
                uVar6 = this->unk358->count();
            } while (uVar13 < uVar6);
        }
        double local_84 = delta * 60.0;
        double local_9c = local_84 * 4.0;
        double local_98;
        if (local_9c < 0.0) {
            local_98 = local_9c - 0.5;
            // pCStack_cc = (CCNode*)0x602de8;
            dVar17 = ceil((double)local_98);
        } else {
            local_98 = local_9c + 0.5;
            // pCStack_cc = (CCNode*)0x602dc9;
            dVar17 = floor((double)local_98);
        }
        float local_8c = dVar17;
        float fVar16 = 4.0;
        if (4.0 <= (float)local_8c) {
            if ((float)local_9c < 0.0) {
                local_98 = (double)((float)local_9c - 0.5);
                // pCStack_cc = (CCNode*)0x602e4b;
                dVar17 = ceil((double)local_98);
            } else {
                local_98 = (double)((float)local_9c + 0.5);
                // pCStack_cc = (CCNode*)0x602e2c;
                dVar17 = floor((double)local_98);
            }
            local_9c = (float)dVar17;
            fVar16 = (float)local_9c;
        }
        int iVar14 = (int)fVar16;
        // local_98 = (CCPoint)CONCAT44(local_98.y, iVar14);
        // pCVar8 = (CCPoint*)(**(
        //     code**)(*(int*)&((this->m_player1)->field0_0x0).inherit.field0_0x0 +
        //             100))();
        
        CCPoint local_70 = m_player1->getPosition();
        this->m_player1->m_unk64C = local_70;
        if (this->m_isDualMode) {
            local_70 = m_player2->getPosition();
            this->m_player2->m_unk64C = local_70;
        }
        float fStack_80 = local_84 / fVar16;
        local_9c = delta / (float)iVar14;
        m_effectManager->preCollisionCheck();
        local_8c = 0;
        if (0 < iVar14) {
            // local_70 = (CCPoint)((ulonglong)local_70 & 0xffffffff00000000 | (ulonglong)(iVar14 - 1));
            do {
                float pPVar12 = local_8c;
                float pPStack_7c = this->unknown5f4;
                if ((float)pPStack_7c != 0.0) {
                    this->unknown5f4 = 0.0;
                    this->m_player1->updateTimeMod((float)pPStack_7c);
                    if (this->m_isDualMode != false) {
                        this->m_player2->updateTimeMod((float)pPStack_7c);
                    }
                }
                if (this->m_isDead == false) {
                    this->m_time = (double)(float)local_9c + this->m_time;
                    if (this->unk42A == false) {
                        if (this->m_isPaused != false) {
                            this->saveRecordAction(false, m_player1);
                        }
                    } else {
                        this->saveRecordAction(true, m_player1);
                    }
                    if (this->unk42C == false) {
                        if (this->m_isPlayer2Frozen) {
                            this->saveRecordAction(false, m_player2);
                        }
                    } else {
                        this->saveRecordAction(true, m_player2);
                    }
                    this->updateReplay(m_time);
                }
                m_spawnedGroups->removeAllObjects();
                auto* pPVar3 = this->m_player1;
                bool cStack_85 = this->m_isDualMode;
                CCPoint pCVar9 = pPVar3->getPosition();
                float pPVar18 = pCVar9.y - pPVar3->m_firstPosition.y;
                // pPStack_7c = (PlayLayer *)pPVar19;
                if (this->m_isDead == false) {
                    pPVar3 = this->m_player1;
                    fVar16 = fabs(pPVar18 - pPVar3->m_unk69C);
                    if (fStack_80 * 16.0 < fVar16) {
                        pPVar18 = 0;
                    }
                    dVar17 = (double)pPVar3->m_playerSpeed * pPVar3->m_xVelocity * (double)fStack_80;
                } else {
                    dVar17 = 0.0;
                }
                (this->m_effectManager)->m_velocity = (float)dVar17;
                if (this->m_isDead != false) {
                    pPVar18 = 0;
                }
                this->m_effectManager->m_acceleration = (float)pPVar18;
                this->m_effectManager->prepareMoveActions(local_9c, (int)pPVar12 < (int)local_70.x);
                this->processMoveActionsStep(local_9c);
                if ((m_antiCheatPassed == false) &&
                    (pPVar12 = local_8c, m_antiCheatObject != nullptr)) {
                    // iVar14 = *(int*)&(m_antiCheatObject->inherit).field0_0x0;
                    // pPVar12 = this->m_player1;
                    // pfVar9 = (float*)(**(
                    //     code**)(*(int*)&(m_antiCheatObject->inherit).field0_0x0 + 0x2b0))();
                    auto iVar10 = m_antiCheatObject->getStartPos();
                    // cocos2d::CCPoint::CCPoint((CCPoint*)&pCStack_cc, *pfVar9, *(float*)(iVar10 + 4));
                    // this = pPStack_7c;
                    m_antiCheatObject->setStartPos(CCPoint(iVar10.x, m_player1->getPosition().y));
                    pPVar12 = local_8c;
                }
                this->updateCollisionBlocks();
                if (this->m_isDead == false) {
                    pPVar3 = this->m_player1;
                    m_player1->update(fStack_80);
                    this->checkCollisions(this->m_player1, fStack_80);
                    this->m_player1->updateSpecial((float)local_9c);
                    if ((cStack_85 != false) && (this->m_isDualMode != false)) {
                        m_player2->update(fStack_80);
                        this->checkCollisions(this->m_player2, fStack_80);
                        this->m_player2->PlayerObject::updateSpecial((float)local_9c);
                    }
                    this->checkSpawnObjects();
                }
                local_8c = (float)((int)pPVar12 + 1);
            } while ((int)local_8c < iVar14);
        }

        this->m_player1->updateCheckpointTest();
        this->m_effectManager->postCollisionCheck();
        this->updateQueuedLabels();
        this->m_player1->updateRotation(local_84);
        if (((((this->m_isDualMode != false) &&
               (this->m_player2->updateRotation(local_84),
                this->m_isDualMode != false)) &&
              (pPVar12 = this->m_player1, pPVar12->m_isBall != false)) &&
             ((
               this->m_player2->m_isBall != false &&
                   ((this->m_levelSettings)->m_twoPlayerMode == false)))) &&
            (pPVar12->m_isUpsideDown == this->m_player2->m_isUpsideDown)) {
            float fVar15 =  pPVar12->getScaleX(); // (float10)(**(code**)(*(int*)&(pPVar12->field0_0x0).inherit.field0_0x0 + 0x40))();
            float local_78_x = (float)(pPVar12->getScaleX() * (float10)30.0 * (float10)0.5);
            fVar15 = m_player2->getScaleX(); //(float10)(**(code**)(*(int*)&((this->m_player2)->field0_0x0).inherit.field0_0x0 +0x40))();
            local_98 = m_player2->getScaleX() * (float10)30.0 * (float10)0.5;
            // iVar14 = (**(code**)(*(int*)&((this->m_player1)->field0_0x0).inherit.field0_0x0 + 100))();
            // iVar10 = (**(code**)(*(int*)&((this->m_player2)->field0_0x0).inherit.field0_0x0 + 100))();
            // fVar16 = *(float*)(iVar14 + 4) - *(float*)(iVar10 + 4);
            float fVar16 = fabs(m_player1->getPosition().x - m_player2->getPosition().x);
            float local_9c = local_78_x;
            if (fVar16 < local_98 + local_78_x + 5.0) {
                pPVar12 = this->m_player1;
                if ((pPVar12->m_isSliding != false) ||
                    ((this->m_player2)->m_isSliding != false)) {
                    // this is code for when the two balls touch in the same gravity in dual mode

                    auto* pPVar3 = this->m_player2;
                    auto* this_03 = pPVar3;
                    auto* local_8c = pPVar12;
                    if ((pPVar12->m_isSliding == false) && (pPVar3->m_isSliding != false)) {
                        this_03 = pPVar12;
                        local_8c = pPVar3;
                        local_9c = local_98;
                    }
                    // pCStack_cc = (CCNode*)0x603376;
                    this_03->flipGravity(this_03->m_isUpsideDown == false, true);
                    this_03->m_yVelocity = (double)(((uint)(this_03->m_isUpsideDown == false) * 2 + -1) * -2);
                    auto pCVar8 = local_8c->getPosition();
                    // cocos2d::CCPoint::CCPoint(&local_78, pCVar8);
                    auto* this_02 = CCCircleWave::create(local_9c + 2.0,local_9c * 4.0,0.3,false,false);
                    this_02->m_color = local_8c->m_playerColor1;
                    this_02->m_lineWidth = 4;
                    this_02->setPosition(pCVar8);
                    this->m_objectLayer->addChild(this_02, 0);
                    this_02->m_filled = 1;
                    if (this_02->m_child != nullptr) {
                        this_02->m_child->release();
                        this_02->m_child = nullptr;
                    }
                    // this is just a guess tho
                    // this_02->m_children = ?????; // *(float*)(this_02 + 1) = local_98.x;
                    this_02->m_child = local_8c;
                    local_8c->retain();
                    this_02->unschedule(schedule_selector(CCCircleWave::updatePosition));
                    this_02->schedule(schedule_selector(CCCircleWave::updatePosition));
                    this_02->setPosition(this_02->m_child->getPosition());
                }
            }
        }
        uVar13 = 0;
        uVar6 = this->m_screenRingObjects->count();
        if (uVar6 != 0) {
            do {
                auto* pCVar7 = (GameObject*)this->m_screenRingObjects->objectAtIndex(uVar13);
                if (pCVar7->m_isDontFade == false) {
                    pCVar7->powerOffObject();
                    // (**(code**)((int)pCVar7->field0_0x0 + 0x27c))();
                }
                uVar13 = uVar13 + 1;
                uVar6 = this->m_screenRingObjects->count();
            } while (uVar13 < uVar6);
        }
        bool bVar4 = false;
        CCPoint CStack_68;
        CCPoint CStack_60;
        pPVar12 = this->m_player1;
        if (pPVar12->m_isLocked == false) {
            auto pCVar8 = this->m_player1->getPosition(); //(CCPoint*)(**(code**)(*(int*)&(pPVar12->field0_0x0).inherit.field0_0x0 + 100))();
            auto local_98 = pCVar8;
            m_player1->m_position = this->m_player1->getPosition();
            if (this->m_isDualMode != false) {
                // pCVar8 = (CCPoint*)(**(code**)(*(int*)&((this->m_player2)->field0_0x0).inherit.field0_0x0 + 100))();
                // cocos2d::CCPoint::CCPoint(&local_98, pCVar8);
                // cocos2d::CCPoint::operator=(&(this->m_player2)->m_position, &local_98);
                m_player2->m_position = m_player2->getPosition();
            }
            fVar16 = this->m_mirrorTransition;
            if ((fVar16 != 0.0) && (fVar16 != 1.0)) {
                if (this->m_cameraFlip == -1.0) {
                    fVar16 = 1.0 - fVar16;
                }
                bVar4 = true;
                // auto local_98 = fVar16 * 150.0;
                // pCStack_cc = (CCNode*)0x6035c0;
                auto local_58 = CCPoint(fVar16 * 150.0, 0.0);
                auto pCVar8 = m_player1->getPosition(); // (CCPoint*)(**(code**)(*(int*)&((this->m_player1)->field0_0x0).inherit.field0_0x0 + 100))();
                // cocos2d::CCPoint::operator+(pCVar8, &local_78);
                // cocos2d::CCPoint::operator=(&CStack_68, &local_78);
                CStack_68 = pCVar8 + local_58;
                if (this->m_isDualMode != false) {
                    pCVar8 = m_player2->getPosition(); //(CCPoint*)(**(code**)(*(int*)&((this->m_player2)->field0_0x0).inherit.field0_0x0 + 100))();
                    CStack_60 = pCVar8 + local_58;
                }
            }
        }
        this->updateCamera(local_84);
        this->updateVisibility();
        this->m_player1->m_unk688 = this->m_totalTime;
        this->m_player2->m_unk688 = this->m_totalTime;
        if (this->m_isAudioMeteringSupported == false) {
            auto* this_01 = this->m_audioEffectsLayer;
            // FIXME: add AudioEffectsLayer members to geode
            from<float>(this_01, 0x1a4) = (float)from<float>(this_01, 0x1a4) + delta;
            if (from<CCArray*>(this_01, 0x1a0) != (CCArray*)0x0) {
                this_01->audioStep(local_84);
            }
        }
        this->updateLevelColors();
        if (bVar4) {
            this->m_player1->setPosition(CStack_68);
            if (this->m_isDualMode != false) {
                this->m_player2->setPosition(CStack_60);
            }
        }
        this->updateProgressbar();
        this->updateEffectPositions();
    }
};
