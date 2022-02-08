#include "npc.h"

Npc::Npc() {
    sprite.setTextureRect(sf::IntRect(textureSheetBase.x + (animFrameSet[animFrame] * TILE_SIZE_X), textureSheetBase.y + (facing * TILE_SIZE_Y), TILE_SIZE_X, TILE_SIZE_Y));
}

/// @brief Update
/// @param delta_seconds Time since last update.
void Npc::Update(float delta_seconds) {
    if(moveMode == MOVE_STATIONARY) return;
    //std::cout << delta_seconds << std::endl;
    float speed = NPC_SPEED_DEFAULT * delta_seconds;

    if(!waypoints.empty() && !m_grabbed) {
        if(waypoints.size() == 1) { /// Stationary
            m_distance = Tool::dist(sprite.getPosition(), waypoints[m_currWP].pos);
            if(m_distance > 1) sprite.move(speed *(waypoints[m_currWP].pos/m_distance - sprite.getPosition()/m_distance));
            else moveMode = MOVE_STATIONARY;
        } else { /// Patrol
            m_distance = Tool::dist(sprite.getPosition(), waypoints[m_currWP].pos);
            if(m_distance > 1) {
                //std::cout << "m_currWP: " << m_currWP << " X:" << waypoints[m_currWP].pos.x << " Y:" << waypoints[m_currWP].pos.y << std::endl;
                sprite.move(speed * (waypoints[m_currWP].pos/m_distance - sprite.getPosition()/m_distance));
            } else {
                m_prevWP = m_currWP;

                if(moveMode == MOVE_PATROL_CIRCLE) {
                    if(++m_currWP > (int)waypoints.size() - 1) m_currWP = 0;
                } else if(moveMode == MOVE_PATROL_BACK_AND_FORTH) {
                    //std::cout << "MOVE_PATROL_BACK_AND_FORTH" << std::endl;
                    if (m_moving_forward_on_path) {
                        //std::cout << "Forward: " << m_currWP << std::endl;
                        if(++m_currWP > (int)waypoints.size() - 1) {
                            //std::cout << "Switching to backward path!" << std::endl;
                            m_moving_forward_on_path = false;
                            --m_currWP;
                            --m_currWP;
                        }
                    } else {
                        //std::cout << "Backward: " << m_currWP << std::endl;
                        if(--m_currWP < 0) {
                            //std::cout << "Switching to forward path!" << std::endl;
                            m_moving_forward_on_path = true;
                            ++m_currWP;
                            ++m_currWP;
                        }
                    }
                }


                sf::Vector2f m_facingVec = waypoints[m_currWP].pos - waypoints[m_prevWP].pos;

                if (std::abs(m_facingVec.x) > std::abs(m_facingVec.y)) {
                    if(m_facingVec.x > 0) facing = DIR_EAST;
                    else facing = DIR_WEST;
                } else {
                    if(m_facingVec.y > 0) facing = DIR_SOUTH;
                    else facing = DIR_NORTH;
                }
            }
        }
    }

    animTimer += delta_seconds;
    if (animTimer > 0.25) {
        ++animFrame %= 4;
        animTimer = 0.0f;
        UpdateAppearance();
    }
}

void Npc::Draw(sf::RenderWindow& rw) {
    if(avatar != 41) rw.draw(sprite);
}

void Npc::UpdateAppearance() {
    textureSheetBase.x = ((avatar % NUMER_OF_CHARACTERS_PER_ROW) * CHARACTER_CELLS_X * CHARACTER_TILE_X);
    textureSheetBase.y = ((avatar / NUMER_OF_CHARACTERS_PER_ROW) * CHARACTER_CELLS_Y * CHARACTER_TILE_Y);
    sprite.setTextureRect(sf::IntRect(textureSheetBase.x + (animFrameSet[animFrame] * TILE_SIZE_X), textureSheetBase.y + (facing * TILE_SIZE_Y), TILE_SIZE_X, TILE_SIZE_Y));
    sprite.setOrigin(Tool::centerSprite(sprite));
    sprite.setColor(sf::Color(color.r, color.b, color.g, color.a));
}
