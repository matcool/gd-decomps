#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

#include "utils.hpp"

// using members from geode#7e60770

class $modify(PlayerObject) {
    void updateJump(float dt) {
        // why
        this->m_unk620 = this->m_isOnGround ? 213 : 0;
        // onGroundRand?
        this->m_unk61C = (rand() / 32767.0) * 1000.0;
        // onGroundSeed?
        this->m_unk618 = this->m_unk620 + this->m_unk61C;

        float localGravity = this->m_gravity;
        if (this->m_isBall || this->m_isShip || this->m_isBird || this->m_isDart || this->m_isSpider) {
            localGravity = 0.958199;
        }

        const bool isMini = this->m_vehicleSize != 1.0;

        float playerSize = isMini ? 0.8 : 1.0;

        const int flipGravityMult = this->m_isUpsideDown ? -1 : 1;

        if (this->m_isShip || this->m_isBird || this->m_isDart) {
            if (isMini) {
                playerSize = 0.85;
            }
            // limit for velocity going up
            float upperVelocityLimit = 8.0 / playerSize;
            // limit for velocity going down
            float lowerVelocityLimit = -6.4 / playerSize;
            
            if (!this->m_isUpsideDown) {
                if (
                    (this->m_yVelocity >= 0.0 && this->m_yVelocity < upperVelocityLimit)
                    || (this->m_yVelocity <= 0.0 && this->m_yVelocity > lowerVelocityLimit)
                ) {
                    this->m_hasHitPortal = false;
                }
            } else if (
                (this->m_yVelocity <= 0.0 && -upperVelocityLimit < this->m_yVelocity)
                || (this->m_yVelocity >= 0.0 && this->m_yVelocity < -lowerVelocityLimit)
            ) {
                this->m_hasHitPortal = false;
            }

            if (this->m_isShip) {
                // ship
                float shipAccel = 0.8;
                
                if (this->m_hasHitPortal) {
                    if (!this->m_isUpsideDown) {
                        if (this->m_yVelocity < 0.0) {
                            shipAccel = -1.0;
                        }
                    } else if (this->m_yVelocity > 0.0) {
                        shipAccel = -1.0;
                    }
                } else if (this->m_isHolding) {
                    shipAccel = -1.0;
                }

                if (!this->m_isHolding && !this->playerIsFalling()) {
                    shipAccel = 1.2;
                }
                float extraBoost = 0.4;
                if (this->m_isHolding && this->playerIsFalling()) {
                    extraBoost = 0.5;
                }
                this->m_yVelocity -= localGravity * dt * flipGravityMult * shipAccel * extraBoost / playerSize;

                if (this->m_isHolding) {
                    this->m_isSliding = false;
                }
            } else if (this->m_isDart) {
                // wave
                int holdingMult = this->m_isHolding ? 1 : -1;
                this->m_yVelocity = this->m_playerSpeed * this->m_xVelocity * flipGravityMult * holdingMult;
            } else {
                // ufo
                if (this->m_hasJustHeld && this->m_isHolding) {
                    this->m_hasJustHeld = false;

                    const float sizeMultiplier = isMini ? 8.0 : 7.0;
                    float newVelocity = flipGravityMult * sizeMultiplier * playerSize;

                    if (!this->m_isUpsideDown && this->m_yVelocity < newVelocity || newVelocity < this->m_yVelocity) {
                        this->m_yVelocity = newVelocity;

                        if ((this->m_wasOnSlope || this->m_isOnSlope) && (this->m_unk634 > 0.0)) {
                            this->m_yVelocity = std::min(this->m_yVelocity + this->m_unk634 * 0.5, newVelocity * 1.4);
                        }
                    }
                    this->playBurstEffect();
                }

                float gravityMult = 0.8;
                if (!this->playerIsFalling()) {
                    gravityMult = 1.2;
                }
                this->m_yVelocity -= localGravity * dt * flipGravityMult * gravityMult * 0.5 / playerSize;
                if (this->m_isHolding) {
                    this->m_isSliding = false;
                }
            }
            if (this->m_hasHitPortal) {
                return;
            }
            if (this->m_isDart) {
                return;
            }
            if (!this->m_isUpsideDown) {
                if (this->m_yVelocity <= lowerVelocityLimit) {
                    this->m_yVelocity = lowerVelocityLimit;
                }
            } else {
                if (this->m_yVelocity <= -upperVelocityLimit) {
                    this->m_yVelocity = -upperVelocityLimit;
                }
                upperVelocityLimit = 6.4 / playerSize;
            }
            if (this->m_yVelocity >= upperVelocityLimit) {
                this->m_yVelocity = upperVelocityLimit;
            }
        } else {
            // not ship wave or ufo
            // so cube, robot, ball, spider

            float gravityMultiplier = 1.0;

            if (this->m_isBall) {
                gravityMultiplier = 0.6;
            } else if (this->m_isSpider) {
                gravityMultiplier = 0.6;
            } else if (this->m_isRobot) {
                gravityMultiplier = 0.9;
            }

            bool shouldJump = this->m_isHolding;
            if (this->m_isRobot) {
                shouldJump = this->m_hasJustHeld && this->m_isHolding;
            }
            
            if (shouldJump && this->m_isOnGround && !this->m_isDashing) {
                if (this->m_isSpider) {
                    this->spiderTestJump(false);
                } else {
                    this->m_isSliding = false;
                    this->m_isRising = true;

                    this->m_isOnGround = false;
                    this->m_hasJustHeld = false;
                    this->m_canRobotJump = false;
                    this->m_decelerationRate = 0.0;

                    float jumpAccel = this->m_jumpAccel;
                    if (this->m_isRobot) {
                        jumpAccel *= 0.5;
                    }
                    // oh hey its the rand() variables from the beginning.. why..
                    // like in theory this should never run, the code at the beginning
                    // of the function sets this to 213 if m_isOnGround and this
                    // only runs if m_isOnGround is true, and no code in between sets it to true
                    if (this->m_unk620 != 213) {
                        jumpAccel = 9999.0;
                        this->m_hasHitPortal = true;
                    }
                    this->m_yVelocity = flipGravityMult * jumpAccel * playerSize;

                    if (this->m_wasOnSlope || this->m_isOnSlope) {
                        if (flipGravityMult * this->m_unk634 > 0.0) {
                            double scaledVelocityIDK = this->m_yVelocity * 1.4;

                            float slopeVel = this->getModifiedSlopeYVel();
                            if (this->m_isBall) {
                                slopeVel = this->m_unk634;
                            }
                            this->m_yVelocity += slopeVel * 0.25;
                            if (!this->m_isUpsideDown) {
                                this->m_yVelocity = std::min(this->m_yVelocity, scaledVelocityIDK);
                            } else {
                                this->m_yVelocity = std::max(this->m_yVelocity, scaledVelocityIDK);
                            }
                        }
                    }
                    
                    // has jumped?
                    this->m_unk53B = true;
                    this->m_lastJumpTime = getTimeInSeconds();
                    this->incrementJumps();
                    
                    if (this->m_isBall) {
                        this->flipGravity(!this->m_isUpsideDown,true);
                        this->m_unk53F = false;
                        this->m_isHolding = false;
                        this->m_yVelocity *= 0.6;
                    } else if (this->m_isRobot) {
                        this->m_robotSprite->tweenToAnimation("jump_loop", 0.1);
                    } else if (!this->m_isSpider) {
                        // should only get here when cube
                        this->runRotateAction();
                    }
                }
                // return;
            } else {
                float flippedGravityIfIDKThing = this->m_gravity;
                // unk685 = is on dual? ive only seen it true there
                if (this->m_unk685) {
                    flippedGravityIfIDKThing = -this->m_gravity;
                }
                if (this->m_isRising) {
                    if (!this->m_isRobot || !this->m_isHolding || this->m_canRobotJump || 1.5 <= this->m_decelerationRate) {
                        this->m_yVelocity -= localGravity * dt * flipGravityMult * gravityMultiplier;
                    } else {
                        this->m_decelerationRate += dt * 0.1;
                    }
                    bool condition;
                    if (!this->m_isUpsideDown) {
                        condition = this->m_yVelocity <= flippedGravityIfIDKThing * 2.0;
                    } else {
                        condition = this->m_yVelocity >= this->m_gravity * 2.0;
                    }
                    if (condition) {
                        this->m_isSliding = false;
                        this->m_isRising = false;

                        this->m_unk53A = true;
                        if (this->m_isRobot) {
                            this->m_robotSprite->tweenToAnimation("fall_loop", 0.1);
                        } else if (this->m_isSpider) {
                            this->m_spiderSprite->tweenToAnimation("fall_loop", 0.1);
                        }
                    }
                } else {
                    if (!this->m_isUpsideDown) {
                        if (this->m_yVelocity < flippedGravityIfIDKThing * 2.0) {
                            this->m_isOnGround = false;
                        }
                    } else {
                        if (this->m_yVelocity > this->m_gravity * 2.0) {
                            this->m_isOnGround = false;
                        }
                    }
                    this->m_yVelocity -= localGravity * dt * flipGravityMult * gravityMultiplier;
                    if (!this->m_isUpsideDown) {
                        this->m_yVelocity = std::max(this->m_yVelocity, -15.0);
                    } else {
                        this->m_yVelocity = std::min(this->m_yVelocity, 15.0);
                    }
                    if (!this->m_isUpsideDown) {
                        if (this->m_yVelocity >= flippedGravityIfIDKThing * 2.0) {
                            return;
                        }
                    } else {
                        if (this->m_yVelocity <= this->m_gravity * 2.0) {
                            return;
                        }
                    }
                    if (!this->m_isBall && !this->m_isSpider && !this->m_isRobot
                        && this->getActionByTag(0) == nullptr && !this->m_isOnSlope && !this->m_unk4B0) {
                        this->runRotateAction();
                    }
                    if (!this->m_isUpsideDown) {
                        if (this->m_yVelocity >= -4.0) {
                            return;
                        }
                    } else {
                        if (this->m_yVelocity <= 4.0) {
                            return;
                        }
                    }
                    if (this->m_isSliding && !this->m_unk5B0) {
                        if (this->m_isRobot) {
                            this->m_robotSprite->tweenToAnimation("fall_loop", 0.1);
                        } else if (this->m_isSpider) {
                            this->m_spiderSprite->tweenToAnimation("fall_loop", 0.1);
                        }
                    }
                    this->m_isSliding = false;
                }
            }
        }
    }
    SET_DECOMP_PRIORITY("PlayerObject::updateJump");
};