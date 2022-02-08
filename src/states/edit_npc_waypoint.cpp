#include "states/edit_npc_waypoint.h"

EditNpcWaypoint EditNpcWaypoint::m_EditNpcWaypoint;

void EditNpcWaypoint::Init(Engine* e) {
    e->m_RenderWindow.setMouseCursorVisible(true);

    m_Boxes.getBoxes().clear();
    m_Boxes.addBox("Desc", sf::Vector2f(10, 10), sf::Vector2f(50, 50), Box::BOX_NONE);
    m_Boxes.getBox("Desc").addText("NPC/Waypoint Editor", sf::Vector2f(0, 0), 32);
    m_Boxes.getBox("Desc").addText("Left click to manage waypoints", sf::Vector2f(0, 40), 24);
    m_Boxes.getBox("Desc").addText("Right click to manage NPCs", sf::Vector2f(0, 70), 24);
    m_Boxes.getBox("Desc").addText("Single click to edit", sf::Vector2f(0, 100), 24);
    m_Boxes.getBox("Desc").addText("Double click to create a new entity", sf::Vector2f(0, 130), 24);

    m_Boxes.addBox("Tips", sf::Vector2f(10, 10), sf::Vector2f(TIP_BOX_LEFT, TIP_BOX_TOP), Box::BOX_NONE);

    LoadWaypoints(e);
    m_npcPtr = nullptr;
    m_wp_color = 0;

    if(!avatar_sheet_texture.loadFromFile("graphics/avatar_sheet.png"))
        std::cout << "Failed to load: graphics/avatar_sheet.png" << std::endl;
    avatar_sheet.setTexture(avatar_sheet_texture);
    avatar_sheet.setPosition(500, 650);
    //avatar_sheet.setĵ
}

void EditNpcWaypoint::Cleanup(Engine* e) {
    e->m_RenderWindow.setMouseCursorVisible(false);
    e->m_Level.SaveNpcs(e->m_Level.getName());
    SaveWaypoints(e);
}

void EditNpcWaypoint::HandleEvents(Engine* e) {
    int colorr = 0, colorg = 0, colorb = 0, colora = 0;
    sf::Event event;
    while (e->m_RenderWindow.pollEvent(event)) {
        switch(event.type) {
        case sf::Event::Closed:
            e->Quit();
            break;
        case sf::Event::TextEntered:
            switch(m_mode) {
            case MODE_NONE:
                if(event.text.unicode == 27) e->PopState();
                break;
            case MODE_EDIT:
                switch(event.text.unicode) {
                case 27:
                    m_Boxes.removeBox("EditBox");
                    m_mode = MODE_NONE;
                    break;
                case 127:
                    for(auto it = e->m_Level.Npcs.begin(); it != e->m_Level.Npcs.end();) {
                        if(m_npcPtr && (!m_npcPtr->m_grabbed) && it->ID == m_npcPtr->ID) {
                            e->m_Level.Npcs.erase(it);
                            m_npcPtr = nullptr;
                            m_Boxes.removeBox("EditBox");
                            m_mode = MODE_NONE;
                        } else {
                            ++it;
                        }
                    }
                    break;
                case 49:
                    m_inputField = FIELD_NAME;
                    m_mode = MODE_INPUT;
                    m_tip = "Edit the NPC's name.";
                    break;
                case 50:
                    m_inputField = FIELD_TAG;
                    m_mode = MODE_INPUT;
                    m_tip = "Edit the NPC's tag.";
                    break;
                case 51:
                    m_inputField = FIELD_POSX;
                    m_mode = MODE_INPUT;
                    m_tip = "Edit the NPC's X position.";
                    break;
                case 52:
                    m_inputField = FIELD_POSY;
                    m_mode = MODE_INPUT;
                    m_tip = "Edit the NPC's Y position.";
                    break;
                case 53:
                    m_inputField = FIELD_MOVEMODE;
                    m_mode = MODE_INPUT;
                    m_tip = "Edit the NPC's move mode:\n\t0\tMOVE_STATIONARY\n\t1\tMOVE_PATROL_CIRCLE\n\t2\tMOVE_PATROL_BACK_AND_FORTH\n\t3\tMOVE_MARCHING_IN_PLACE\n\t4\tMOVE_FOLLOW\n\t5\tMOVE_RANDOM";
                    break;
                case 54:
                    m_inputField = FIELD_AVATAR;
                    m_mode = MODE_INPUT;
                    m_tip = "Edit the NPC's avatar.";
                    break;
                case 55:
                    m_inputField = FIELD_SPEED;
                    m_mode = MODE_INPUT;
                    m_tip = "Edit the NPC's speed modifier.";
                    break;
                case 56:
                    m_inputField = FIELD_FACING;
                    m_mode = MODE_INPUT;
                    m_tip = "Edit the NPC's avatar:\n\t0\tDIR_SOUTH\n\t1\tDIR_WEST\n\t2\tDIR_EAST\n\t3\tDIR_NORTH";
                    break;
                case 57:
                    m_inputField = FIELD_COLOR;
                    m_mode = MODE_INPUT;
                    m_tip = "Edit the NPC's 3-part color and alpha, seperated by spaces.\n\t1st Value:\tRed (0-255)\n\t2nd Value:\tGreen (0-255)\n\t3rd Value:\tBlue (0-255)\n\t4th Value:\tAlpha (0-255)";
                    break;
                case 48:
                    m_inputField = FIELD_VAL1;
                    m_mode = MODE_INPUT;
                    m_tip = "There's no use for this value yet.";
                    break;
                case 45:
                    m_inputField = FIELD_VAL2;
                    m_mode = MODE_INPUT;
                    m_tip = "There's no use for this value yet.";
                    break;
                case 61:
                    m_inputField = FIELD_VAL3;
                    m_mode = MODE_INPUT;
                    m_tip = "There's no use for this value yet.";
                    break;
                default:
                    std::cout << "Unhandled keypress during EDIT_MODE: " << event.text.unicode << std::endl;
                    break;
                }
                if (m_mode == MODE_INPUT) {
                    m_Boxes.addBox("Input", sf::Vector2f(INPUT_BOX_WIDTH, INPUT_BOX_HEIGHT), sf::Vector2f(INPUT_BOX_LEFT, INPUT_BOX_TOP));
                    m_Boxes.getBox("Tips").getText().clear();
                    m_Boxes.getBox("Tips").addText(m_tip, sf::Vector2f(0, 10), 24);
                }
                break;
            case MODE_EDIT_WAYPOINT:
                switch(event.text.unicode) {
                case 27:
                    m_Boxes.removeBox("EditBoxWP");
                    m_mode = MODE_NONE;
                    m_wpPtr = nullptr;
                    break;
                case 127:
                    if(!m_wpPtr) break;

                    //std::cout << "Debug WP* Deletion:  X" << m_wpPtr->pos.x << " Y" << m_wpPtr->pos.y << std::endl;
                    /// Delete selected WP...
                    for (auto wp_set = e->m_Level.WaypointMap.begin(); wp_set != e->m_Level.WaypointMap.end(); ++wp_set) {
                        for(auto wp = wp_set->second.begin(); wp != wp_set->second.end();) {
                            //std::cout << "Debug Internal WP:  X" << wp->pos.x << " Y" << wp->pos.y << std::endl;
//                            if(wp->pos.x == m_wpPtr->pos.x && wp->pos.y == m_wpPtr->pos.y)
                            /* FIXED */
                            if(&(*wp) == m_wpPtr) {
                                //LOG("WP Match for deletion");
                                m_wpPtr = nullptr;
                                wp_set->second.erase(wp);
                                m_Boxes.removeBox("EditBoxWP");
                                m_mode = MODE_NONE;
                            } else ++wp;
                        }
                    }
                    break;
                case 49:
                    m_inputField = WP_FIELD_TAG;
                    m_mode = MODE_INPUT_WAYPOINT;
                    m_tip = "Edit the waypoints tag.";
                    break;
                case 50:
                    m_inputField = WP_FIELD_DIALOG;
                    m_mode = MODE_INPUT_WAYPOINT;
                    m_tip = "Edit the waypoints dialog tag.";
                    break;
                case 51:
                    m_inputField = WP_FIELD_POSX;
                    m_mode = MODE_INPUT_WAYPOINT;
                    m_tip = "Edit the waypoints X position manually.";
                    break;
                case 52:
                    m_inputField = WP_FIELD_POSY;
                    m_mode = MODE_INPUT_WAYPOINT;
                    m_tip = "Edit the waypoints Y position manually.";
                    break;
                case 53:
                    m_inputField = WP_FIELD_PAUSE;
                    m_mode = MODE_INPUT_WAYPOINT;
                    m_tip = "Edit the waypoints npc pause amount in seconds.";
                    break;
                case 54:
                    m_inputField = WP_FIELD_SPEED;
                    m_mode = MODE_INPUT_WAYPOINT;
                    m_tip = "Edit the waypoints npc speed modifier. (Default 1.0f)";
                    break;
                default:
                    std::cout << "Unhandled keypress during EDIT_MODE_WP: " << event.text.unicode << std::endl;
                    break;
                }
                if (m_mode == MODE_INPUT_WAYPOINT) {
                    m_Boxes.addBox("Input", sf::Vector2f(INPUT_BOX_WIDTH, INPUT_BOX_HEIGHT), sf::Vector2f(INPUT_BOX_LEFT, INPUT_BOX_TOP));
                    m_Boxes.getBox("Tips").getText().clear();
                    m_Boxes.getBox("Tips").addText(m_tip, sf::Vector2f(0, 10), 24);
                }
                break;
            case MODE_INPUT:
                if(event.text.unicode == 27) { /// Escape
                    m_Boxes.removeBox("Input");
                    m_Boxes.getBox("Tips").getText().clear();
                    m_input.clear();
                    m_mode = MODE_EDIT;
                } else if (event.text.unicode == 13) { /// Enter
                    std::stringstream ss;
                    if(m_input.empty()) m_input = "0";
                    ss.str(m_input);
                    m_input.clear();
                    float tempFloat;
                    switch(m_inputField) {
                    case FIELD_NAME:
                        m_npcPtr->name = ss.str();
                        break;
                    case FIELD_TAG:
                        m_npcPtr->tag = ss.str();
                        break;
                    case FIELD_POSX:
                        ss >> tempFloat;
                        m_npcPtr->sprite.setPosition(tempFloat, m_npcPtr->sprite.getPosition().y);
                        UpdateStartPosition(m_npcPtr);
                        break;
                    case FIELD_POSY:
                        ss >> tempFloat;
                        m_npcPtr->sprite.setPosition(m_npcPtr->sprite.getPosition().x, tempFloat);
                        UpdateStartPosition(m_npcPtr);
                        break;
                    case FIELD_MOVEMODE:
                        ss >> m_npcPtr->moveMode;
                        break;
                    case FIELD_AVATAR:
                        ss >> m_npcPtr->avatar;
                        if(m_npcPtr->avatar > 44) m_npcPtr->avatar = 44;
                        if(m_npcPtr->avatar < 0 ) m_npcPtr->avatar = 0;
                        break;
                    case FIELD_SPEED:
                        ss >> m_npcPtr->speed;
                        break;
                    case FIELD_FACING:
                        ss >> m_npcPtr->facing;
                        if(m_npcPtr->facing > 3) m_npcPtr->facing = 3;
                        if(m_npcPtr->facing < 0 ) m_npcPtr->facing = 0;
                        break;
                    case FIELD_COLOR:
                        ss >> colorr >> colorg >> colorb >> colora;
                        if(colorr > 255)    colorr = 255;
                        if(colorg > 255)    colorg = 255;
                        if(colorb > 255)    colorb = 255;
                        if(colora > 255)    colora = 255;
                        if(colorr <   0)    colorr = 0;
                        if(colorg <   0)    colorg = 0;
                        if(colorb <   0)    colorb = 0;
                        if(colora <   0)    colora = 0;
                        m_npcPtr->color = sf::Color(colorr, colorg, colorb, colora);
                        break;
                    case FIELD_VAL1:
                        ss >> m_npcPtr->value1;
                        break;
                    case FIELD_VAL2:
                        ss >> m_npcPtr->value2;
                        break;
                    case FIELD_VAL3:
                        ss >> m_npcPtr->value3;
                        break;
                    default:
                        std::cout << "Unknown Field" << std::endl;
                        break;
                    }
                    m_mode = MODE_EDIT;
                    m_Boxes.removeBox("Input");
                    m_Boxes.getBox("Tips").getText().clear();
                    m_npcPtr->UpdateAppearance();
                    RefreshEditBox(*m_npcPtr);
                } else if(event.text.unicode == 8) { /// Backspace
                    if (!m_input.empty()) m_input.resize(m_input.size() - 1);
                } else { /// Add a character to the input
                    if(m_mode == MODE_INPUT) m_input += static_cast<char>(event.text.unicode);
                }
                break;

            case MODE_INPUT_WAYPOINT:
                if(event.text.unicode == 27) { /// Escape
                    m_Boxes.removeBox("Input");
                    m_input.clear();
                    m_mode = MODE_EDIT_WAYPOINT;
                    m_Boxes.getBox("Tips").getText().clear();
                } else if (event.text.unicode == 13) { /// Enter
                    std::stringstream ss;
                    ss.str(m_input);
                    m_input.clear();
                    float tempFloat;
                    switch(m_inputField) {
                    case WP_FIELD_TAG:
                        m_wpPtr->tag = ss.str();
                        break;
                    case WP_FIELD_DIALOG:
                        m_wpPtr->dialog_trigger = ss.str();
                        break;
                    case WP_FIELD_POSX:
                        ss >> tempFloat;
                        m_wpPtr->pos.x = tempFloat;
                        break;
                    case WP_FIELD_POSY:
                        ss >> tempFloat;
                        m_wpPtr->pos.y = tempFloat;
                        break;
                    case WP_FIELD_PAUSE:
                        ss >> m_wpPtr->pause;
                        break;
                    case WP_FIELD_SPEED:
                        ss >> m_wpPtr->speedMod;
                        break;
                    default:
                        std::cout << "Unknown WP+Field" << std::endl;
                        break;
                    }
                    m_mode = MODE_EDIT_WAYPOINT;
                    m_Boxes.removeBox("Input");
                    m_Boxes.getBox("Tips").getText().clear();
                    //m_wpPtr->UpdateAppearance();
                    RefreshEditBoxWP(*m_wpPtr);
                } else if(event.text.unicode == 8) { /// Backspace
                    if (!m_input.empty()) m_input.resize(m_input.size() - 1);
                } else { /// Add a character to the input
                    if(m_mode == MODE_INPUT_WAYPOINT) m_input += static_cast<char>(event.text.unicode);
                }
                break;

            default:
                std::cout << "Unhandled mode during event::Text_Input" << std::endl;
                break;
            }
            break;

        case sf::Event::MouseMoved:
            if(m_npcPtr && m_npcPtr->m_grabbed) {
                m_npcPtr->sprite.setPosition(GetMouse(e));
            } else if (m_wpPtr && m_wpPtr->m_grabbed) {
                m_wpPtr->pos = GetMouse(e);
            }
            break;

        case sf::Event::MouseButtonPressed:
            if (event.mouseButton.button == sf::Mouse::Button::Left) {
                if(e->getTimeRunning().getElapsedTime().asSeconds() - m_LastClickTime.asSeconds() < DOUBLE_CLICK_TIME) CreateWaypoint(e);
                else GrabWaypoint(e);
            }
            if (event.mouseButton.button == sf::Mouse::Button::Right) {
                if(e->getTimeRunning().getElapsedTime().asSeconds() - m_LastClickTime.asSeconds() < DOUBLE_CLICK_TIME) CreateNpc(e);
                else GrabNpc(e);
            }
            break;

        case sf::Event::MouseButtonReleased:
            if (event.mouseButton.button == sf::Mouse::Button::Left) {
                DropWaypoint(e);
                if (GetMouse(e) == m_LastClickPos) EditWaypoint(e);
            }
            if (event.mouseButton.button == sf::Mouse::Button::Right) {
                DropNpc(e);
                if (GetMouse(e) == m_LastClickPos) EditNpc(e);
            }
            break;

        case sf::Event::MouseWheelScrolled:
            if(m_npcPtr && MODE_EDIT) {
                m_npcPtr->facing++;
                m_npcPtr->facing %= 4;
                m_npcPtr->UpdateAppearance();
            }
            break;
        default:
            break;
        }
    }
}

void EditNpcWaypoint::Update(Engine * e) {
    if(m_mode == MODE_NONE || m_mode == MODE_EDIT) MoveView(e);
    e->m_Level.Update(e->getTimeRunning(), m_viewPos, e->viewExtents);
    if (m_mode == MODE_INPUT || m_mode == MODE_INPUT_WAYPOINT) UpdateInput();
}

void EditNpcWaypoint::RefreshEditBox(Npc & npc) {
    if(m_Boxes.hasBox("EditBox")) m_Boxes.removeBox("EditBox");
    if(m_Boxes.hasBox("EditBoxWP")) m_Boxes.removeBox("EditBoxWP");
    m_Boxes.addBox("EditBox", sf::Vector2f(EDIT_BOX_WIDTH, EDIT_BOX_HEIGHT), sf::Vector2f(EDIT_BOX_LEFT, EDIT_BOX_TOP));

    std::stringstream ss;
    ss << "1 #" << npc.ID << " " << npc.name << std::endl;
    ss << "2 Tag    : " << npc.tag << std::endl;
    ss << "3 Pos.x  : " << npc.sprite.getPosition().x << std::endl;
    ss << "4 Pos.y  : " << npc.sprite.getPosition().y << std::endl;
    ss << "5 Mv Mode: " << getMoveModeString(npc.moveMode) << std::endl;
    ss << "6 Avatar : " << npc.avatar << std::endl;
    ss << "7 Speed  : " << npc.speed << std::endl;
    ss << "8 Facing : " << npc.facing << std::endl;
    ss << "9 Color:" << (int)npc.color.r << "," << (int)npc.color.g << "," << (int)npc.color.b << "," << (int)npc.color.a << std::endl;
    ss << "0 Value 1: " << npc.value1 << std::endl;
    ss << "- Value 2: " << npc.value2 << std::endl;
    ss << "= Value 3: " << npc.value3 << std::endl;

    m_Boxes.getBox("EditBox").addText(ss.str(), sf::Vector2f(10, 0), 32);
}

void EditNpcWaypoint::Draw(Engine * e) {
    e->m_RenderWindow.setView(e->m_View);
    e->m_Level.Draw(e->m_RenderWindow, true);
    DrawWaypoints(e);
    e->m_RenderWindow.setView(e->m_ViewBase);

    if(m_inputField == FIELD_AVATAR && m_mode == MODE_INPUT) {
        //std::cout << "Tip Sheet" << std::endl;
        e->m_RenderWindow.draw(avatar_sheet);
    }

    m_Boxes.DrawBoxes(e->m_RenderWindow);
}

void EditNpcWaypoint::MoveView(Engine * e) {
    m_viewPos = e->m_View.getCenter();
    float moveDelta = (e->getTimeRunning().getElapsedTime().asSeconds() - m_delta.asSeconds()) * MAP_EDIT_MOVE_SPEED;
    m_delta = sf::microseconds(e->getTimeRunning().getElapsedTime().asMicroseconds());

    using kb = sf::Keyboard;

    if(kb::isKeyPressed(kb::Up)    || kb::isKeyPressed(kb::W)) m_viewPos.y -= moveDelta;
    if(kb::isKeyPressed(kb::Down)  || kb::isKeyPressed(kb::S)) m_viewPos.y += moveDelta;
    if(kb::isKeyPressed(kb::Left)  || kb::isKeyPressed(kb::A)) m_viewPos.x -= moveDelta;
    if(kb::isKeyPressed(kb::Right) || kb::isKeyPressed(kb::D)) m_viewPos.x += moveDelta;

    if(m_viewPos.x < 1000) m_viewPos.x = 1000;
    if(m_viewPos.y < 1000) m_viewPos.y = 1000;

    e->m_View.setCenter(m_viewPos);
}

std::string EditNpcWaypoint::getModeString(int mode) {
    switch (mode) {
    case MODE_NONE:
        return "None";
        break;
    case MODE_EDIT:
        return "Edit";
        break;
    case MODE_INPUT:
        return "Input";
        break;
    case MODE_EDIT_WAYPOINT:
        return "Edit Waypoint";
        break;
    case MODE_INPUT_WAYPOINT:
        return "Input Waypoint";
        break;
    default:
        return "Unknown";
        break;
    }
    return std::string();
}

std::string EditNpcWaypoint::getMoveModeString(int mode) {
    switch (mode) {
    case Npc::MOVE_STATIONARY:
        return "Stationary";
        break;
    case Npc::MOVE_PATROL_CIRCLE:
        return "Circular Patrol";
        break;
    case Npc::MOVE_PATROL_BACK_AND_FORTH:
        return "Back and Forth";
        break;
    case Npc::MOVE_MARCHING_IN_PLACE:
        return "March in Place";
        break;
    case Npc::MOVE_FOLLOW:
        return "Following";
        break;
    case Npc::MOVE_RANDOM:
        return "Random Walk";
        break;
    default:
        return "Unrecognized Move Mode";
        break;
    }
    return std::string();
}

void EditNpcWaypoint::UpdateInput() {
    m_Boxes.getBox("Input").getText().clear();
    m_Boxes.getBox("Input").addText(m_input, sf::Vector2f(10, 0), 32);
}

void EditNpcWaypoint::UpdateStartPosition(Npc* ptr) {
    ptr->OriginalPosition = ptr->sprite.getPosition();
}

void EditNpcWaypoint::GrabNpc(Engine *e) {
    std::cout << "Single Right Click Pressed" << std::endl;
    /// Single Right Click Press (Select)
    m_LastClickPos = GetMouse(e);
    bool found = false;
    for(auto& npc : e->m_Level.Npcs) {
        if(npc.sprite.getGlobalBounds().contains(e->m_RenderWindow.mapPixelToCoords(sf::Mouse::getPosition(e->m_RenderWindow), e->m_View))) {
            found = true;
            npc.m_grabbed = true;
            m_npcPtr      = &npc;
            if(m_mode == MODE_EDIT) RefreshEditBox(npc);
        }
    }
    if(!found) {
        //m_npcPtr = nullptr;
        m_Boxes.removeBox("EditBox");
        m_mode = MODE_NONE;
    }
    m_LastClickTime = e->getTimeRunning().getElapsedTime();
}

void EditNpcWaypoint::EditNpc(Engine *e) {
    std::cout << "Single Right Click Released" << std::endl;
    /// Single Right Click
    if(m_npcPtr) {
        m_mode = MODE_EDIT;
        m_npcPtr->m_grabbed = false;
        RefreshEditBox(*m_npcPtr);
    }
    m_LastClickTime = e->getTimeRunning().getElapsedTime();
}

sf::Vector2f EditNpcWaypoint::GetMouse(Engine *e) {
    return e->m_RenderWindow.mapPixelToCoords(sf::Mouse::getPosition(e->m_RenderWindow), e->m_View);
}

void EditNpcWaypoint::DropNpc(Engine *e) {
    if(m_npcPtr) {
        m_npcPtr->m_grabbed = false;
        if(m_npcPtr->moveMode == Npc::MOVE_STATIONARY) m_npcPtr->OriginalPosition = GetMouse(e);
    }
}

void EditNpcWaypoint::CreateNpc(Engine *e) {
    /// Double Right Click (Create New NPC)
    Npc newNpc;
    newNpc.sprite.setTexture(e->m_Level.npc_texture);
    newNpc.OriginalPosition = e->m_RenderWindow.mapPixelToCoords(sf::Mouse::getPosition(e->m_RenderWindow), e->m_View);
    newNpc.sprite.setPosition(newNpc.OriginalPosition);
    newNpc.ID = (e->m_Level.Npcs.end() - 1)->ID + 1; // Last NPC's ID + 1
    newNpc.m_zLevel = e->m_Level.z_Level;

    if(m_npcPtr) {
        newNpc.name     = m_npcPtr->name;
        newNpc.tag      = m_npcPtr->tag;
        newNpc.avatar   = m_npcPtr->avatar;
        newNpc.facing   = m_npcPtr->facing;
        newNpc.color.r  = m_npcPtr->color.r;
        newNpc.color.g  = m_npcPtr->color.g;
        newNpc.color.b  = m_npcPtr->color.b;
        newNpc.color.a  = m_npcPtr->color.a;
        newNpc.moveMode = m_npcPtr->moveMode;
        newNpc.waypoints = m_npcPtr->waypoints;
    } else { // No previous npc pointed to
        newNpc.name     = "New Guy";
        newNpc.tag      = "NEWGUY";
        newNpc.avatar   = 0;
        newNpc.facing   = DIR_SOUTH;
        newNpc.color.r  = 255;
        newNpc.color.g  = 255;
        newNpc.color.b  = 255;
        newNpc.color.a  = 255;
        newNpc.moveMode = 0;
    }
    newNpc.UpdateAppearance();
    e->m_Level.Npcs.push_back(newNpc);
}

void EditNpcWaypoint::LoadWaypoints(Engine *e) {
    waypoint.setRadius(3);
    waypoint.setOutlineColor(sf::Color::Black);
    waypoint.setFillColor(sf::Color::White);
    waypoint.setOutlineThickness(1);
    waypoint.setOrigin(sf::Vector2f(3, 3));
}

void EditNpcWaypoint::SaveWaypoints(Engine *e) {
    e->m_Level.SaveWaypoints(e->m_Level.getName());
}

void EditNpcWaypoint::GrabWaypoint(Engine *e) {
    m_LastClickPos = GetMouse(e);
    m_wpPtr = nullptr;

    for (auto wp_set = e->m_Level.WaypointMap.begin(); wp_set != e->m_Level.WaypointMap.end(); ++wp_set) {
        for(auto wp = wp_set->second.begin(); wp != wp_set->second.end(); ++wp) {
            if(Tool::dist(m_LastClickPos, wp->pos) < 3.0) {
                wp->m_grabbed = true;
                m_wpPtr = &(*wp);
                if(m_mode == MODE_EDIT_WAYPOINT) RefreshEditBoxWP(*wp);
            }
        }
    }
    if(!m_wpPtr) {
        m_Boxes.removeBox("EditBoxWP");
        m_mode = MODE_NONE;
    }
    m_LastClickTime = e->getTimeRunning().getElapsedTime();
}

void EditNpcWaypoint::DropWaypoint(Engine *e) {
    if(m_wpPtr) {
        m_wpPtr->m_grabbed = false;
        m_wpPtr->pos = GetMouse(e);

        /// Refresh NPC waypoints
        for(auto it = e->m_Level.Npcs.begin(); it != e->m_Level.Npcs.end(); ++it) {
            if(it->tag == m_wpPtr->tag) {
                std::cout << "Matching Tag Found: " << it->tag << std::endl;
                it->waypoints = e->m_Level.WaypointMap[m_wpPtr->tag];
            }
        }
    }
}

void EditNpcWaypoint::CreateWaypoint(Engine *e) {
    Waypoint newWP;

    newWP.tag               = "NEW_WAYPOINT";
    newWP.dialog_trigger    = "NONE";
    newWP.m_zLevel          = e->m_Level.z_Level;
    newWP.pos               = GetMouse(e);
    newWP.pause             = 0.0f;
    newWP.speedMod          = 1.0f;

    e->m_Level.WaypointMap[newWP.tag].push_back(newWP);

    std::cout << "New Waypoint Added: " << newWP.tag << std::endl;
}

void EditNpcWaypoint::EditWaypoint(Engine *e) {
    sf::Vector2f clickPos = GetMouse(e);
    m_wpPtr = nullptr;
    for (auto wp_set = e->m_Level.WaypointMap.begin(); wp_set != e->m_Level.WaypointMap.end(); ++wp_set) {
        for(auto wp = wp_set->second.begin(); wp != wp_set->second.end(); ++wp) {
            if(Tool::dist(clickPos, wp->pos) < 3.0) m_wpPtr = &(*wp);
        }
    }

    if(m_wpPtr) {
        std::cout << "Waypoint found: " << m_wpPtr->tag << std::endl;
        m_mode = MODE_EDIT_WAYPOINT;
        m_wpPtr->m_grabbed = false;
        RefreshEditBoxWP(*m_wpPtr);
    }
    m_LastClickTime = e->getTimeRunning().getElapsedTime();
}

void EditNpcWaypoint::DrawWaypoints(Engine *e) {
    sf::Color color;
    m_wp_color = 0;
    sf::VertexArray line(sf::Lines, 2);
//    line[1].color = sf::Color::White;

    for (auto wp_set = e->m_Level.WaypointMap.begin(); wp_set != e->m_Level.WaypointMap.end(); ++wp_set) {
        // If the zlevel of the wp isn't on this level, skip
        if(wp_set->second.begin()->m_zLevel != e->m_Level.z_Level) continue;

        m_wp_color %= 6;
        switch (m_wp_color) {
        case 0:
            color = sf::Color::Blue;
            break;
        case 1:
            color = sf::Color::Cyan;
            break;
        case 2:
            color = sf::Color::Green;
            break;
        case 3:
            color = sf::Color::Magenta;
            break;
        case 4:
            color = sf::Color::Red;
            break;
        case 5:
            color = sf::Color::Yellow;
            break;
        default:
            std::cout << "Waypoint color error!" << std::endl;
            break;
        }
        waypoint.setFillColor(color);

        for(auto wp = wp_set->second.begin(); wp != wp_set->second.end(); ++wp) {
            waypoint.setPosition(wp->pos);
            e->m_RenderWindow.draw(waypoint);

            // Check for a next WP. If it exists, draw a line to it.
            if(wp + 1  != wp_set->second.end()) {
                line[0].color = color;
                line[1].color = color;
                line[0].position = wp->pos;
                line[1].position = (wp + 1)->pos;
                e->m_RenderWindow.draw(line);
            } else { // The next wp is the end of the array, there are no more wps.
                if(wp_set->second.size() > 2) { // Draw a line back to zero.
                    line[0].color = sf::Color::Black;
                    line[1].color = sf::Color::Black;
                    line[0].position = wp->pos;
                    line[1].position = wp_set->second.begin()->pos;
                    e->m_RenderWindow.draw(line);
                }
            }
        }
        m_wp_color++;
    }
}

void EditNpcWaypoint::RefreshEditBoxWP(Waypoint& waypoint) {
    if(m_Boxes.hasBox("EditBox")) m_Boxes.removeBox("EditBox");
    if(m_Boxes.hasBox("EditBoxWP")) m_Boxes.removeBox("EditBoxWP");
    m_Boxes.addBox("EditBoxWP", sf::Vector2f(EDIT_BOX_WIDTH, EDIT_BOX_HEIGHT), sf::Vector2f(1920 - (EDIT_BOX_LEFT + EDIT_BOX_WIDTH), EDIT_BOX_TOP));

    std::stringstream ss;
    ss << "1 Tag    : " << waypoint.tag << std::endl;
    ss << "2 Dialog : " << waypoint.dialog_trigger << std::endl;
    ss << "3 Pos.x  : " << waypoint.pos.x << std::endl;
    ss << "4 Pos.y  : " << waypoint.pos.y << std::endl;
    ss << "5 Pause  : " << waypoint.pause << std::endl;
    ss << "6 Speed  : " << waypoint.speedMod << std::endl;

    m_Boxes.getBox("EditBoxWP").addText(ss.str(), sf::Vector2f(10, 0), 32);
}
