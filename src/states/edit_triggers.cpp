#include "states/edit_triggers.h"

EditTriggers EditTriggers::m_EditTriggers;

void EditTriggers::Init(Engine* eng) {
    e = eng;
    m_viewPos = e->getParty().getPos();
    m_Boxes.getBoxes().clear();
    UpdateHud();
    DrawTriggers();
}

void EditTriggers::HandleEvents(Engine* eng) {
    // Process events
    sf::Event event;
    while (e->m_RenderWindow.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            e->Quit();
        } else if (event.type == sf::Event::KeyPressed) {
            switch(event.key.code) {
            case sf::Keyboard::Escape:
                e->PopState();
                break;

            case sf::Keyboard::Q:
                e->Quit();
                break;

            case sf::Keyboard::Delete:
                if(m_pCurrentTrigger) {
                    for(auto it = e->m_Level.getAllTriggers().begin(); it != e->m_Level.getAllTriggers().end(); /* Left out increment on purpose. */) {
                        if( m_pCurrentTrigger != nullptr &&
                                it->Tag == m_pCurrentTrigger->Tag &&
                                it->Area.top == m_pCurrentTrigger->Area.top &&
                                it->Area.left == m_pCurrentTrigger->Area.left &&
                                it->Area.width == m_pCurrentTrigger->Area.width &&
                                it->Area.height == m_pCurrentTrigger->Area.height &&
                                it->ZLevel == m_pCurrentTrigger->ZLevel) {
                            std::cout << "Match at: " << m_pCurrentTrigger->Tag << std::endl;
                            it = e->m_Level.getAllTriggers().erase(it);
                            m_pCurrentTrigger = nullptr;
                        } else {
                            ++it;
                        }
                    }
                    DrawTriggers();
                }
                break;

            default:
                break;
            }
        } else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Button::Left) {
                LeftMouseButtonPressed();
            }
            if (event.mouseButton.button == sf::Mouse::Button::Right) {
                RightMouseButtonPressed();
            }
        } else if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Button::Left) {
                LeftMouseButtonReleased();
            } else if (event.mouseButton.button == sf::Mouse::Button::Right) {
                RightMouseButtonReleased();
            }
        } else if (event.type == sf::Event::MouseMoved) {
            MouseMoved();
        }
    }
}

void EditTriggers::Update(Engine* eng) {
    m_mousePos = sf::Mouse::getPosition(e->m_RenderWindow);
    m_worldPos = e->m_RenderWindow.mapPixelToCoords(sf::Mouse::getPosition(e->m_RenderWindow), e->m_View);

    UpdateHud();
    UpdateCamera();

    e->m_View.setCenter(m_viewPos);
    e->m_Level.Update(e->getTimeRunning(), m_viewPos, e->viewExtents);
}

void EditTriggers::Draw(Engine* eng) {
    e->m_RenderWindow.setView(e->m_View);
    e->m_Level.Draw(e->m_RenderWindow);

    for(const auto& r : m_Rectangles) e->m_RenderWindow.draw(r);

    e->m_RenderWindow.setView(e->m_ViewBase);
    m_Boxes.DrawBoxes(e->m_RenderWindow);
}

void EditTriggers::UpdateCamera() {
    float moveDelta = (e->getTimeRunning().getElapsedTime().asSeconds() - m_delta.asSeconds()) * MAP_EDIT_MOVE_SPEED;
    m_delta = e->getTimeRunning().getElapsedTime();

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))     m_viewPos.y -= moveDelta;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))   m_viewPos.y += moveDelta;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))   m_viewPos.x -= moveDelta;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))  m_viewPos.x += moveDelta;

    if(m_viewPos.x < 1000) m_viewPos.x = 1000;
    if(m_viewPos.y < 1000) m_viewPos.y = 1000;
}

void EditTriggers::UpdateHud() {
    m_Boxes.removeBox("Desc");
    m_Boxes.addBox("Desc", sf::Vector2f(100, 100), sf::Vector2f(50,50), Box::BOX_NONE).addText("Trigger Editor", sf::Vector2f(0,0), 36);
    std::stringstream ss;
    ss << "Use the mouse to edit trigger areas." << std::endl;
    if(m_pCurrentTrigger) {
        ss << std::endl << "Currently Selected Trigger:" << std::endl << m_pCurrentTrigger->Tag << std::endl;
        sf::RectangleShape Shade;
        Shade.setFillColor(sf::Color(0, 0, 0, 200));
        Shade.setPosition(35, 178);
        Shade.setSize(sf::Vector2f(550, 100));
        m_Boxes.getBox("Desc").getRectangles().push_back(Shade);
    }
    m_Boxes.getBox("Desc").addText(ss.str(), sf::Vector2f(0,50), 36);
}

void EditTriggers::Cleanup(Engine* eng) {
    e->m_Level.SaveTriggers(e->m_Level.getName());
    e->getParty().MovePartyTo(m_viewPos);
    m_pCurrentTrigger   = nullptr;
    m_currentCorner     = CORNER_NONE;
    m_dragging          = false;
    m_Rectangles.clear();
}

void EditTriggers::LeftMouseButtonPressed() {
    sf::Vector2f WorldPos = e->m_RenderWindow.mapPixelToCoords(sf::Mouse::getPosition(e->m_RenderWindow), e->m_View);
    m_pCurrentTrigger = nullptr;

    for(auto& t : e->m_Level.getAllTriggers()) {
        if(e->m_Level.z_Level == t.ZLevel && t.Area.contains(WorldPos)) {
            /// Set the pointer
            m_pCurrentTrigger = &t;
            break;
        }
    }

    /* If you're not in a trigger, simply ignore the keypress */
    if(!m_pCurrentTrigger) return;

    /* If you're inside a trigger, determine where. */
    //std::cout << "LMB Pressed" << std::endl;
    m_dragging = true;

    sf::RectangleShape triggerCorners;
    triggerCorners.setSize(sf::Vector2f(10, 10));
    sf::RectangleShape tr_NW, tr_NE, tr_SW, tr_SE;
    tr_NW = triggerCorners;
    tr_NE = triggerCorners;
    tr_SW = triggerCorners;
    tr_SE = triggerCorners;

    tr_NW.setPosition(m_pCurrentTrigger->Area.left, m_pCurrentTrigger->Area.top);
    tr_NE.setPosition(m_pCurrentTrigger->Area.left + m_pCurrentTrigger->Area.width - 10, m_pCurrentTrigger->Area.top);
    tr_SW.setPosition(m_pCurrentTrigger->Area.left, m_pCurrentTrigger->Area.top + m_pCurrentTrigger->Area.height - 10);
    tr_SE.setPosition(m_pCurrentTrigger->Area.left + m_pCurrentTrigger->Area.width - 10, m_pCurrentTrigger->Area.top + m_pCurrentTrigger->Area.height - 10);

    if(tr_NW.getGlobalBounds().contains(WorldPos)) {
        m_currentCorner = CORNER_NW;
    } else if(tr_NE.getGlobalBounds().contains(WorldPos)) {
        m_currentCorner = CORNER_NE;
    } else if(tr_SW.getGlobalBounds().contains(WorldPos)) {
        m_currentCorner = CORNER_SW;
    } else if(tr_SE.getGlobalBounds().contains(WorldPos)) {
        m_currentCorner = CORNER_SE;
    } else {
        m_currentCorner = CORNER_NONE;
    }
}

void EditTriggers::LeftMouseButtonReleased() {
    //std::cout << "LMB Released" << std::endl;
    m_dragging = false;
}

void EditTriggers::Resume(Engine* eng) {
    //std::cout << "EditTriggers Resume" << std::endl;
    if(e->getInputString() == "CaNcElEd") {
        e->m_Level.getAllTriggers().pop_back();
        m_pCurrentTrigger = nullptr;
    } else {
        m_pCurrentTrigger->Tag = e->getInputString();
    }
    DrawTriggers();
}

void EditTriggers::RightMouseButtonPressed() {
    sf::Vector2f WorldPos = e->m_RenderWindow.mapPixelToCoords(sf::Mouse::getPosition(e->m_RenderWindow), e->m_View);
    Trigger newTrigger;

    e->setInputString("Enter the tag of the new trigger.:Tags cannot contain spaces. Typically the string will be in all capitals. It is case sensative, TRIGGER != Trigger.");
    e->PushState(Input_String::Instance());

    newTrigger.Tag = Tool::generateName(12);
    newTrigger.ZLevel = e->m_Level.z_Level;
    newTrigger.Area.top = WorldPos.y - 50;
    newTrigger.Area.left = WorldPos.x - 50;
    newTrigger.Area.width = 100;
    newTrigger.Area.height = 100;
    e->m_Level.getAllTriggers().push_back(newTrigger);
    m_pCurrentTrigger = &e->m_Level.getAllTriggers().back();
}

void EditTriggers::RightMouseButtonReleased() {
}

void EditTriggers::MouseMoved() {
    sf::Vector2f WorldPos = e->m_RenderWindow.mapPixelToCoords(sf::Mouse::getPosition(e->m_RenderWindow), e->m_View);
    sf::Vector2f delta = m_prevPos - WorldPos;

    if(m_dragging && m_pCurrentTrigger) {

        if(m_currentCorner == CORNER_NONE) {
            m_pCurrentTrigger->Area.left    = m_pCurrentTrigger->Area.left - delta.x;
            m_pCurrentTrigger->Area.top     = m_pCurrentTrigger->Area.top - delta.y;
        } else if(m_currentCorner == CORNER_NW) {
            if ((m_pCurrentTrigger->Area.left   - delta.x) > 31) m_pCurrentTrigger->Area.left     = m_pCurrentTrigger->Area.left   - delta.x;
            if ((m_pCurrentTrigger->Area.top    - delta.y) > 31) m_pCurrentTrigger->Area.top      = m_pCurrentTrigger->Area.top    - delta.y;
            if ((m_pCurrentTrigger->Area.width  + delta.x) > 31) m_pCurrentTrigger->Area.width    = m_pCurrentTrigger->Area.width  + delta.x;
            if ((m_pCurrentTrigger->Area.height + delta.y) > 31) m_pCurrentTrigger->Area.height   = m_pCurrentTrigger->Area.height + delta.y;
        } else if(m_currentCorner == CORNER_NE) {
            if (m_pCurrentTrigger->Area.height + delta.y > 31) m_pCurrentTrigger->Area.top      = m_pCurrentTrigger->Area.top    - delta.y;
            if (m_pCurrentTrigger->Area.width  - delta.x > 31) m_pCurrentTrigger->Area.width    = m_pCurrentTrigger->Area.width  - delta.x;
            if (m_pCurrentTrigger->Area.height + delta.y > 31) m_pCurrentTrigger->Area.height   = m_pCurrentTrigger->Area.height + delta.y;
        } else if(m_currentCorner == CORNER_SW) {
            if (m_pCurrentTrigger->Area.width  + delta.x > 31) m_pCurrentTrigger->Area.left     = m_pCurrentTrigger->Area.left   - delta.x;
            if (m_pCurrentTrigger->Area.width  + delta.x > 31) m_pCurrentTrigger->Area.width    = m_pCurrentTrigger->Area.width  + delta.x;
            if (m_pCurrentTrigger->Area.height - delta.y > 31) m_pCurrentTrigger->Area.height   = m_pCurrentTrigger->Area.height - delta.y;
        } else if(m_currentCorner == CORNER_SE) {
            if (m_pCurrentTrigger->Area.width  - delta.x > 31) m_pCurrentTrigger->Area.width    = m_pCurrentTrigger->Area.width  - delta.x;
            if (m_pCurrentTrigger->Area.height - delta.y > 31) m_pCurrentTrigger->Area.height   = m_pCurrentTrigger->Area.height - delta.y;
        }
    }
    m_prevPos = WorldPos;
    DrawTriggers();
}

void EditTriggers::DrawTriggers() {
    sf::Vector2f WorldPos = e->m_RenderWindow.mapPixelToCoords(sf::Mouse::getPosition(e->m_RenderWindow), e->m_View);
    m_Rectangles.clear();
    sf::RectangleShape tempRect;
    //m_pCurrentTrigger = nullptr;

    for(auto& t : e->m_Level.getAllTriggers()) {
        if(e->m_Level.z_Level == t.ZLevel) {
            tempRect.setFillColor(sf::Color(255,0,0,64));
            tempRect.setOutlineColor(sf::Color::Red);
            tempRect.setPosition(sf::Vector2f(t.Area.left, t.Area.top));
            tempRect.setSize(sf::Vector2f(t.Area.width, t.Area.height));
            tempRect.setOutlineThickness(1);

            if(!m_dragging && t.Area.contains(WorldPos)) {
                /// Highlight the Trigger
                tempRect.setFillColor(sf::Color(255,255,0,64));
                tempRect.setOutlineColor(sf::Color::Yellow);
                m_Rectangles.push_back(tempRect);

                /// Add Corners (size, then position)
                tempRect.setFillColor(sf::Color::Blue);
                tempRect.setOutlineThickness(0);
                tempRect.setSize(sf::Vector2f(10.0, 10.0));
                tempRect.setPosition(t.Area.left + 1, t.Area.top + 1);
                m_Rectangles.push_back(tempRect);
                tempRect.setPosition(t.Area.left + t.Area.width - 10 - 1, t.Area.top + 1);
                m_Rectangles.push_back(tempRect);
                tempRect.setPosition(t.Area.left + 1, t.Area.top + t.Area.height - 10 - 1);
                m_Rectangles.push_back(tempRect);
                tempRect.setPosition(t.Area.left + t.Area.width - 10 - 1, t.Area.top + t.Area.height - 10 - 1);
                m_Rectangles.push_back(tempRect);

//                /// Set the pointer?
//                m_pCurrentTrigger = &t;
            } else {
                m_Rectangles.push_back(tempRect);
            }
        }
    }
}
