#include "states/minimenu_edit.h"

MiniMenu_Edit MiniMenu_Edit::m_MiniMenu_Edit;

void MiniMenu_Edit::Init(Engine* e) {
    m_Boxes.removeBox("Menu");

    sf::View tempView;
    tempView.reset(sf::FloatRect(0, 0, 1920, 1080));
    e->m_RenderWindow.setView(tempView);

    m_Boxes.getBoxes().clear();
    frameRates = std::vector<int>(FPS_SAMPLE_SIZE, 0);

    m_Boxes.addBox("Hud", sf::Vector2f(800, 157), sf::Vector2f(50, 900));
    m_Boxes.Menu({"Map & Beacon", "NPC & Waypoint", "Item", "Enemy & Encounter", "Animation"}, sf::Vector2f(100.0f, 100.0f));
}

void MiniMenu_Edit::HandleEvents(Engine* e) {
    // Process events
    sf::Event event;
    while (e->m_RenderWindow.pollEvent(event)) {
        // Close window : exit
        if (event.type == sf::Event::Closed) {
            e->Quit();
        }
        if (event.type == sf::Event::KeyPressed) {
            switch(event.key.code) {
            case sf::Keyboard::Escape:
                m_Boxes.PlayCancel();
                e->PopState();
                break;

            case sf::Keyboard::Up:
            case sf::Keyboard::W:
                m_Boxes.PlaySelect();
                SelectUp();
                break;

            case sf::Keyboard::Down:
            case sf::Keyboard::S:
                m_Boxes.PlaySelect();
                SelectDown();
                break;

            case sf::Keyboard::Return:
                m_Boxes.PlayConfirm();
                Confirm(e);
                break;

            case sf::Keyboard::Q:
                e->Quit();
                break;

            case sf::Keyboard::F:
                m_sync = !m_sync;
                e->m_RenderWindow.setVerticalSyncEnabled(m_sync);
                break;

            case sf::Keyboard::H:
                for (auto& p : e->getParty().getMembers()) {
                    p.setCurHP(p.getMaxHP());
                    p.setCurMP(p.getMaxMP());
                }
                break;

            case sf::Keyboard::F3:
                m_Boxes.PlaySelect();
                e->getParty().RemoveMember();
                break;

            case sf::Keyboard::F4:
                m_Boxes.PlaySelect();
                e->getParty().AddMember(Tool::getRand(0, 40), 1, Tool::getRand(1, 4), Tool::generateName());
                break;

            case sf::Keyboard::LBracket:
                m_Boxes.PlaySelect();
                e->m_Level.z_Level--;
                break;

            case sf::Keyboard::RBracket:
                m_Boxes.PlaySelect();
                e->m_Level.z_Level++;
                break;

            default:
                break;
            }
        }
    }
}

void MiniMenu_Edit::Update(Engine* e) {
    // Center the View
    e->m_View.setCenter(e->getParty().getPos());

    // Update the Level
    e->m_Level.Update(e->getTimeRunning(), e->getParty().getPos(), e->viewExtents);

    // Update the hud.
    Update_Hud(e);
}

void MiniMenu_Edit::Draw(Engine* e) {
    // Set the View
    e->m_RenderWindow.setView(e->m_View);

    // Draw the Level
    e->m_Level.Draw(e->m_RenderWindow);

    // Draw the Party
    e->getParty().Draw(e->m_RenderWindow);

    // Reset the View
    e->m_RenderWindow.setView(e->m_ViewBase);

    // Draw the HUD
//    Hud.setPosition(52, 902);
//    Hud.setColor(sf::Color::Black);
//    e->m_RenderWindow.draw(Hud);
//
//    Hud.setPosition(50, 900);
//    Hud.setColor(sf::Color::White);
//    e->m_RenderWindow.draw(Hud);
//
    m_Boxes.DrawBoxes(e->m_RenderWindow);

    // Draw Arrow
    m_Boxes.drawArrow(sf::Vector2f(140.0f, 157.0f + (50.0f * m_choice)), 2, sf::Vector2f(1.0f, 1.0f), e->m_RenderWindow);
}

/// @brief Update the Hud with Playstate information.
/// @param e GameEngine pointer.
void MiniMenu_Edit::Update_Hud(Engine* e) {
    m_Boxes.getBox("Hud").getText().clear();
    HudString.clear();
    HudString.str(std::string());

    HudString << (int)e->getParty().getPos().x - OFFSET_TO_AVOID_NEGATIVES_X << ", "
              << (int)e->getParty().getPos().y - OFFSET_TO_AVOID_NEGATIVES_Y << " ";

    // Framerates
    prevFpsClock += ClockFramerate.getElapsedTime().asSeconds();
    frameRates.push_back((int)(1 / ClockFramerate.restart().asSeconds()));
    frameRates.erase(frameRates.begin());

    prevFpsClock += ClockFramerate.restart().asSeconds();
    if (prevFpsClock > 1) {
        prevFpsClock = 0;
        currFrameRate = std::accumulate(frameRates.begin(), frameRates.end(), 0.0) / frameRates.size();
    }

    HudString << " Actual:" << e->getParty().getPos().x << ", " << e->getParty().getPos().y << " (" << e->m_Level.z_Level << ")" << std::endl;
    HudString << "FPS: " << currFrameRate << "(Press 'f' to toggle Framelock)" << std::endl;
    HudString << Tool::ConvertMsToTimeString(e->getTimePlaying()) << std::endl;
    HudString << "Party Size: " << e->getParty().getSize();
    m_Boxes.getBox("Hud").addText(HudString.str(), sf::Vector2f(10, -8), 36);
}

void MiniMenu_Edit::Confirm(Engine* e) {
    //std::cout << "Confirmed Choice: " << m_choice << std::endl;
    switch(m_choice) {
    case 0:
        e->PushState( StateEditMap::Instance() );
        break;
    case 1:
        e->PushState( EditNpcWaypoint::Instance() );
        break;
    case 2:
        e->PushState( EditItem::Instance() );
        break;
    case 3:
        e->PushState( EditEnemy::Instance() );
        break;
    case 4:
        e->PushState( Edit_Anim::Instance() );
        break;
    default:
        break;
    }
}

void MiniMenu_Edit::SelectUp() {
    m_choice--;
    if (m_choice < 0) m_choice = m_Boxes.getNumberOfMenuChoices() - 1;
}

void MiniMenu_Edit::SelectDown() {
    m_choice++;
    if (m_choice > m_Boxes.getNumberOfMenuChoices() - 1) m_choice = 0;
}
